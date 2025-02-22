/**
 * @brief
 * This file is part of the MeetiX Operating System.
 * Copyright (c) 2017-2022, Marco Cicognani (marco.cicognani@meetixos.org)
 *
 * @developers
 * Marco Cicognani (marco.cicognani@meetixos.org)
 *
 * @license
 * GNU General Public License version 3
 */

#include "Inspector.hh"

#include "ELFInspector.hh"
#include "ShellInspector.hh"
#include "TextInspector.hh"

#include <cstring>

Inspector* Inspector::inspector_for_file(std::ifstream& stream) {
    u8 header[10]{ '\0' };
    stream.read(reinterpret_cast<char*>(header), 10);

    auto text_checker = [](auto value) {
        return value == 0x09 || value == 0x0A || value == 0x0D || (0x20 <= value && value <= 0x7E) || isascii(value);
    };

    if ( !std::memcmp(header + 1, "ELF", 3) )
        return new ELFInspector{ stream };
    else if ( !std::memcmp(header, "//>", 3) )
        return new ShellInspector{ stream };
    else if ( std::all_of(header, header + 9, text_checker) )
        return new TextInspector{ stream };
    else
        return nullptr;
}
