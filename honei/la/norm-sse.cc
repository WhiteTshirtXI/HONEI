/* vim: set sw=4 sts=4 et nofoldenable : */

/*
 * Copyright (c) 2007, 2008 Dirk Ribbrock <dirk.ribbrock@uni-dortmund.de>
 *
 * This file is part of the LA C++ library. LibLa is free software;
 * you can redistribute it and/or modify it under the terms of the GNU General
 * Public License version 2, as published by the Free Software Foundation.
 *
 * LibLa is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc., 59 Temple
 * Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include <honei/la/norm.hh>
#include <honei/backends/sse/operations.hh>


using namespace honei;


float Norm<vnt_l_two, false, tags::CPU::SSE>::value(const DenseVectorContinuousBase<float> & a)
{
    CONTEXT("When calculating L2 norm (false) of DenseVectorContinuousBase<float> (SSE):");

    return sse::norm_l2(a.elements(), a.size());
}

double Norm<vnt_l_two, false, tags::CPU::SSE>::value(const DenseVectorContinuousBase<double> & a)
{
    CONTEXT("When calculating L2 norm (false) of DenseVectorContinuousBase<double> (SSE):");

    return sse::norm_l2(a.elements(), a.size());
}

float Norm<vnt_l_two, true, tags::CPU::SSE>::value(const DenseVectorContinuousBase<float> & a)
{
    CONTEXT("When calculating L2 norm (true) of DenseVectorContinuousBase<float> (SSE):");

    return sqrt(sse::norm_l2(a.elements(), a.size()));
}

double Norm<vnt_l_two, true, tags::CPU::SSE>::value(const DenseVectorContinuousBase<double> & a)
{
    CONTEXT("When calculating L2 norm (true) of DenseVectorContinuousBase<double> (SSE):");

    return sqrt(sse::norm_l2(a.elements(), a.size()));
}

float Norm<vnt_l_two, false, tags::CPU::SSE>::value(const DenseMatrix<float> & a)
{
    CONTEXT("When calculating L2 norm (false) of DenseMatrix<float> (SSE):");

    return sse::norm_l2(a.elements(), a.size());
}

double Norm<vnt_l_two, false, tags::CPU::SSE>::value(const DenseMatrix<double> & a)
{
    CONTEXT("When calculating L2 norm (false) of DenseMatrix<double> (SSE):");

    return sse::norm_l2(a.elements(), a.size());
}

float Norm<vnt_l_two, true, tags::CPU::SSE>::value(const DenseMatrix<float> & a)
{
    CONTEXT("When calculating L2 norm (true) of DenseMatrix<float> (SSE):");

    return sse::norm_l2(a.elements(), a.size());
}

double Norm<vnt_l_two, true, tags::CPU::SSE>::value(const DenseMatrix<double> & a)
{
    CONTEXT("When calculating L2 norm (true) of DenseMatrix<double> (SSE):");

    return sse::norm_l2(a.elements(), a.size());
}

float Norm<vnt_l_two, false, tags::CPU::SSE>::value(const SparseVector<float> & a)
{
    CONTEXT("When calculating L2 norm (false) of SparseVector<float> (SSE):");

    return sse::norm_l2(a.elements(), a.used_elements());
}

double Norm<vnt_l_two, false, tags::CPU::SSE>::value(const SparseVector<double> & a)
{
    CONTEXT("When calculating L2 norm (false) of SparseVector<double> (SSE):");

    return sse::norm_l2(a.elements(), a.used_elements());
}

float Norm<vnt_l_two, true, tags::CPU::SSE>::value(const SparseVector<float> & a)
{
    CONTEXT("When calculating L2 norm (true) of SparseVector<float> (SSE):");

    return sqrt(sse::norm_l2(a.elements(), a.used_elements()));
}

double Norm<vnt_l_two, true, tags::CPU::SSE>::value(const SparseVector<double> & a)
{
    CONTEXT("When calculating L2 norm (true) of SparseVector<double> (SSE):");

    return sqrt(sse::norm_l2(a.elements(), a.used_elements()));
}

