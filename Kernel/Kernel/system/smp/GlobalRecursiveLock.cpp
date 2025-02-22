/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *  * * * * * * * *
 * MeetiX OS By MeetiX OS Project [Marco Cicognani]                                    *
 *                                                                                     *
 *         DERIVED FROM THE GHOST OPERATING SYSTEM                                     *
 *         This software is derived from the Ghost operating system project,           *
 *         written by Max Schlüssel <lokoxe@gmail.com>. Copyright 2012-2017            *
 *         https://ghostkernel.org/                                                    *
 *         https://github.com/maxdev1/ghost                                            *
 *                                                                                     *
 * This program is free software; you can redistribute it and/or                       *
 * modify it under the terms of the GNU General Public License                         *
 * as published by the Free Software Foundation; either version 2                      *
 * of the License, or (char *argumentat your option) any later version.                *
 *                                                                                     *
 * This program is distributed in the hope that it will be useful,                     *
 * but WITHout ANY WARRANTY; without even the implied warranty of                      *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the                       *
 * GNU General Public License for more details.                                        *
 *                                                                                     *
 * You should have received a copy of the GNU General Public License                   *
 * along with this program; if not, write to the Free Software                         *
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA      *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *  * * * * * * */

#include <logger/logger.hpp>
#include <system/smp/GlobalRecursiveLock.hpp>
#include <system/system.hpp>

/**
 * lock the cpu
 */
void GlobalRecursiveLock::lock() {
    while ( !__sync_bool_compare_and_swap(&atom, 0, 1) ) {
        if ( owner == System::currentProcessorId() ) {
            ++depth;
            return;
        }
        asm("pause");
    }
    owner = System::currentProcessorId();
    depth = 0;
}

/**
 * unlock the cpu
 */
void GlobalRecursiveLock::unlock() {
    if ( depth > 0 ) {
        --depth;
        return;
    }
    owner = -1;
    atom  = 0;
}

/**
 * check locking
 *
 * @return wether is locking
 */
bool GlobalRecursiveLock::isLocked() {
    return atom;
}
