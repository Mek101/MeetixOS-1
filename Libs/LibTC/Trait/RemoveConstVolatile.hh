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

#pragma once

#include <LibTC/Trait/RemoveConst.hh>
#include <LibTC/Trait/RemoveVolatile.hh>

namespace TC {
namespace Trait {

template<typename T>
using RemoveConstVolatile = RemoveVolatile<RemoveConst<T>>;

} /* namespace Trait */

using Trait::RemoveConstVolatile;

} /* namespace TC */