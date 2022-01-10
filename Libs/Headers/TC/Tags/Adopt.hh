/**
 * @brief
 * This file is part of the MeetiX Operating System.
 * Copyright (c) 2017-2021, Marco Cicognani (marco.cicognani@meetixos.org)
 *
 * @developers
 * Marco Cicognani (marco.cicognani@meetixos.org)
 *
 * @license
 * GNU General Public License version 3
 */

#pragma once

namespace TC::Tags {

enum class Adopt : bool { __Value };

/**
 * @brief The object must take the ownership of the memory
 */
constexpr auto C_ADOPT = Adopt::__Value;

} /* namespace TC::Tags */