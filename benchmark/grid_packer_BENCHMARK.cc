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

#include <string>
#endif

#include <honei/lbm/solver_lbm_grid.hh>
#include <honei/swe/volume.hh>
#include <iostream>
#include <honei/swe/volume.hh>
#include <honei/math/quadrature.hh>
#include <honei/lbm/grid.hh>
#include <honei/lbm/grid_packer.hh>

using namespace std;
using namespace honei;

template <typename Tag_, typename DataType_>
class GridPackerPackBench :
    public Benchmark
{
    private:
        unsigned long _size;
        int _count;
        int _parts;
    public:
        GridPackerPackBench(const std::string & id, unsigned long size, int count) :
            Benchmark(id)
        {
            register_tag(Tag_::name);
            _size = size;
            _count = count;
        }

        virtual void run()
        {
            for(int i = 0; i < _count; ++i)
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
                BENCHMARK((GridPacker<D2Q9, NOSLIP, DataType_>::pack(grid, info, data)));

                grid.destroy();
                info.destroy();
                data.destroy();
            }
            evaluate();
        }
};

GridPackerPackBench<tags::CPU, float> packer_pack_bench_float_1("LBM Grid packer pack Benchmark - size: 500x500, float", 500, 5);
GridPackerPackBench<tags::CPU, double> packer_pack_bench_double_1("LBM Grid packer pack Benchmark - size: 500x500, double", 500, 5);
