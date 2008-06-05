/* vim: set sw=4 sts=4 et nofoldenable : */

/*
 * Copyright (c) 2008 Dirk Ribbrock <dirk.ribbrock@uni-dortmund.de>
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

#include <honei/backends/cuda/cuda_util.hh>

namespace honei
{
    namespace cuda
    {
        __global__ void scaled_sum_gpu(float * x, float * y, float b, unsigned long size)
        {
            int idx = blockDim.x *blockIdx.x + threadIdx.x;
            x[idx] = x[idx] + b * y[idx];
        }

        __global__ void scaled_sum_gpu(float * x, float * y, float * z, unsigned long size)
        {
            int idx = blockDim.x *blockIdx.x + threadIdx.x;
            x[idx] = x[idx] + y[idx] * z[idx];
        }
    }
}

extern "C" void cuda_scaled_sum_two_float(float * x, float * y, float b, unsigned long size, unsigned long blocksize)
{
    dim3 grid;
    dim3 block;
    block.x = blocksize;
    grid.x = ceil(size/(float)block.x);
    float * x_gpu(0);
    float * y_gpu(0);

    cudaMalloc((void**)&x_gpu, size * sizeof(float));
    cudaMalloc((void**)&y_gpu, size * sizeof(float));

    cudaMemcpy(x_gpu, x, size * sizeof(float), cudaMemcpyHostToDevice);
    cudaMemcpy(y_gpu, y, size * sizeof(float), cudaMemcpyHostToDevice);

    honei::cuda::scaled_sum_gpu<<<grid, block>>>(x_gpu, y_gpu, b, size);

    cudaMemcpy(x, x_gpu, size * sizeof(float), cudaMemcpyDeviceToHost);
    cudaFree(x_gpu);
    cudaFree(y_gpu);

    CUDA_ERROR();
}

extern "C" void cuda_scaled_sum_three_float(float * x, float * y, float * z, unsigned long size, unsigned long blocksize)
{
    dim3 grid;
    dim3 block;
    block.x = blocksize;
    grid.x = ceil(size/(float)block.x);
    float * x_gpu(0);
    float * y_gpu(0);
    float * z_gpu(0);

    cudaMalloc((void**)&x_gpu, size * sizeof(float));
    cudaMalloc((void**)&y_gpu, size * sizeof(float));
    cudaMalloc((void**)&z_gpu, size * sizeof(float));

    cudaMemcpy(x_gpu, x, size * sizeof(float), cudaMemcpyHostToDevice);
    cudaMemcpy(y_gpu, y, size * sizeof(float), cudaMemcpyHostToDevice);
    cudaMemcpy(z_gpu, z, size * sizeof(float), cudaMemcpyHostToDevice);

    honei::cuda::scaled_sum_gpu<<<grid, block>>>(x_gpu, y_gpu, z_gpu, size);

    cudaMemcpy(x, x_gpu, size * sizeof(float), cudaMemcpyDeviceToHost);
    cudaFree(x_gpu);
    cudaFree(y_gpu);
    cudaFree(z_gpu);

    CUDA_ERROR();
}
