/*
 * Copyright (c) 2009 Mans Rullgard <mans@mansr.com>
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

#ifndef BVUTIL_ARM_FLOAT_DSP_ARM_H
#define BVUTIL_ARM_FLOAT_DSP_ARM_H

#include "libbvutil/float_dsp.h"

void bb_float_dsp_init_vfp(BVFloatDSPContext *fdsp, int cpu_flags);
void bb_float_dsp_init_neon(BVFloatDSPContext *fdsp);

#endif /* BVUTIL_ARM_FLOAT_DSP_ARM_H */
