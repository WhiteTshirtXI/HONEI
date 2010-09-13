/* vim: set sw=4 sts=4 et foldmethod=syntax : */

/*
 * Copyright (c) 2010 Markus Geveler <apryde@gmx.de>
 *
 * This file is part of the LBM C++ library. HONEI is free software;
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

#include <honei/lbm/solver_lbm_grid.hh>
#include <honei/lbm/bitmap_io.hh>
#include <honei/swe/post_processing.hh>
#include <unittest/unittest.hh>
#include <iostream>
#include <honei/lbm/grid.hh>
#include <honei/lbm/grid_packer.hh>
#include <honei/la/difference.hh>
#include <honei/la/sum.hh>

using namespace honei;
using namespace tests;
using namespace std;
using namespace output_types;
using namespace lbm::lbm_lattice_types;

//#define SOLVER_VERBOSE 1
//#define SOLVER_POSTPROCESSING 1
template <typename Tag_, typename DataType_>
class SolverLBMGridExternalComparisonTest :
    public TaggedTest<Tag_>
{
    private:
        std::string _fn_initial_h;
        std::string _fn_initial_veloc_x;
        std::string _fn_initial_veloc_y;
        std::string _fn_initial_b;
        std::string _fn_obstacles;
        unsigned long _max_timesteps;
        DataType_ _max_h;
        DataType_ _dx, _dy, _dt, _tau;
    public:
        SolverLBMGridExternalComparisonTest(const std::string & type,
                                            const std::string & fn_h,
                                            const std::string & fn_v_x,
                                            const std::string & fn_v_y,
                                            const std::string & fn_b,
                                            const std::string & fn_o,
                                            unsigned long max_ts,
                                            DataType_ max_h,
                                            DataType_ dx,
                                            DataType_ dy,
                                            DataType_ dt,
                                            DataType_ tau) :
            TaggedTest<Tag_>("solver_lbm_grid_external_comparison_test<" + type + ">")
        {
            _fn_initial_h = fn_h;
            _fn_initial_veloc_x = fn_v_x;
            _fn_initial_veloc_y = fn_v_y;
            _fn_initial_b = fn_b;
            _fn_obstacles = fn_o;
            _max_timesteps = max_ts;
            _max_h = max_h;
            _dx = dx;
            _dy = dy;
            _dt = dt;
            _tau = tau;
        }

        virtual void run() const
        {
            //Reading data from files:
            DenseMatrix<DataType_> h(BitmapIO<io_formats::PPM>::read_scalar_field(_fn_initial_h, _max_h));
            DenseMatrix<DataType_> u(BitmapIO<io_formats::PPM>::read_scalar_field(_fn_initial_veloc_x, _max_h));
            DenseMatrix<DataType_> v(BitmapIO<io_formats::PPM>::read_scalar_field(_fn_initial_veloc_y, _max_h));
            DenseMatrix<DataType_> b(BitmapIO<io_formats::PPM>::read_scalar_field(_fn_initial_b, _max_h));
            DenseMatrix<bool> o(BitmapIO<io_formats::PPM>::read_scalar_field(_fn_obstacles, bool(1)));

            Grid<D2Q9, DataType_> grid;
            grid.h = &h;
            grid.u = &u;
            grid.v = &v;
            grid.b = &b;
            grid.obstacles = &o;

            grid.d_x = _dx;
            grid.d_y = _dy;
            grid.d_t = _dt;
            grid.tau = _tau;

            PackedGridData<D2Q9, DataType_>  data;
            PackedGridInfo<D2Q9> info;

            GridPacker<D2Q9, NOSLIP, DataType_>::pack(grid, info, data);

            SolverLBMGrid<Tag_, lbm_applications::LABSWE, DataType_,lbm_force::CENTRED, lbm_source_schemes::BED_FULL, lbm_grid_types::RECTANGULAR, lbm_lattice_types::D2Q9, lbm_boundary_types::NOSLIP, lbm_modes::DRY> solver(&info, &data, grid.d_x, grid.d_y, grid.d_t, grid.tau);

            solver.do_preprocessing();
            for(unsigned long i(0); i < _max_timesteps; ++i)
            {
#ifdef SOLVER_VERBOSE
                std::cout<<"Timestep: " << i << "/" << _max_timesteps << std::endl;
#endif
                solver.solve();
#ifdef SOLVER_POSTPROCESSING
                solver.do_postprocessing();
                GridPacker<D2Q9, NOSLIP, DataType_>::unpack(grid, info, data);
                PostProcessing<GNUPLOT>::value(*grid.h, 1, h.columns(), h.rows(), i);
#endif
            }
            solver.do_postprocessing();
            GridPacker<D2Q9, NOSLIP, DataType_>::unpack(grid, info, data);
#ifdef SOLVER_VERBOSE
            std::cout << *grid.h << std::endl;
#endif
            for (unsigned long i(0) ; i < (*grid.h).rows() ; ++i)
                for(unsigned long j(0) ; j < (*grid.h).columns() ; ++j)
                    TEST_CHECK_EQUAL_WITHIN_EPS((*grid.h)( i , j), DataType_(0.04), DataType_(0.041));

            DataType_ max_v(0);
            for(unsigned long i(0) ; i < grid.h->rows() ; ++i)
                for(unsigned long j(0) ; j < grid.h->columns() ; ++j)
                    max_v = std::max(max_v, (*grid.h)[i][j]);

            //0->0, max_v->1
            for(unsigned long i(0) ; i < grid.h->rows() ; ++i)
                for(unsigned long j(0) ; j < grid.h->columns() ; ++j)
                    (*grid.h)[i][j] /= max_v;

            BitmapIO<io_formats::PPM>::write_scalar_field(*grid.h, "ext_result_h_1.ppm");
            DenseMatrix<DataType_> bluppy2(BitmapIO<io_formats::PPM>::read_scalar_field("ext_result_h_1.ppm", DataType_(1.)));
        }

};
/*SolverLBMGridExternalComparisonTest_1<tags::CPU, float> cpu_solver_test_float("float", "ext_initial_h.ppm", "ext_initial_vx.ppm", "ext_initial_vy.ppm", "ext_initial_b.ppm", "ext_obstacles.ppm", 300ul, 0.025f, 0.001f, 0.001f, 0.00051f, 1.5f);
#ifdef HONEI_SSE
SolverLBMGridExternalComparisonTest<tags::CPU::SSE, float> sse_solver_test_float("float", "ext_initial_h.ppm", "ext_initial_vx.ppm", "ext_initial_vy.ppm", "ext_initial_b.ppm", "ext_obstacles.ppm",150ul, 0.1f, 0.01f, 0.01f, 0.01f, 1.f);
SolverLBMGridExternalComparisonTest<tags::CPU::SSE, double> sse_solver_test_double("double", "ext_initial_h.ppm", "ext_initial_vx.ppm", "ext_initial_vy.ppm", "ext_initial_b.ppm", "ext_obstacles.ppm", 150ul, 0.1, 0.01, 0.01, 0.01, 1.);
SolverLBMGridExternalComparisonTest<tags::CPU::MultiCore::SSE, float> mcsse_solver_test_float("float", "ext_initial_h.ppm", "ext_initial_vx.ppm", "ext_initial_vy.ppm", "ext_initial_b.ppm", "ext_obstacles.ppm",150ul, 0.1f, 0.01f, 0.01f, 0.01f, 1.f);
SolverLBMGridExternalComparisonTest<tags::CPU::MultiCore::SSE, double> mcsse_solver_test_double("double", "ext_initial_h.ppm", "ext_initial_vx.ppm", "ext_initial_vy.ppm", "ext_initial_b.ppm", "ext_obstacles.ppm",150ul, 0.1, 0.01, 0.01, 0.01, 1.);
#endif
#ifdef HONEI_CUDA
SolverLBMGridExternalComparisonTest<tags::GPU::CUDA, float> cuda_solver_test_float("float", "ext_initial_h.ppm", "ext_initial_vx.ppm", "ext_initial_vy.ppm", "ext_initial_b.ppm", "ext_obstacles.ppm", 150ul, 0.1f, 0.01f, 0.01f, 0.01f, 1.f);
#endif*/


template <typename Tag_, typename DataType_>
class SolverLBMGridExternalComparisonTest_CDUB :
    public TaggedTest<Tag_>
{
    private:
        std::string _fn_initial_h;
        std::string _fn_initial_veloc_x;
        std::string _fn_initial_veloc_y;
        std::string _fn_initial_b;
        std::string _fn_obstacles;
        unsigned long _max_timesteps;
        DataType_ _scale_h, _scale_vx, _scale_vy, _scale_b;
        DataType_ _dx, _dy, _dt, _tau;
    public:
        SolverLBMGridExternalComparisonTest_CDUB(const std::string & type,
                                            const std::string & fn_h,
                                            const std::string & fn_v_x,
                                            const std::string & fn_v_y,
                                            const std::string & fn_b,
                                            const std::string & fn_o,
                                            unsigned long max_ts,
                                            DataType_ scale_h,
                                            DataType_ scale_vx,
                                            DataType_ scale_vy,
                                            DataType_ scale_b,
                                            DataType_ dx,
                                            DataType_ dy,
                                            DataType_ dt,
                                            DataType_ tau) :
            TaggedTest<Tag_>("solver_lbm_grid_external_comparison_test<" + type + ">")
        {
            _fn_initial_h = fn_h;
            _fn_initial_veloc_x = fn_v_x;
            _fn_initial_veloc_y = fn_v_y;
            _fn_initial_b = fn_b;
            _fn_obstacles = fn_o;
            _max_timesteps = max_ts;
            _scale_h = scale_h;
            _scale_vx = scale_vx;
            _scale_vy = scale_vy;
            _scale_b = scale_b;
            _dx = dx;
            _dy = dy;
            _dt = dt;
            _tau = tau;
        }

        virtual void run() const
        {
            //Code used for bed topography generation
            /*Grid<D2Q9, DataType_> grid;

            ScenarioCollection::get_scenario(5, 100, 200, grid);

            DataType_ max_v(0);
            for(unsigned long i(0) ; i < grid.b->rows() ; ++i)
                for(unsigned long j(0) ; j < grid.b->columns() ; ++j)
                    max_v = std::max(max_v, (*grid.b)[i][j]);

            //0->0, max_v->1
            for(unsigned long i(0) ; i < grid.b->rows() ; ++i)
                for(unsigned long j(0) ; j < grid.b->columns() ; ++j)
                    (*grid.b)[i][j] /= max_v;

            BitmapIO<io_formats::PPM>::write_scalar_field(*grid.b, "ext_initial_b_cdub.ppm");*/

            //Reading data from files:
            DenseMatrix<DataType_> h(BitmapIO<io_formats::PPM>::read_scalar_field(_fn_initial_h, _scale_h));
            DenseMatrix<DataType_> u(BitmapIO<io_formats::PPM>::read_scalar_field(_fn_initial_veloc_x, _scale_vx));
            DenseMatrix<DataType_> v(BitmapIO<io_formats::PPM>::read_scalar_field(_fn_initial_veloc_y, _scale_vy));
            DenseMatrix<DataType_> b(BitmapIO<io_formats::PPM>::read_scalar_field(_fn_initial_b, _scale_b));
            DenseMatrix<bool> o(BitmapIO<io_formats::PPM>::read_scalar_field(_fn_obstacles, bool(1)));

            //Renormalize initial h:
            Difference<Tag_>::value(h, b);

            Grid<D2Q9, DataType_> grid;
            grid.h = &h;
            grid.u = &u;
            grid.v = &v;
            grid.b = &b;
            grid.obstacles = &o;

            grid.d_x = _dx;
            grid.d_y = _dy;
            grid.d_t = _dt;
            grid.tau = _tau;

            PackedGridData<D2Q9, DataType_>  data;
            PackedGridInfo<D2Q9> info;

            GridPacker<D2Q9, NOSLIP, DataType_>::pack(grid, info, data);

            SolverLBMGrid<Tag_, lbm_applications::LABSWE, DataType_,lbm_force::CENTRED, lbm_source_schemes::BED_FULL, lbm_grid_types::RECTANGULAR, lbm_lattice_types::D2Q9, lbm_boundary_types::NOSLIP, lbm_modes::DRY> solver(&info, &data, grid.d_x, grid.d_y, grid.d_t, grid.tau);

            solver.do_preprocessing();
            for(unsigned long i(0); i < _max_timesteps; ++i)
            {
#ifdef SOLVER_VERBOSE
                std::cout<<"Timestep: " << i << "/" << _max_timesteps << std::endl;
#endif
                solver.solve();
#ifdef SOLVER_POSTPROCESSING
                solver.do_postprocessing();
                GridPacker<D2Q9, NOSLIP, DataType_>::unpack(grid, info, data);
                //PostProcessing<GNUPLOT>::value(*grid.h, _max_timesteps - 1 , h.columns(), h.rows(), i);
                PostProcessing<GNUPLOT>::value(*grid.h, 10, h.columns(), h.rows(), i);
#endif
            }
            solver.do_postprocessing();
            GridPacker<D2Q9, NOSLIP, DataType_>::unpack(grid, info, data);

            DenseMatrix<DataType_> h_plus_b(grid.h->copy());
            Sum<Tag_>::value(h_plus_b, b);
            h_plus_b.lock(lm_read_and_write);
            DataType_ max_v(0);
            for(unsigned long i(0) ; i < grid.h->rows() ; ++i)
                for(unsigned long j(0) ; j < grid.h->columns() ; ++j)
                    max_v = std::max(max_v, (h_plus_b)[i][j]);

            //0->0, max_v->1
            for(unsigned long i(0) ; i < grid.h->rows() ; ++i)
                for(unsigned long j(0) ; j < grid.h->columns() ; ++j)
                    (h_plus_b)[i][j] /= max_v;
            h_plus_b.unlock(lm_read_and_write);

            BitmapIO<io_formats::PPM>::write_scalar_field(h_plus_b, "ext_result_h_b_cdub.ppm");
            DenseMatrix<DataType_> bluppy2(BitmapIO<io_formats::PPM>::read_scalar_field("ext_result_h_b_cdub.ppm", DataType_(1.)));
        }

};
SolverLBMGridExternalComparisonTest_CDUB<tags::CPU, float> cpu_solver_test_float_cdub("float", "ext_initial_h_cdub.ppm", "ext_initial_vx_cdub.ppm", "ext_initial_vy_cdub.ppm", "ext_initial_b_cdub.ppm", "ext_obstacles_cdub.ppm", 200ul, 0.08f, 1.0f, 1.0f, 0.08f, 0.01f, 0.01f, 0.01f, 1.1f);
#ifdef HONEI_CUDA
SolverLBMGridExternalComparisonTest_CDUB<tags::GPU::CUDA, float> cuda_solver_test_float_cdub("float", "ext_initial_h_cdub.ppm", "ext_initial_vx_cdub.ppm", "ext_initial_vy_cdub.ppm", "ext_initial_b_cdub.ppm", "ext_obstacles_cdub.ppm", 200ul, 0.08f, 1.0f, 1.0f, 0.08f, 0.01f, 0.01f, 0.01f, 1.1f);
#endif
#ifdef HONEI_SSE
SolverLBMGridExternalComparisonTest_CDUB<tags::CPU::SSE, float> sse_solver_test_float_cdub("float", "ext_initial_h_cdub.ppm", "ext_initial_vx_cdub.ppm", "ext_initial_vy_cdub.ppm", "ext_initial_b_cdub.ppm", "ext_obstacles_cdub.ppm", 200ul, 0.08f, 1.0f, 1.0f, 0.08f, 0.01f, 0.01f, 0.01f, 1.1f);
#endif