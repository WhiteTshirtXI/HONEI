/* vim: set sw=4 sts=4 et nofoldenable filetype=cpp : */

/*
* Copyright (c) 2011 Dirk Ribbrock <dirk.ribbrock@math.uni-dortmund.de>
*
* This file is part of the HONEI C++ library. HONEI is free software;
* you can redistribute it and/or modify it under the terms of the GNU General
* Public License version 2, as published by the Free Software Foundation.
*
* HONEI is distributed in the hope that it will be useful, but WITHOUT ANY
* WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
* FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
* details.
*
* You should have received a copy of the GNU General Public License along with
* this program; if not, write to the Free Software Foundation, Inc., 59 Temple
* Place, Suite 330, Boston, MA  02111-1307  USA
*/

#ifdef HONEI_OPENCL_GPU
#pragma OPENCL EXTENSION cl_khr_fp64 : enable
#endif

__kernel void scale_f(__global  float * output,
                                   const     float multiplier,
                                   const unsigned long size)
{
    uint tid = get_global_id(0);

    if (tid < size) output[tid] = output[tid] * multiplier;
}

__kernel void scale_d(__global  double * output,
                                   const     double multiplier,
                                   const unsigned long size)
{
    uint tid = get_global_id(0);

    if (tid < size) output[tid] = output[tid] * multiplier;
}
