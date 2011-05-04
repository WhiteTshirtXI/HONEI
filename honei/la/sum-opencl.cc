/* vim: set sw=4 sts=4 et nofoldenable : */

/*
 * Copyright (c) 2011 Dirk Ribbrock <dirk.ribbrock@math.uni-dortmund.de>
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

#include <honei/la/sum.hh>
#include <honei/backends/opencl/operations.hh>


using namespace honei;

DenseVectorContinuousBase<float> & Sum<tags::OpenCL::CPU>::value(DenseVectorContinuousBase<float> & x, const DenseVectorContinuousBase<float> & y)
{
    CONTEXT("When calculating the sum of two DenseVectorContinuousBase<float> (OpenCL CPU):");
    if (x.size() != y.size())
        throw VectorSizeDoesNotMatch(y.size(), x.size());

    void * x_cl(x.lock(lm_read_and_write, tags::OpenCL::CPU::memory_value));
    void * y_cl(y.lock(lm_read_only, tags::OpenCL::CPU::memory_value));
    opencl::sum_float(x_cl, x_cl, y_cl, x.size(), CL_DEVICE_TYPE_CPU);
    x.unlock(lm_read_and_write);
    y.unlock(lm_read_only);

    return x;
}

DenseVectorContinuousBase<double> & Sum<tags::OpenCL::CPU>::value(DenseVectorContinuousBase<double> & x, const DenseVectorContinuousBase<double> & y)
{
    CONTEXT("When calculating the sum of two DenseVectorContinuousBase<double> (OpenCL CPU):");
    if (x.size() != y.size())
        throw VectorSizeDoesNotMatch(y.size(), x.size());

    void * x_cl(x.lock(lm_read_and_write, tags::OpenCL::CPU::memory_value));
    void * y_cl(y.lock(lm_read_only, tags::OpenCL::CPU::memory_value));
    opencl::sum_double(x_cl, x_cl, y_cl, x.size(), CL_DEVICE_TYPE_CPU);
    x.unlock(lm_read_and_write);
    y.unlock(lm_read_only);

    return x;
}

DenseVectorContinuousBase<float> & Sum<tags::OpenCL::GPU>::value(DenseVectorContinuousBase<float> & x, const DenseVectorContinuousBase<float> & y)
{
    CONTEXT("When calculating the sum of two DenseVectorContinuousBase<float> (OpenCL GPU):");
    if (x.size() != y.size())
        throw VectorSizeDoesNotMatch(y.size(), x.size());

    void * x_cl(x.lock(lm_read_and_write, tags::OpenCL::GPU::memory_value));
    void * y_cl(y.lock(lm_read_only, tags::OpenCL::GPU::memory_value));
    opencl::sum_float(x_cl, x_cl, y_cl, x.size(), CL_DEVICE_TYPE_GPU);
    x.unlock(lm_read_and_write);
    y.unlock(lm_read_only);

    return x;
}

DenseVectorContinuousBase<double> & Sum<tags::OpenCL::GPU>::value(DenseVectorContinuousBase<double> & x, const DenseVectorContinuousBase<double> & y)
{
    CONTEXT("When calculating the sum of two DenseVectorContinuousBase<double> (OpenCL GPU):");
    if (x.size() != y.size())
        throw VectorSizeDoesNotMatch(y.size(), x.size());

    void * x_cl(x.lock(lm_read_and_write, tags::OpenCL::GPU::memory_value));
    void * y_cl(y.lock(lm_read_only, tags::OpenCL::GPU::memory_value));
    opencl::sum_double(x_cl, x_cl, y_cl, x.size(), CL_DEVICE_TYPE_GPU);
    x.unlock(lm_read_and_write);
    y.unlock(lm_read_only);

    return x;
}