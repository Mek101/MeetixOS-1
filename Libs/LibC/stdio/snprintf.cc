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

#include <stdio.h>

extern "C" int snprintf(char* s, usize n, const char* format, ...) {
    va_list va;
    va_start(va, format);
    auto res = vsnprintf(s, n, format, va);
    va_end(va);
    return res;
}
