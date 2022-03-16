//
// Created by mek101 on 14/03/22.
//

#pragma once

#include <string>
#include <memory>

#include <LibTC/Functional/Result.hh>

#include "../ElfDynamicObject.hh"
#include "../Definitions.hh"


enum class ElfLoadError {
    IOError,
    FormatError,
    UnsupportedElfError,
    SizeError,
    UnknownLoadError
};

template <typename T>
using ElfLoadResult = TC::Functional::Result<T, ElfLoadError>;

struct LoadedElf {
    std::unique_ptr<Elf32Ehdr> header;
    LoadedSegments segments;
    LoadedSections sections;
};

/**
 * Try to load an ELF file from the specified path.
 * This function is thread-safe.
 *
 * @param target_path:  the path of the ELF file.
 * @return A result with the loaded ElfDynamicObject if successful, or an error otherwise.
 */

ElfLoadResult<LoadedElf> try_load(std::string const& target_path);
