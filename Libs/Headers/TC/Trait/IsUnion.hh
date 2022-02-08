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

#include <TC/Trait/Constant.hh>

namespace TC {
namespace Trait {

template<typename T>
inline constexpr bool IsUnion = __is_union(T);

} /* namespace Trait */

using Trait::IsUnion;

} /* namespace TC */