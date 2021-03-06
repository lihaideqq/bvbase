/*
 * copyright (c) 2006 Mans Rullgard
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

#ifndef BVUTIL_ADLER32_H
#define BVUTIL_ADLER32_H

#include <stdint.h>
#include "attributes.h"

/**
 * @file
 * Public header for libbvutil Adler32 hasher
 *
 * @defgroup lavu_adler32 Adler32
 * @ingroup lavu_crypto
 * @{
 */

/**
 * Calculate the Adler32 checksum of a buffer.
 *
 * Passing the return value to a subsequent bv_adler32_update() call
 * allows the checksum of multiple buffers to be calculated as though
 * they were concatenated.
 *
 * @param adler initial checksum value
 * @param buf   pointer to input buffer
 * @param len   size of input buffer
 * @return      updated checksum
 */
unsigned long bv_adler32_update(unsigned long adler, const uint8_t *buf,
                                unsigned int len) bv_pure;

/**
 * @}
 */

#endif /* BVUTIL_ADLER32_H */
