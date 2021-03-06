/*
 * linear least squares model
 *
 * Copyright (c) 2006 Michael Niedermayer <michaelni@gmx.at>
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

#ifndef BVUTIL_LLS_H
#define BVUTIL_LLS_H

#include "common.h"
#include "mem.h"
#include "version.h"

#define MAX_VARS 32
#define MAX_VARS_ALIGN BBALIGN(MAX_VARS+1,4)

//FIXME avoid direct access to LLSModel from outside

/**
 * Linear least squares model.
 */
typedef struct LLSModel {
    BV_DECLARE_ALIGNED(32, double, covariance[MAX_VARS_ALIGN][MAX_VARS_ALIGN]);
    BV_DECLARE_ALIGNED(32, double, coeff[MAX_VARS][MAX_VARS]);
    double variance[MAX_VARS];
    int indep_count;
    /**
     * Take the outer-product of var[] with itself, and add to the covariance matrix.
     * @param m this context
     * @param var training samples, starting with the value to be predicted
     *            32-byte aligned, and any padding elements must be initialized
     *            (i.e not denormal/nan).
     */
    void (*update_lls)(struct LLSModel *m, const double *var);
    /**
     * Inner product of var[] and the LPC coefs.
     * @param m this context
     * @param var training samples, excluding the value to be predicted. unaligned.
     * @param order lpc order
     */
    double (*evaluate_lls)(struct LLSModel *m, const double *var, int order);
} LLSModel;

void bvpriv_init_lls(LLSModel *m, int indep_count);
void bb_init_lls_x86(LLSModel *m);
void bvpriv_solve_lls(LLSModel *m, double threshold, unsigned short min_order);

#endif /* BVUTIL_LLS_H */
