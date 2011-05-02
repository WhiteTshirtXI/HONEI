/* vim: set sw=4 sts=4 et nofoldenable : */

/*
 * Copyright (c) 2010 Dirk Ribbrock <dirk.ribbrock@math.uni-dortmund.de>
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

#include <honei/backends/opencl/opencl_backend.hh>

namespace honei
{
    namespace opencl
    {
        void scaled_sum_float(void * x, void * y, float b, unsigned long size, cl_device_type type)
        {
            cl_command_queue command_queue;
            cl_kernel kernel;
            cl_context context;
            cl_device_id device;
            size_t threads = size;

            DCQ dcq = OpenCLBackend::instance()->prepare_device(type);
            device = dcq.device;
            context = dcq.context;
            command_queue = dcq.command_queue;

            //print_device_info(device);
            std::string filename(HONEI_SOURCEDIR);
            filename += "/honei/backends/opencl/";
            filename += "scaled_sum.cl";
            kernel = OpenCLBackend::instance()->create_kernel(filename, "scaled_sum_float", context, device);
            clSetKernelArg(kernel, 0, sizeof(cl_mem), &x);
            clSetKernelArg(kernel, 1, sizeof(cl_mem), &y);
            clSetKernelArg(kernel, 2, sizeof(cl_float), (void *)&b);
            clSetKernelArg(kernel, 3, sizeof(cl_uint), (void *)&size);

            clEnqueueNDRangeKernel(command_queue, kernel, 1, NULL, &threads, NULL, 0, NULL, NULL);

        }

        void scaled_sum_double(void * x, void * y, double b, unsigned long size, cl_device_type type)
        {
            cl_command_queue command_queue;
            cl_kernel kernel;
            cl_context context;
            cl_device_id device;
            size_t threads =size;

            DCQ dcq = OpenCLBackend::instance()->prepare_device(type);
            device = dcq.device;
            context = dcq.context;
            command_queue = dcq.command_queue;

            //print_device_info(device);
            std::string filename(HONEI_SOURCEDIR);
            filename += "/honei/backends/opencl/";
            filename += "scaled_sum.cl";
            kernel = OpenCLBackend::instance()->create_kernel(filename, "scaled_sum_double", context, device);
            clSetKernelArg(kernel, 0, sizeof(cl_mem), &x);
            clSetKernelArg(kernel, 1, sizeof(cl_mem), &y);
            clSetKernelArg(kernel, 2, sizeof(cl_double), (void *)&b);
            clSetKernelArg(kernel, 3, sizeof(cl_uint), (void *)&size);

            clEnqueueNDRangeKernel(command_queue, kernel, 1, NULL, &threads, NULL, 0, NULL, NULL);
        }

        void scaled_sum_float(void * r, void * x, void * y, float b, unsigned long size, cl_device_type type)
        {
            cl_command_queue command_queue;
            cl_kernel kernel;
            cl_context context;
            cl_device_id device;
            size_t threads = size;

            DCQ dcq = OpenCLBackend::instance()->prepare_device(type);
            device = dcq.device;
            context = dcq.context;
            command_queue = dcq.command_queue;

            //print_device_info(device);
            std::string filename(HONEI_SOURCEDIR);
            filename += "/honei/backends/opencl/";
            filename += "scaled_sum.cl";
            kernel = OpenCLBackend::instance()->create_kernel(filename, "scaled_sum_three_float_s", context, device);
            clSetKernelArg(kernel, 0, sizeof(cl_mem), &r);
            clSetKernelArg(kernel, 1, sizeof(cl_mem), &x);
            clSetKernelArg(kernel, 2, sizeof(cl_mem), &y);
            clSetKernelArg(kernel, 3, sizeof(cl_float), (void *)&b);
            clSetKernelArg(kernel, 4, sizeof(cl_uint), (void *)&size);

            clEnqueueNDRangeKernel(command_queue, kernel, 1, NULL, &threads, NULL, 0, NULL, NULL);
        }

        void scaled_sum_double(void * r, void * x, void * y, double b, unsigned long size, cl_device_type type)
        {
            cl_command_queue command_queue;
            cl_kernel kernel;
            cl_context context;
            cl_device_id device;
            size_t threads = size;

            DCQ dcq = OpenCLBackend::instance()->prepare_device(type);
            device = dcq.device;
            context = dcq.context;
            command_queue = dcq.command_queue;

            //print_device_info(device);
            std::string filename(HONEI_SOURCEDIR);
            filename += "/honei/backends/opencl/";
            filename += "scaled_sum.cl";
            kernel = OpenCLBackend::instance()->create_kernel(filename, "scaled_sum_three_double_s", context, device);
            clSetKernelArg(kernel, 0, sizeof(cl_mem), &r);
            clSetKernelArg(kernel, 1, sizeof(cl_mem), &x);
            clSetKernelArg(kernel, 2, sizeof(cl_mem), &y);
            clSetKernelArg(kernel, 3, sizeof(cl_double), (void *)&b);
            clSetKernelArg(kernel, 4, sizeof(cl_uint), (void *)&size);

            clEnqueueNDRangeKernel(command_queue, kernel, 1, NULL, &threads, NULL, 0, NULL, NULL);
        }

        void scaled_sum_float(void * r, void * x, void * y, unsigned long size, cl_device_type type)
        {
            cl_command_queue command_queue;
            cl_kernel kernel;
            cl_context context;
            cl_device_id device;
            size_t threads = size;

            DCQ dcq = OpenCLBackend::instance()->prepare_device(type);
            device = dcq.device;
            context = dcq.context;
            command_queue = dcq.command_queue;

            //print_device_info(device);
            std::string filename(HONEI_SOURCEDIR);
            filename += "/honei/backends/opencl/";
            filename += "scaled_sum.cl";
            kernel = OpenCLBackend::instance()->create_kernel(filename, "scaled_sum_three_float", context, device);
            clSetKernelArg(kernel, 0, sizeof(cl_mem), &r);
            clSetKernelArg(kernel, 1, sizeof(cl_mem), &x);
            clSetKernelArg(kernel, 2, sizeof(cl_mem), &y);
            clSetKernelArg(kernel, 3, sizeof(cl_uint), (void *)&size);

            clEnqueueNDRangeKernel(command_queue, kernel, 1, NULL, &threads, NULL, 0, NULL, NULL);
        }

        void scaled_sum_double(void * r, void * x, void * y, unsigned long size, cl_device_type type)
        {
            cl_command_queue command_queue;
            cl_kernel kernel;
            cl_context context;
            cl_device_id device;
            size_t threads = size;

            DCQ dcq = OpenCLBackend::instance()->prepare_device(type);
            device = dcq.device;
            context = dcq.context;
            command_queue = dcq.command_queue;

            //print_device_info(device);
            std::string filename(HONEI_SOURCEDIR);
            filename += "/honei/backends/opencl/";
            filename += "scaled_sum.cl";
            kernel = OpenCLBackend::instance()->create_kernel(filename, "scaled_sum_three_double", context, device);
            clSetKernelArg(kernel, 0, sizeof(cl_mem), &r);
            clSetKernelArg(kernel, 1, sizeof(cl_mem), &x);
            clSetKernelArg(kernel, 2, sizeof(cl_mem), &y);
            clSetKernelArg(kernel, 3, sizeof(cl_uint), (void *)&size);

            clEnqueueNDRangeKernel(command_queue, kernel, 1, NULL, &threads, NULL, 0, NULL, NULL);
        }
    }
}
