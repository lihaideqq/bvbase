/*
 * Copyright (c) 2012 Ronald S. Bultje <rsbultje@gmail.com>
 *
 * This file is part of BVBase.
 *
 * BVBase is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * BVBase is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with BVBase; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

#ifndef BVUTIL_ATOMIC_WIN32_H
#define BVUTIL_ATOMIC_WIN32_H

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#define bvpriv_atomic_int_get atomic_int_get_win32
static inline int atomic_int_get_win32(volatile int *ptr)
{
    MemoryBarrier();
    return *ptr;
}

#define bvpriv_atomic_int_set atomic_int_set_win32
static inline void atomic_int_set_win32(volatile int *ptr, int val)
{
    *ptr = val;
    MemoryBarrier();
}

#define bvpriv_atomic_int_add_and_fetch atomic_int_add_and_fetch_win32
static inline int atomic_int_add_and_fetch_win32(volatile int *ptr, int inc)
{
    return inc + InterlockedExchangeAdd(ptr, inc);
}

#define bvpriv_atomic_ptr_cas atomic_ptr_cas_win32
static inline void *atomic_ptr_cas_win32(void * volatile *ptr,
                                         void *oldval, void *newval)
{
    return InterlockedCompareExchangePointer(ptr, newval, oldval);
}

#endif /* BVUTIL_ATOMIC_WIN32_H */
