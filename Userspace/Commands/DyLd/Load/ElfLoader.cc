//
// Created by mek101 on 14/03/22.
//

#include <memory>

#include <Api/StdInt.h>
#include <Api/ELF32.h>
#include <Api/User.h>
#include <Api/FileSystem.h>
#include <LibTC/Collection/Vector.hh>

#include "ElfLoader.hh"
#include "ElfLoader.hhi"
#include "../ElfDynamicObject.hh"

namespace {
    ElfLoadResult<std::unique_ptr<Elf32Ehdr>> parse_header(i32 fd) {
        // Still unverified header.
        auto header_res = map_to_struct<Elf32Ehdr>(fd, 0);
        if ( header_res.is_error() ) {
            return header_res.unwrap_error();
        }
        auto header_ptr = header_res.unwrap_value();

        // Check magic
        if ( header_ptr->e_ident[0] != ELFMAG0 || header_ptr->e_ident[1] != ELFMAG1 ||
             header_ptr->e_ident[2] != ELFMAG2 || header_ptr->e_ident[3] != ELFMAG3 ) {
            return ElfLoadError::FormatError;
        }
        // Check type and version.
        if ( header_ptr->e_ident[EI_CLASS] == ELFCLASS32 ||
             header_ptr->e_ident[EI_DATA] == ELFDATA2LSB ||
             header_ptr->e_ident[EI_VERSION] == EV_CURRENT ) {
            return ElfLoadError::UnsupportedElfError;
        }
        // Check for the platform
        if ( header_ptr->e_version != EV_CURRENT ||
             // Position independent executables and shared libraries
             header_ptr->e_machine != ET_DYN ||
             header_ptr->e_machine != EM_386 ) {
            return ElfLoadError::UnsupportedElfError;
        }
        // Validate entries
        if ( header_ptr->e_phoff == 0 || header_ptr->e_shoff == 0 ||
             header_ptr->e_phnum == 0 || header_ptr->e_phentsize != sizeof(Elf32Phdr*) ||
             header_ptr->e_shnum == 0 || header_ptr->e_shentsize != sizeof(Elf32Shdr*)) {
            return ElfLoadError::UnsupportedElfError;
        }

        return header_ptr;
    }

    ElfLoadResult<TC::Collection::Vector<Elf32Phdr>> parse_program_headers(Elf32Ehdr& header, FileHandle fd, usize file_size) {
        auto filter = [](const Elf32Phdr& header) {
            // Filter loadable, dynamic, program header and thread local storage headers.
            return header.p_type == PT_LOAD || header.p_type == PT_DYNAMIC ||
                   header.p_type == PT_PHDR || header.p_type == PT_TLS;
        };
        auto sort = [](const Elf32Phdr& a, const Elf32Phdr& b){
            return a.p_offset < b.p_offset;
        };
        return parse_headers<Elf32Phdr>(fd, header.e_phoff, header.e_phentsize, header.e_phnum, file_size, filter, sort);
    }

    ElfLoadResult<TC::Collection::Vector<Elf32Shdr>> parse_section_headers(Elf32Ehdr& header, FileHandle fd, usize file_size) {
        auto filter = [](const Elf32Shdr& header) {
            // Filter out null, notes and reserved segments.
            return header.sh_type != SHT_NULL && header.sh_type != SHT_NOTE && header.sh_type != SHT_SHLIB;
        };
        auto sort = [](const Elf32Shdr& a, const Elf32Shdr& b) {
            return a.sh_offset < b.sh_offset;
        };
        return parse_headers<Elf32Shdr>(fd, header.e_shoff, header.e_shentsize, header.e_shnum, file_size, filter, sort);
    }

    void* allocate_at(usize address, usize alloc_size) {
        // Using this syscall until a user API is provided
        //return s_create_pages_in_spaces(, address, alloc_size);
    }

    ElfLoadResult<std::unique_ptr<u8[]>> allocate_program_segment(const FileHandle fd, const Elf32Phdr& header) {
        auto allocation = reinterpret_cast<u8*>(allocate_at(header.p_paddr, header.p_memsz));
        auto seek_res = s_seek(fd, header.p_offset, FS_SEEK_SET);
        if ( seek_res == -1 || seek_res != header.p_offset ) {
            delete allocation;
            return ElfLoadError::IOError;
        }
        auto read_res = s_read(fd, allocation, header.p_filesz);
        if ( read_res == -1 || read_res != header.p_filesz ) {
            delete allocation;
            return ElfLoadError::IOError;
        }

        auto zeroable_area = header.p_memsz - header.p_filesz;
        if ( zeroable_area > 0 ) {
            auto zeroable_ptr = allocation + zeroable_area;
            Memory::setWords(zeroable_ptr, 0x00, (i32)zeroable_area);
        }
        return std::unique_ptr<u8[]> { allocation };
    }

    ElfLoadResult<LoadedSegments> load_segments(TC::Collection::Vector<Elf32Phdr> prog_headers, FileHandle fd, usize file_size, const Elf32Ehdr& header) {
        auto loaded_segments = LoadedSegments {};

        for ( auto prog_header : prog_headers ) {
            if ( prog_header.p_type == PT_PHDR ) {
                if ( loaded_segments.program_header.has_value() ) {
                    // The program header table is unique per ELF.
                    return ElfLoadError::FormatError;
                }
                auto allocation = reinterpret_cast<u8*>(allocate_at(prog_header.p_offset, sizeof(Elf32Phdr)));
                *(reinterpret_cast<Elf32Ehdr*>(allocation)) = header;
                loaded_segments.program_header = SegmentInfo { prog_header, std::unique_ptr<u8[]>(allocation) };
            } else {
                auto allocation_res = allocate_program_segment(fd, prog_header);
                if ( allocation_res.is_error() ) {
                    return allocation_res.unwrap_error();
                }
                auto allocation = allocation_res.unwrap_value();
                switch ( prog_header.p_type ) {
                    case PT_LOAD: {
                        loaded_segments.load.push_back(SegmentInfo{ prog_header, std::move(allocation) });
                        break;
                    }
                    case PT_DYNAMIC: {
                        loaded_segments.dynamic.push_back(
                            SegmentInfo{ prog_header, std::move(allocation) });
                        break;
                    }
                    case PT_TLS:
                        loaded_segments.tls.push_back(SegmentInfo { prog_header, std::move(allocation) });
                        break;
                    default:
                        return ElfLoadError::UnknownLoadError;

                }
            }
        }
        return loaded_segments;
    }

    ElfLoadResult<LoadedSections> load_sections(TC::Collection::Vector<Elf32Shdr> sect_headers, FileHandle fd, usize file_size) {
        auto loaded_sections = LoadedSections {};

        for ( auto sect_header : sect_headers ) {
            switch ( sect_header.sh_type ) {
                case SHT_PROGBITS:
                    loaded_sections.program_bits.push_back(SectionInfo { sect_header });
                    break;
                case SHT_SYMTAB:
                    loaded_sections.symbol_table.push_back(SectionInfo { sect_header });
                    break;
                case SHT_STRTAB:
                    if ( loaded_sections.string_table.has_value() ) {
                        // The string table is unique per ELF.
                        return ElfLoadError::FormatError;
                    }
                    loaded_sections.string_table = SectionInfo { sect_header };
                    break;
                case SHT_RELA:
                    // No RELA sections in x86 ABI.
                    return ElfLoadError::FormatError;
                case SHT_HASH:
                    loaded_sections.hash.push_back(SectionInfo { sect_header });
                    break;
                case SHT_DYNAMIC:
                    if ( loaded_sections.dynamic.has_value() ) {
                        // The dynamic section is unique per ELF.
                        return ElfLoadError::FormatError;
                    }
                    loaded_sections.dynamic = SectionInfo { sect_header };
                    break;
                case SHT_NOBITS:
                    loaded_sections.no_bits.push_back(SectionInfo { sect_header });
                    break;
                case SHT_REL:
                    loaded_sections.relocation.push_back(SectionInfo { sect_header });
                    break;
                case SHT_DYNSYM:
                    loaded_sections.dynamic_symbols.push_back(SectionInfo { sect_header });
                    break;
                default:
                    return ElfLoadError::UnknownLoadError;
            }
        }
        return loaded_sections;
    }
}

ElfLoadResult<LoadedElf> try_load(std::string const& target_path) {
    // Open the file.
    auto target_fd = s_open_f(target_path.c_str(), FILE_FLAG_MODE_READ | FILE_FLAG_MODE_BINARY);
    if ( target_fd < 1 ) {
        return ElfLoadError::IOError;
    }

    // Get the size of the file for future checks.
    auto file_size = s_length(target_fd);
    if ( file_size <= sizeof(Elf32Ehdr) ) {
        return ElfLoadError::SizeError;
    }

    // Parse the header
    auto header_res = parse_header(target_fd);
    if ( header_res.is_error() ) {
        return header_res.unwrap_error();
    }
    auto header = header_res.unwrap_value();

    // Parse the segment headers.
    auto prog_headers_res = parse_program_headers(*header, target_fd, file_size);
    if ( prog_headers_res.is_error() ) {
        return prog_headers_res.unwrap_error();
    }
    auto prog_headers = prog_headers_res.unwrap_value();

    // Load the segments.
    auto loaded_segments_res = load_segments(prog_headers, target_fd, file_size, *header);
    if ( loaded_segments_res.is_error() ) {
        return loaded_segments_res.unwrap_error();
    }
    auto loaded_segments = loaded_segments_res.unwrap_value();

    // Parse the section headers.
    auto sect_headers_res = parse_section_headers(*header, target_fd, file_size);
    if ( sect_headers_res.is_error() ) {
        return sect_headers_res.unwrap_error();
    }
    auto sect_headers = sect_headers_res.unwrap_value();

    // Load the sections, their memory values must overlap with the sections
    auto loaded_sections_res = load_sections(sect_headers, target_fd, file_size);
    if ( loaded_segments_res.is_error() ) {
        return loaded_segments_res.unwrap_error();
    }
    auto loaded_sections = loaded_sections_res.unwrap_value();

    return LoadedElf { std::move(header),std::move(loaded_segments), std::move(loaded_sections) };
}
