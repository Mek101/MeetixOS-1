//
// Created by mek101 on 16/03/22.
//

#pragma once

#include <vector>
#include <optional>
#include <memory>

#include <Api/ELF32.h>

struct SegmentInfo {
    Elf32Phdr header;
    std::unique_ptr<u8[]> segment;
};

struct LoadedSegments {
    std::vector<SegmentInfo> load;
    std::vector<SegmentInfo> dynamic;
    std::optional<SegmentInfo> program_header;
    std::vector<SegmentInfo> tls;
};

struct SectionInfo {
    Elf32Shdr header;
};

struct LoadedSections {
    std::vector<SectionInfo> program_bits;
    std::vector<SectionInfo> symbol_table;
    std::optional<SectionInfo> string_table;
    std::vector<SectionInfo> hash;
    std::optional<SectionInfo> dynamic;
    std::vector<SectionInfo> no_bits;
    std::vector<SectionInfo> relocation;
    std::vector<SectionInfo> dynamic_symbols;
};
