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

#include <TC/Trait/RemoveReference.hh>

namespace std {

template<typename T>
constexpr TC::Trait::RemoveReference<T>&& move(T&& arg) noexcept {
    return static_cast<TC::Trait::RemoveReference<T>&&>(arg);
}

} /* namespace std */