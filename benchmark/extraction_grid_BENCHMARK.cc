/* vim: set number sw=4 sts=4 et nofoldenable : */

/*
 * Copyright (c) 2009 Dirk Ribbrock <dirk.ribbrock@uni-dortmund.de>
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

#ifndef ALLBENCH
#include <benchmark/benchmark.cc>
#include <tr1/memory>
#include <string>
#endif

#include <honei/lbm/extraction_grid.hh>
#include <honei/lbm/grid_packer.hh>
#include <honei/lbm/solver_lbm_grid.hh>
#include <honei/swe/volume.hh>
#include <honei/math/quadrature.hh>
#include <honei/backends/cuda/operations.hh>

using namespace std;
using namespace honei;
using namespace lbm;
using namespace lbm_lattice_types;

template <typename Tag_, typename DataType_, typename LbmMode_>
class ExtractionGridBench :
    public Benchmark
{
    private:
        unsigned long _size;
        int _count;
    public:
        ExtractionGridBench(const std::string & id, unsigned long size, int count) :
            Benchmark(id)
        {
            register_tag(Tag_::name);
            _size = size;
            _count = count;
        }

        virtual void run()
        {
            unsigned long g_h(_size);
            unsigned long g_w(_size);

            DenseMatrix<DataType_> h(g_h, g_w, DataType_(0.05));
            Cylinder<DataType_> c1(h, DataType_(0.02), 25, 25);
            c1.value();

            DenseMatrix<DataType_> u(g_h, g_w, DataType_(0.));
            DenseMatrix<DataType_> v(g_h, g_w, DataType_(0.));

            DenseMatrix<DataType_> b(g_h, g_w, DataType_(0.));

            Cylinder<DataType_> b1(b, DataType_(0.04), 15, 15);
            b1.value();

            Grid<D2Q9, DataType_> grid;
            DenseMatrix<bool> obstacles(g_h, g_w, false);
            grid.obstacles = &obstacles;
            grid.h = &h;
            grid.u = &u;
            grid.v = &v;
            grid.b = &b;

            PackedGridData<D2Q9, DataType_>  data;
            PackedGridInfo<D2Q9> info;

            GridPacker<D2Q9, NOSLIP, DataType_>::pack(grid, info, data);

            SolverLBMGrid<Tag_, lbm_applications::LABSWE, DataType_,lbm_force::CENTRED, lbm_source_schemes::BED_FULL, lbm_grid_types::RECTANGULAR, lbm_lattice_types::D2Q9, lbm_boundary_types::NOSLIP, LbmMode_> solver(&info, &data, 1., 1., 1., 1.5);

            solver.do_preprocessing();

            for(int i = 0; i < _count; ++i)
            {
                BENCHMARK(
                        for (unsigned long j(0) ; j < 5 ; ++j)
                        {
                        (ExtractionGrid<Tag_, LbmMode_>::value(info, data, DataType_(10e-5)));
                        }
                        if (Tag_::tag_value == tags::tv_gpu_cuda)
                            cuda_thread_synchronize();
                        );
            }
            BenchmarkInfo benchinfo(ExtractionGrid<tags::CPU, LbmMode_>::get_benchmark_info(&info, &data));
            evaluate(benchinfo * 5);
        }
};

ExtractionGridBench<tags::CPU, float, lbm_modes::DRY> dry_extraction_grid_bench_float("DRY ExtractionGridBench - size: 2000, float", 2000, 5);
ExtractionGridBench<tags::CPU, double, lbm_modes::DRY> dry_extraction_grid_bench_double("DRY ExtractionGridBench - size: 2000, double", 2000, 5);
#ifdef HONEI_SSE
ExtractionGridBench<tags::CPU::SSE, float, lbm_modes::DRY> dry_sse_extraction_grid_bench_float("DRY SSE ExtractionGridBench - size: 2000, float", 2000, 5);
ExtractionGridBench<tags::CPU::SSE, double, lbm_modes::DRY> dry_sse_extraction_grid_bench_double("DRY SSE ExtractionGridBench - size: 2000, double", 2000, 5);
#endif
#ifdef HONEI_CUDA
ExtractionGridBench<tags::GPU::CUDA, float, lbm_modes::DRY> dry_cuda_extraction_grid_bench_float("DRY CUDA ExtractionGridBench - size: 2000, float", 2000, 25);
#endif

ExtractionGridBench<tags::CPU, float, lbm_modes::WET> wet_extraction_grid_bench_float("WET ExtractionGridBench - size: 2000, float", 2000, 5);
ExtractionGridBench<tags::CPU, double, lbm_modes::WET> wet_extraction_grid_bench_double("WET ExtractionGridBench - size: 2000, double", 2000, 5);
#ifdef HONEI_SSE
ExtractionGridBench<tags::CPU::SSE, float, lbm_modes::WET> wet_sse_extraction_grid_bench_float("WET SSE ExtractionGridBench - size: 2000, float", 2000, 5);
ExtractionGridBench<tags::CPU::SSE, double, lbm_modes::WET> wet_sse_extraction_grid_bench_double("WET SSE ExtractionGridBench - size: 2000, double", 2000, 5);
#endif
#ifdef HONEI_CUDA
ExtractionGridBench<tags::GPU::CUDA, float, lbm_modes::WET> wet_cuda_extraction_grid_bench_float("WET CUDA ExtractionGridBench - size: 2000, float", 2000, 25);
#endif