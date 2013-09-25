/* vim: set number sw=4 sts=4 et nofoldenable : */

/*
 * Copyright (c) 2009 Dirk Ribbrock <dirk.ribbrock@uni-dortmund.de>
 *
 * This file is part of the SWE C++ library. LiSWE is free software;
 * you can redistribute it and/or modify it under the terms of the GNU General
 * Public License version 2, as published by the Free Software Foundation.
 *
 * LibSWE is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc., 59 Temple
 * Place, Suite 330, Boston, MA  02111-1307  USA
 */
#include <honei/lbm/solver_lbm_grid.hh>
#include <honei/swe/volume.hh>
#include <honei/util/unittest.hh>
#include <iostream>
#include <honei/swe/volume.hh>
#include <honei/math/quadrature.hh>
#include <honei/lbm/grid.hh>
#include <honei/lbm/grid_packer.hh>

using namespace honei;
using namespace tests;
using namespace std;
using namespace lbm::lbm_lattice_types;

//#define SOLVER_VERBOSE 1

template <typename Tag_, typename DataType_, typename ExtType_>
class ExtractionGridRegressionTest :
    public TaggedTest<Tag_>
{
    public:
        ExtractionGridRegressionTest(const std::string & type) :
            TaggedTest<Tag_>("extraction_grid_regression_test<" + type + ">")
        {
        }

        virtual void run() const
        {
            unsigned long g_h(50);
            unsigned long g_w(50);

            DenseMatrix<DataType_> h(g_h, g_w, DataType_(0.05));
            Cylinder<DataType_> c1(h, DataType_(0.02), 25, 25);
            c1.value();

            DenseMatrix<DataType_> u(g_h, g_w, DataType_(0.));
            DenseMatrix<DataType_> v(g_h, g_w, DataType_(0.));
            DenseMatrix<DataType_> b(g_h, g_w, DataType_(0.));

            Cylinder<DataType_> b1(b, DataType_(0.0001), 15, 15);
            b1.value();

            Grid<D2Q9, DataType_> grid;
            DenseMatrix<bool> obstacles(g_h, g_w, false);
            Cuboid<bool> q2(obstacles, 15, 5, 1, 10, 0);
            q2.value();
            Cuboid<bool> q3(obstacles, 40, 5, 1, 10, 30);
            q3.value();
            grid.obstacles = new DenseMatrix<bool>(obstacles);
            grid.h = new DenseMatrix<DataType_>(h);
            grid.u = new DenseMatrix<DataType_>(u);
            grid.v = new DenseMatrix<DataType_>(v);
            grid.b = new DenseMatrix<DataType_>(b);
            PackedGridData<D2Q9, DataType_>  data;
            PackedGridInfo<D2Q9> info;

            GridPacker<D2Q9, NOSLIP, DataType_>::pack(grid, info, data);

            SolverLBMGrid<Tag_, lbm_applications::LABSWE, DataType_,lbm_force::NONE, lbm_source_schemes::NONE, lbm_grid_types::RECTANGULAR, lbm_lattice_types::D2Q9, lbm_boundary_types::NOSLIP, ExtType_> solver(&info, &data, 0.01, 0.01, 0.005, 1.1);

            solver.do_preprocessing();
            solver.solve();

            ExtractionGrid<Tag_, ExtType_>::value(info, data, DataType_(10e-5));

            //Standard solver using tags::CPU:
            unsigned long g_h_standard(50);
            unsigned long g_w_standard(50);

            DenseMatrix<DataType_> h_standard(g_h_standard, g_w_standard, DataType_(0.05));
            Cylinder<DataType_> c1_standard(h_standard, DataType_(0.02), 25, 25);
            c1_standard.value();

            DenseMatrix<DataType_> u_standard(g_h_standard, g_w_standard, DataType_(0.));
            DenseMatrix<DataType_> v_standard(g_h_standard, g_w_standard, DataType_(0.));
            DenseMatrix<DataType_> b_standard(g_h_standard, g_w_standard, DataType_(0.));

            Cylinder<DataType_> b1_standard(b_standard, DataType_(0.0001), 15, 15);
            b1_standard.value();

            Grid<D2Q9, DataType_> grid_standard;
            DenseMatrix<bool> obstacles_standard(g_h_standard, g_w_standard, false);
            Cuboid<bool> q2_standard(obstacles_standard, 15, 5, 1, 10, 0);
            q2_standard.value();
            Cuboid<bool> q3_standard(obstacles_standard, 40, 5, 1, 10, 30);
            q3_standard.value();
            grid_standard.obstacles = new DenseMatrix<bool>(obstacles_standard);
            grid_standard.h = new DenseMatrix<DataType_>(h_standard);
            grid_standard.u = new DenseMatrix<DataType_>(u_standard);
            grid_standard.v = new DenseMatrix<DataType_>(v_standard);
            grid_standard.b = new DenseMatrix<DataType_>(b_standard);
            PackedGridData<D2Q9, DataType_>  data_standard;
            PackedGridInfo<D2Q9> info_standard;

            GridPacker<D2Q9, NOSLIP, DataType_>::pack(grid_standard, info_standard, data_standard);

            SolverLBMGrid<tags::CPU, lbm_applications::LABSWE, DataType_,lbm_force::NONE, lbm_source_schemes::NONE, lbm_grid_types::RECTANGULAR, lbm_lattice_types::D2Q9, lbm_boundary_types::NOSLIP, ExtType_> solver_standard(&info_standard, &data_standard, 0.01, 0.01, 0.005, 1.1);

            solver_standard.do_preprocessing();
            solver_standard.solve();

            ExtractionGrid<tags::CPU, ExtType_>::value(info_standard, data_standard, DataType_(10e-5));


            //Compare results of both solvers:
            data.h->lock(lm_read_only);
            data.u->lock(lm_read_only);
            data.v->lock(lm_read_only);
            for(unsigned long i(0) ; i < data.h->size() ; ++i)
            {
                TEST_CHECK_EQUAL_WITHIN_EPS((*data.h)[i], (*data_standard.h)[i], std::numeric_limits<DataType_>::epsilon());
                TEST_CHECK_EQUAL_WITHIN_EPS((*data.u)[i], (*data_standard.u)[i], std::numeric_limits<DataType_>::epsilon());
                TEST_CHECK_EQUAL_WITHIN_EPS((*data.v)[i], (*data_standard.v)[i], std::numeric_limits<DataType_>::epsilon());
            }
            data.h->unlock(lm_read_only);
            data.u->unlock(lm_read_only);
            data.v->unlock(lm_read_only);

            data.destroy();
            info.destroy();
            grid.destroy();
            data_standard.destroy();
            info_standard.destroy();
            grid_standard.destroy();
        }
};
#ifdef HONEI_SSE
ExtractionGridRegressionTest<tags::CPU::Generic, float, lbm_modes::DRY> generic_solver_multi_test_float_dry("float dry");
ExtractionGridRegressionTest<tags::CPU::Generic, double, lbm_modes::DRY> generic_solver_multi_test_double_dry("double dry");
#endif
#ifdef HONEI_CUDA
ExtractionGridRegressionTest<tags::GPU::CUDA, float, lbm_modes::DRY> cuda_solver_multi_test_float_dry("float dry");
#ifdef HONEI_CUDA_DOUBLE
ExtractionGridRegressionTest<tags::GPU::CUDA, double, lbm_modes::DRY> cuda_solver_multi_test_double_dry("double dry");
#endif
#endif
#ifdef HONEI_CELL
ExtractionGridRegressionTest<tags::Cell, float, lbm_modes::DRY> cell_solver_multi_test_float_dry("float dry");
#endif
#ifdef HONEI_OPENCL
ExtractionGridRegressionTest<tags::OpenCL::CPU, float, lbm_modes::DRY> ocl_cpu_solver_multi_test_float_dry("float dry");
ExtractionGridRegressionTest<tags::OpenCL::CPU, double, lbm_modes::DRY> ocl_cpu_solver_multi_test_double_dry("double dry");
#endif
#ifdef HONEI_OPENCL_GPU
ExtractionGridRegressionTest<tags::OpenCL::GPU, float, lbm_modes::DRY> ocl_gpu_solver_multi_test_float_dry("float dry");
ExtractionGridRegressionTest<tags::OpenCL::GPU, double, lbm_modes::DRY> ocl_gpu_solver_multi_test_double_dry("double dry");
#endif
#ifdef HONEI_OPENCL_ACC
ExtractionGridRegressionTest<tags::OpenCL::Accelerator, float, lbm_modes::DRY> ocl_acc_solver_multi_test_float_dry("float dry");
ExtractionGridRegressionTest<tags::OpenCL::Accelerator, double, lbm_modes::DRY> ocl_acc_solver_multi_test_double_dry("double dry");
#endif

#ifdef HONEI_SSE
ExtractionGridRegressionTest<tags::CPU::Generic, float, lbm_modes::WET> generic_solver_multi_test_float_wet("float wet");
ExtractionGridRegressionTest<tags::CPU::Generic, double, lbm_modes::WET> generic_solver_multi_test_double_wet("double wet");
#endif
#ifdef HONEI_CUDA
ExtractionGridRegressionTest<tags::GPU::CUDA, float, lbm_modes::WET> cuda_solver_multi_test_float_wet("float wet");
#ifdef HONEI_CUDA_DOUBLE
ExtractionGridRegressionTest<tags::GPU::CUDA, double, lbm_modes::WET> cuda_solver_multi_test_double_wet("double wet");
#endif
#endif
#ifdef HONEI_CELL
ExtractionGridRegressionTest<tags::Cell, float, lbm_modes::WET> cell_solver_multi_test_float_wet("float wet");
#endif
#ifdef HONEI_OPENCL
ExtractionGridRegressionTest<tags::OpenCL::CPU, float, lbm_modes::WET> ocl_cpu_solver_multi_test_float_wet("float wet");
ExtractionGridRegressionTest<tags::OpenCL::CPU, double, lbm_modes::WET> ocl_cpu_solver_multi_test_double_wet("double wet");
#endif
#ifdef HONEI_OPENCL_GPU
ExtractionGridRegressionTest<tags::OpenCL::GPU, float, lbm_modes::WET> ocl_gpu_solver_multi_test_float_wet("float wet");
ExtractionGridRegressionTest<tags::OpenCL::GPU, double, lbm_modes::WET> ocl_gpu_solver_multi_test_double_wet("double wet");
#endif
#ifdef HONEI_OPENCL_ACC
ExtractionGridRegressionTest<tags::OpenCL::Accelerator, float, lbm_modes::WET> ocl_acc_solver_multi_test_float_wet("float wet");
ExtractionGridRegressionTest<tags::OpenCL::Accelerator, double, lbm_modes::WET> ocl_acc_solver_multi_test_double_wet("double wet");
#endif
