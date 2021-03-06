/* vim: set sw=4 sts=4 et foldmethod=syntax : */

/*
 * Copyright (c) 2011 Markus Geveler <apryde@gmx.de>
 *
 * This file is part of the Math C++ library. LibMath is free software;
 * you can redistribute it and/or modify it under the terms of the GNU General
 * Public License version 2, as published by the Free Software Foundation.
 *
 * LibMath is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc., 59 Temple
 * Place, Suite 330, Boston, MA  02111-1307  USA
 */

//#define SOLVER_VERBOSE 1
#include <honei/math/mg.hh>
#include <honei/math/bicgstab.hh>
#include <honei/math/methods.hh>
#include <honei/math/restriction.hh>
#include <honei/math/prolongation.hh>
#include <honei/math/matrix_io.hh>
#include <honei/math/vector_io.hh>
#include <benchmark/benchmark.hh>
#include <honei/util/stringify.hh>
#include <iostream>
#include <honei/backends/cuda/gpu_pool.hh>

//#include <cstdio>
//#include <cstdlib>

#include <fstream>
using namespace honei;
using namespace std;


template <typename Tag_, typename CycleType_, typename PreconContType_>
class MGBench:
    public Benchmark
{
    private:
        unsigned long _element_type;
        unsigned long _sorting;
        unsigned long _levels;
        std::string _precon;
        double _damping;

    public:
        MGBench(const std::string & tag, unsigned long et, unsigned long s, unsigned long l, std::string p, double d) :
            Benchmark(tag)
        {
            register_tag(Tag_::name);
            _element_type = et;
            _sorting = s;
            _levels = l;
            _precon = p;
            _damping = d;
        }

        virtual void run()
        {
            std::string file(HONEI_SOURCEDIR);
            file += "/honei/math/testdata/poisson_advanced2/";

            if(_element_type == 2)
                file += "q2_";
            else if(_element_type == 3)
                file += "q1t_";

            file += "sort_";

            file += stringify(_sorting);

            file += "/";

            if(typeid(Tag_) == typeid(tags::CPU::MultiCore::SSE))
            {
                if(_element_type == 1)
                {
                    Configuration::instance()->set_value("ell::threads", 2);
                }
                else
                {
                    Configuration::instance()->set_value("ell::threads", 16);
                }
            }
            else
            {
                if(_element_type == 1)
                {
                    Configuration::instance()->set_value("ell::threads", 1);
                    Configuration::instance()->set_value("cuda::product_smell_dv_double", 128);
                }
                else
                {
                    Configuration::instance()->set_value("ell::threads", 2);
                    Configuration::instance()->set_value("cuda::product_smell_dv_double", 256);
                }
            }


            MGData<SparseMatrixELL<double>, DenseVector<double>, SparseMatrixELL<double>, PreconContType_ >  data(MGUtil<Tag_,
                    SparseMatrixELL<double>,
                    DenseVector<double>,
                    SparseMatrixELL<double>,
                    PreconContType_,
                    io_formats::ELL,
                    io_formats::EXP,
                    double,
                    RISmoother<Tag_> >::load_data(file, _levels, _damping, _precon));
            MGUtil<Tag_,
                SparseMatrixELL<double>,
                DenseVector<double>,
                SparseMatrixELL<double>,
                PreconContType_,
                io_formats::ELL,
                io_formats::EXP,
                double,
                RISmoother<Tag_> >::configure(data, 100, 10, 4, 4, 1, double(1e-8));

            OperatorList ol(
                    MGCycleCreation<Tag_,
                    CycleType_,
                    BiCGStab<Tag_, methods::VAR>,
                    RISmoother<Tag_>,
                    Restriction<Tag_, methods::PROLMAT>,
                    Prolongation<Tag_, methods::PROLMAT>,
                    double>::value(data)
                    );



            BENCHMARK(
                    (MGSolver<Tag_, Norm<vnt_l_two, true, Tag_> >::value(data, ol));
#ifdef HONEI_CUDA
                    if (Tag_::tag_value == tags::tv_gpu_cuda)
                    cuda::GPUPool::instance()->flush();
#endif
                    );

            evaluate();

            std::cout << data.used_iters << " iterations used." << std::endl;
        }
};

#ifdef HONEI_SSE
//JAC
//q1
MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::V::STATIC, DenseVector<double> > mcsse_q1_sort0_l7_jac_v("MGBench mcsse | q1 | sort 0 | L7 | jac | V", 1 , 0, 7, "jac", 0.7);
MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::V::STATIC, DenseVector<double> > mcsse_q1_sort0_l8_jac_v("MGBench mcsse | q1 | sort 0 | L8 | jac | V", 1 , 0, 8, "jac", 0.7);
MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::V::STATIC, DenseVector<double> > mcsse_q1_sort0_l9_jac_v("MGBench mcsse | q1 | sort 0 | L9 | jac | V", 1 , 0, 9, "jac", 0.7);


MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::V::STATIC, DenseVector<double> > mcsse_q1_sort1_l7_jac_v("MGBench mcsse | q1 | sort 1 | L7 | jac | V", 1 , 1, 7, "jac", 0.7);
MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::V::STATIC, DenseVector<double> > mcsse_q1_sort1_l8_jac_v("MGBench mcsse | q1 | sort 1 | L8 | jac | V", 1 , 1, 8, "jac", 0.7);
MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::V::STATIC, DenseVector<double> > mcsse_q1_sort1_l9_jac_v("MGBench mcsse | q1 | sort 1 | L9 | jac | V", 1 , 1, 9, "jac", 0.7);


MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::V::STATIC, DenseVector<double> > mcsse_q1_sort2_l7_jac_v("MGBench mcsse | q1 | sort 2 | L7 | jac | V", 1 , 2, 7, "jac", 0.7);
MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::V::STATIC, DenseVector<double> > mcsse_q1_sort2_l8_jac_v("MGBench mcsse | q1 | sort 2 | L8 | jac | V", 1 , 2, 8, "jac", 0.7);
MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::V::STATIC, DenseVector<double> > mcsse_q1_sort2_l9_jac_v("MGBench mcsse | q1 | sort 2 | L9 | jac | V", 1 , 2, 9, "jac", 0.7);

MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::V::STATIC, DenseVector<double> > mcsse_q1_sort3_l7_jac_v("MGBench mcsse | q1 | sort 3 | L7 | jac | V", 1 , 3, 7, "jac", 0.7);
MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::V::STATIC, DenseVector<double> > mcsse_q1_sort3_l8_jac_v("MGBench mcsse | q1 | sort 3 | L8 | jac | V", 1 , 3, 8, "jac", 0.7);
MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::V::STATIC, DenseVector<double> > mcsse_q1_sort3_l9_jac_v("MGBench mcsse | q1 | sort 3 | L9 | jac | V", 1 , 3, 9, "jac", 0.7);


MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::V::STATIC, DenseVector<double> > mcsse_q1_sort4_l7_jac_v("MGBench mcsse | q1 | sort 4 | L7 | jac | V", 1 , 4, 7, "jac", 0.7);
MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::V::STATIC, DenseVector<double> > mcsse_q1_sort4_l8_jac_v("MGBench mcsse | q1 | sort 4 | L8 | jac | V", 1 , 4, 8, "jac", 0.7);
MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::V::STATIC, DenseVector<double> > mcsse_q1_sort4_l9_jac_v("MGBench mcsse | q1 | sort 4 | L9 | jac | V", 1 , 4, 9, "jac", 0.7);

//q2
MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::V::STATIC, DenseVector<double> > mcsse_q2_sort0_l6_jac_v("MGBench mcsse | q2 | sort 0 | L6 | jac | V", 2 , 0, 6, "jac", 0.7);
MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::V::STATIC, DenseVector<double> > mcsse_q2_sort0_l7_jac_v("MGBench mcsse | q2 | sort 0 | L7 | jac | V", 2 , 0, 7, "jac", 0.7);
MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::V::STATIC, DenseVector<double> > mcsse_q2_sort0_l8_jac_v("MGBench mcsse | q2 | sort 0 | L8 | jac | V", 2 , 0, 8, "jac", 0.7);


MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::V::STATIC, DenseVector<double> > mcsse_q2_sort1_l6_jac_v("MGBench mcsse | q2 | sort 1 | L6 | jac | V", 2 , 1, 6, "jac", 0.7);
MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::V::STATIC, DenseVector<double> > mcsse_q2_sort1_l7_jac_v("MGBench mcsse | q2 | sort 1 | L7 | jac | V", 2 , 1, 7, "jac", 0.7);
MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::V::STATIC, DenseVector<double> > mcsse_q2_sort1_l8_jac_v("MGBench mcsse | q2 | sort 1 | L8 | jac | V", 2 , 1, 8, "jac", 0.7);


MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::V::STATIC, DenseVector<double> > mcsse_q2_sort2_l6_jac_v("MGBench mcsse | q2 | sort 2 | L6 | jac | V", 2 , 2, 6, "jac", 0.7);
MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::V::STATIC, DenseVector<double> > mcsse_q2_sort2_l7_jac_v("MGBench mcsse | q2 | sort 2 | L7 | jac | V", 2 , 2, 7, "jac", 0.7);
MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::V::STATIC, DenseVector<double> > mcsse_q2_sort2_l8_jac_v("MGBench mcsse | q2 | sort 2 | L8 | jac | V", 2 , 2, 8, "jac", 0.7);

MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::V::STATIC, DenseVector<double> > mcsse_q2_sort3_l6_jac_v("MGBench mcsse | q2 | sort 3 | L6 | jac | V", 2 , 3, 6, "jac", 0.7);
MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::V::STATIC, DenseVector<double> > mcsse_q2_sort3_l7_jac_v("MGBench mcsse | q2 | sort 3 | L7 | jac | V", 2 , 3, 7, "jac", 0.7);
MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::V::STATIC, DenseVector<double> > mcsse_q2_sort3_l8_jac_v("MGBench mcsse | q2 | sort 3 | L8 | jac | V", 2 , 3, 8, "jac", 0.7);

MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::V::STATIC, DenseVector<double> > mcsse_q2_sort4_l6_jac_v("MGBench mcsse | q2 | sort 4 | L6 | jac | V", 2 , 4, 6, "jac", 0.7);
MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::V::STATIC, DenseVector<double> > mcsse_q2_sort4_l7_jac_v("MGBench mcsse | q2 | sort 4 | L7 | jac | V", 2 , 4, 7, "jac", 0.7);
MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::V::STATIC, DenseVector<double> > mcsse_q2_sort4_l8_jac_v("MGBench mcsse | q2 | sort 4 | L8 | jac | V", 2 , 4, 8, "jac", 0.7);

//q1t
MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::V::STATIC, DenseVector<double> > mcsse_q1t_sort0_l6_jac_v("MGBench mcsse | q1t | sort 0 | L6 | jac | V", 3, 0, 6, "jac", 0.7);
MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::V::STATIC, DenseVector<double> > mcsse_q1t_sort0_l7_jac_v("MGBench mcsse | q1t | sort 0 | L7 | jac | V", 3 , 0, 7, "jac", 0.7);
MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::V::STATIC, DenseVector<double> > mcsse_q1t_sort0_l8_jac_v("MGBench mcsse | q1t | sort 0 | L8 | jac | V", 3 , 0, 8, "jac", 0.7);


MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::V::STATIC, DenseVector<double> > mcsse_q1t_sort1_l6_jac_v("MGBench mcsse | q1t | sort 1 | L6 | jac | V", 3 , 1, 6, "jac", 0.7);
MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::V::STATIC, DenseVector<double> > mcsse_q1t_sort1_l7_jac_v("MGBench mcsse | q1t | sort 1 | L7 | jac | V", 3 , 1, 7, "jac", 0.7);
MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::V::STATIC, DenseVector<double> > mcsse_q1t_sort1_l8_jac_v("MGBench mcsse | q1t | sort 1 | L8 | jac | V", 3 , 1, 8, "jac", 0.7);


MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::V::STATIC, DenseVector<double> > mcsse_q1t_sort2_l6_jac_v("MGBench mcsse | q1t | sort 2 | L6 | jac | V", 3 , 2, 6, "jac", 0.7);
MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::V::STATIC, DenseVector<double> > mcsse_q1t_sort2_l7_jac_v("MGBench mcsse | q1t | sort 2 | L7 | jac | V", 3 , 2, 7, "jac", 0.7);
MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::V::STATIC, DenseVector<double> > mcsse_q1t_sort2_l8_jac_v("MGBench mcsse | q1t | sort 2 | L8 | jac | V", 3 , 2, 8, "jac", 0.7);

MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::V::STATIC, DenseVector<double> > mcsse_q1t_sort3_l6_jac_v("MGBench mcsse | q1t | sort 3 | L6 | jac | V", 3 , 3, 6, "jac", 0.7);
MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::V::STATIC, DenseVector<double> > mcsse_q1t_sort3_l7_jac_v("MGBench mcsse | q1t | sort 3 | L7 | jac | V", 3 , 3, 7, "jac", 0.7);
MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::V::STATIC, DenseVector<double> > mcsse_q1t_sort3_l8_jac_v("MGBench mcsse | q1t | sort 3 | L8 | jac | V", 3 , 3, 8, "jac", 0.7);

MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::V::STATIC, DenseVector<double> > mcsse_q1t_sort4_l6_jac_v("MGBench mcsse | q1t | sort 4 | L6 | jac | V", 3 , 4, 6, "jac", 0.7);
MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::V::STATIC, DenseVector<double> > mcsse_q1t_sort4_l7_jac_v("MGBench mcsse | q1t | sort 4 | L7 | jac | V", 3 , 4, 7, "jac", 0.7);
MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::V::STATIC, DenseVector<double> > mcsse_q1t_sort4_l8_jac_v("MGBench mcsse | q1t | sort 4 | L8 | jac | V", 3 , 4, 8, "jac", 0.7);

//SPAI
//q1
MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::V::STATIC, SparseMatrixELL<double> > mcsse_q1_sort0_l7_spai_grote_v("MGBench mcsse | q1 | sort 0 | L7 | spai_grote | V", 1 , 0, 7, "spai_grote", 1.);
MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::V::STATIC, SparseMatrixELL<double> > mcsse_q1_sort0_l8_spai_grote_v("MGBench mcsse | q1 | sort 0 | L8 | spai_grote | V", 1 , 0, 8, "spai_grote", 1.);
MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::V::STATIC, SparseMatrixELL<double> > mcsse_q1_sort0_l9_spai_grote_v("MGBench mcsse | q1 | sort 0 | L9 | spai_grote | V", 1 , 0, 9, "spai_grote", 1.);


MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::V::STATIC, SparseMatrixELL<double> > mcsse_q1_sort1_l7_spai_grote_v("MGBench mcsse | q1 | sort 1 | L7 | spai_grote | V", 1 , 1, 7, "spai_grote", 1.);
MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::V::STATIC, SparseMatrixELL<double> > mcsse_q1_sort1_l8_spai_grote_v("MGBench mcsse | q1 | sort 1 | L8 | spai_grote | V", 1 , 1, 8, "spai_grote", 1.);
MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::V::STATIC, SparseMatrixELL<double> > mcsse_q1_sort1_l9_spai_grote_v("MGBench mcsse | q1 | sort 1 | L9 | spai_grote | V", 1 , 1, 9, "spai_grote", 1.);


MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::V::STATIC, SparseMatrixELL<double> > mcsse_q1_sort2_l7_spai_grote_v("MGBench mcsse | q1 | sort 2 | L7 | spai_grote | V", 1 , 2, 7, "spai_grote", 1.);
MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::V::STATIC, SparseMatrixELL<double> > mcsse_q1_sort2_l8_spai_grote_v("MGBench mcsse | q1 | sort 2 | L8 | spai_grote | V", 1 , 2, 8, "spai_grote", 1.);
MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::V::STATIC, SparseMatrixELL<double> > mcsse_q1_sort2_l9_spai_grote_v("MGBench mcsse | q1 | sort 2 | L9 | spai_grote | V", 1 , 2, 9, "spai_grote", 1.);

MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::V::STATIC, SparseMatrixELL<double> > mcsse_q1_sort3_l7_spai_grote_v("MGBench mcsse | q1 | sort 3 | L7 | spai_grote | V", 1 , 3, 7, "spai_grote", 1.);
MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::V::STATIC, SparseMatrixELL<double> > mcsse_q1_sort3_l8_spai_grote_v("MGBench mcsse | q1 | sort 3 | L8 | spai_grote | V", 1 , 3, 8, "spai_grote", 1.);
MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::V::STATIC, SparseMatrixELL<double> > mcsse_q1_sort3_l9_spai_grote_v("MGBench mcsse | q1 | sort 3 | L9 | spai_grote | V", 1 , 3, 9, "spai_grote", 1.);

MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::V::STATIC, SparseMatrixELL<double> > mcsse_q1_sort4_l7_spai_grote_v("MGBench mcsse | q1 | sort 4 | L7 | spai_grote | V", 1 , 4, 7, "spai_grote", 1.);
MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::V::STATIC, SparseMatrixELL<double> > mcsse_q1_sort4_l8_spai_grote_v("MGBench mcsse | q1 | sort 4 | L8 | spai_grote | V", 1 , 4, 8, "spai_grote", 1.);
MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::V::STATIC, SparseMatrixELL<double> > mcsse_q1_sort4_l9_spai_grote_v("MGBench mcsse | q1 | sort 4 | L9 | spai_grote | V", 1 , 4, 9, "spai_grote", 1.);

//q2
MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::V::STATIC, SparseMatrixELL<double> > mcsse_q2_sort0_l6_spai_grote_v("MGBench mcsse | q2 | sort 0 | L6 | spai_grote | V", 2 , 0, 6, "spai_grote", 1.);
MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::V::STATIC, SparseMatrixELL<double> > mcsse_q2_sort0_l7_spai_grote_v("MGBench mcsse | q2 | sort 0 | L7 | spai_grote | V", 2 , 0, 7, "spai_grote", 1.);
MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::V::STATIC, SparseMatrixELL<double> > mcsse_q2_sort0_l8_spai_grote_v("MGBench mcsse | q2 | sort 0 | L8 | spai_grote | V", 2 , 0, 8, "spai_grote", 1.);


MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::V::STATIC, SparseMatrixELL<double> > mcsse_q2_sort1_l6_spai_grote_v("MGBench mcsse | q2 | sort 1 | L6 | spai_grote | V", 2 , 1, 6, "spai_grote", 1.);
MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::V::STATIC, SparseMatrixELL<double> > mcsse_q2_sort1_l7_spai_grote_v("MGBench mcsse | q2 | sort 1 | L7 | spai_grote | V", 2 , 1, 7, "spai_grote", 1.);
MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::V::STATIC, SparseMatrixELL<double> > mcsse_q2_sort1_l8_spai_grote_v("MGBench mcsse | q2 | sort 1 | L8 | spai_grote | V", 2 , 1, 8, "spai_grote", 1.);


MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::V::STATIC, SparseMatrixELL<double> > mcsse_q2_sort2_l6_spai_grote_v("MGBench mcsse | q2 | sort 2 | L6 | spai_grote | V", 2 , 2, 6, "spai_grote", 1.);
MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::V::STATIC, SparseMatrixELL<double> > mcsse_q2_sort2_l7_spai_grote_v("MGBench mcsse | q2 | sort 2 | L7 | spai_grote | V", 2 , 2, 7, "spai_grote", 1.);
MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::V::STATIC, SparseMatrixELL<double> > mcsse_q2_sort2_l8_spai_grote_v("MGBench mcsse | q2 | sort 2 | L8 | spai_grote | V", 2 , 2, 8, "spai_grote", 1.);

MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::V::STATIC, SparseMatrixELL<double> > mcsse_q2_sort3_l6_spai_grote_v("MGBench mcsse | q2 | sort 3 | L6 | spai_grote | V", 2 , 3, 6, "spai_grote", 1.);
MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::V::STATIC, SparseMatrixELL<double> > mcsse_q2_sort3_l7_spai_grote_v("MGBench mcsse | q2 | sort 3 | L7 | spai_grote | V", 2 , 3, 7, "spai_grote", 1.);
MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::V::STATIC, SparseMatrixELL<double> > mcsse_q2_sort3_l8_spai_grote_v("MGBench mcsse | q2 | sort 3 | L8 | spai_grote | V", 2 , 3, 8, "spai_grote", 1.);

MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::V::STATIC, SparseMatrixELL<double> > mcsse_q2_sort4_l6_spai_grote_v("MGBench mcsse | q2 | sort 4 | L6 | spai_grote | V", 2 , 4, 6, "spai_grote", 1.);
MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::V::STATIC, SparseMatrixELL<double> > mcsse_q2_sort4_l7_spai_grote_v("MGBench mcsse | q2 | sort 4 | L7 | spai_grote | V", 2 , 4, 7, "spai_grote", 1.);
MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::V::STATIC, SparseMatrixELL<double> > mcsse_q2_sort4_l8_spai_grote_v("MGBench mcsse | q2 | sort 4 | L8 | spai_grote | V", 2 , 4, 8, "spai_grote", 1.);

//q1t
MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::V::STATIC, SparseMatrixELL<double> > mcsse_q1t_sort0_l6_spai_grote_v("MGBench mcsse | q1t | sort 0 | L6 | spai_grote | V", 3, 0, 6, "spai_grote", 1.);
MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::V::STATIC, SparseMatrixELL<double> > mcsse_q1t_sort0_l7_spai_grote_v("MGBench mcsse | q1t | sort 0 | L7 | spai_grote | V", 3 , 0, 7, "spai_grote", 1.);
MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::V::STATIC, SparseMatrixELL<double> > mcsse_q1t_sort0_l8_spai_grote_v("MGBench mcsse | q1t | sort 0 | L8 | spai_grote | V", 3 , 0, 8, "spai_grote", 1.);


MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::V::STATIC, SparseMatrixELL<double> > mcsse_q1t_sort1_l6_spai_grote_v("MGBench mcsse | q1t | sort 1 | L6 | spai_grote | V", 3 , 1, 6, "spai_grote", 1.);
MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::V::STATIC, SparseMatrixELL<double> > mcsse_q1t_sort1_l7_spai_grote_v("MGBench mcsse | q1t | sort 1 | L7 | spai_grote | V", 3 , 1, 7, "spai_grote", 1.);
MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::V::STATIC, SparseMatrixELL<double> > mcsse_q1t_sort1_l8_spai_grote_v("MGBench mcsse | q1t | sort 1 | L8 | spai_grote | V", 3 , 1, 8, "spai_grote", 1.);


MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::V::STATIC, SparseMatrixELL<double> > mcsse_q1t_sort2_l6_spai_grote_v("MGBench mcsse | q1t | sort 2 | L6 | spai_grote | V", 3 , 2, 6, "spai_grote", 1.);
MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::V::STATIC, SparseMatrixELL<double> > mcsse_q1t_sort2_l7_spai_grote_v("MGBench mcsse | q1t | sort 2 | L7 | spai_grote | V", 3 , 2, 7, "spai_grote", 1.);
MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::V::STATIC, SparseMatrixELL<double> > mcsse_q1t_sort2_l8_spai_grote_v("MGBench mcsse | q1t | sort 2 | L8 | spai_grote | V", 3 , 2, 8, "spai_grote", 1.);

MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::V::STATIC, SparseMatrixELL<double> > mcsse_q1t_sort3_l6_spai_grote_v("MGBench mcsse | q1t | sort 3 | L6 | spai_grote | V", 3 , 3, 6, "spai_grote", 1.);
MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::V::STATIC, SparseMatrixELL<double> > mcsse_q1t_sort3_l7_spai_grote_v("MGBench mcsse | q1t | sort 3 | L7 | spai_grote | V", 3 , 3, 7, "spai_grote", 1.);
MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::V::STATIC, SparseMatrixELL<double> > mcsse_q1t_sort3_l8_spai_grote_v("MGBench mcsse | q1t | sort 3 | L8 | spai_grote | V", 3 , 3, 8, "spai_grote", 1.);

MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::V::STATIC, SparseMatrixELL<double> > mcsse_q1t_sort4_l6_spai_grote_v("MGBench mcsse | q1t | sort 4 | L6 | spai_grote | V", 3 , 4, 6, "spai_grote", 1.);
MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::V::STATIC, SparseMatrixELL<double> > mcsse_q1t_sort4_l7_spai_grote_v("MGBench mcsse | q1t | sort 4 | L7 | spai_grote | V", 3 , 4, 7, "spai_grote", 1.);
MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::V::STATIC, SparseMatrixELL<double> > mcsse_q1t_sort4_l8_spai_grote_v("MGBench mcsse | q1t | sort 4 | L8 | spai_grote | V", 3 , 4, 8, "spai_grote", 1.);

//SAINV
//q1
MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::V::STATIC, SparseMatrixELL<double> > mcsse_q1_sort0_l7_sainv_v("MGBench mcsse | q1 | sort 0 | L7 | sainv | V", 1 , 0, 7, "sainv", 0.7);
MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::V::STATIC, SparseMatrixELL<double> > mcsse_q1_sort0_l8_sainv_v("MGBench mcsse | q1 | sort 0 | L8 | sainv | V", 1 , 0, 8, "sainv", 0.7);
MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::V::STATIC, SparseMatrixELL<double> > mcsse_q1_sort0_l9_sainv_v("MGBench mcsse | q1 | sort 0 | L9 | sainv | V", 1 , 0, 9, "sainv", 0.7);


MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::V::STATIC, SparseMatrixELL<double> > mcsse_q1_sort1_l7_sainv_v("MGBench mcsse | q1 | sort 1 | L7 | sainv | V", 1 , 1, 7, "sainv", 0.7);
MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::V::STATIC, SparseMatrixELL<double> > mcsse_q1_sort1_l8_sainv_v("MGBench mcsse | q1 | sort 1 | L8 | sainv | V", 1 , 1, 8, "sainv", 0.7);
MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::V::STATIC, SparseMatrixELL<double> > mcsse_q1_sort1_l9_sainv_v("MGBench mcsse | q1 | sort 1 | L9 | sainv | V", 1 , 1, 9, "sainv", 0.7);


MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::V::STATIC, SparseMatrixELL<double> > mcsse_q1_sort2_l7_sainv_v("MGBench mcsse | q1 | sort 2 | L7 | sainv | V", 1 , 2, 7, "sainv", 0.7);
MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::V::STATIC, SparseMatrixELL<double> > mcsse_q1_sort2_l8_sainv_v("MGBench mcsse | q1 | sort 2 | L8 | sainv | V", 1 , 2, 8, "sainv", 0.7);
MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::V::STATIC, SparseMatrixELL<double> > mcsse_q1_sort2_l9_sainv_v("MGBench mcsse | q1 | sort 2 | L9 | sainv | V", 1 , 2, 9, "sainv", 0.7);

MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::V::STATIC, SparseMatrixELL<double> > mcsse_q1_sort3_l7_sainv_v("MGBench mcsse | q1 | sort 3 | L7 | sainv | V", 1 , 3, 7, "sainv", 0.7);
MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::V::STATIC, SparseMatrixELL<double> > mcsse_q1_sort3_l8_sainv_v("MGBench mcsse | q1 | sort 3 | L8 | sainv | V", 1 , 3, 8, "sainv", 0.7);
MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::V::STATIC, SparseMatrixELL<double> > mcsse_q1_sort3_l9_sainv_v("MGBench mcsse | q1 | sort 3 | L9 | sainv | V", 1 , 3, 9, "sainv", 0.7);

MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::V::STATIC, SparseMatrixELL<double> > mcsse_q1_sort4_l7_sainv_v("MGBench mcsse | q1 | sort 4 | L7 | sainv | V", 1 , 4, 7, "sainv", 0.7);
MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::V::STATIC, SparseMatrixELL<double> > mcsse_q1_sort4_l8_sainv_v("MGBench mcsse | q1 | sort 4 | L8 | sainv | V", 1 , 4, 8, "sainv", 0.7);
MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::V::STATIC, SparseMatrixELL<double> > mcsse_q1_sort4_l9_sainv_v("MGBench mcsse | q1 | sort 4 | L9 | sainv | V", 1 , 4, 9, "sainv", 0.7);

//q2
MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::V::STATIC, SparseMatrixELL<double> > mcsse_q2_sort0_l6_sainv_v("MGBench mcsse | q2 | sort 0 | L6 | sainv | V", 2 , 0, 6, "sainv", 0.7);
MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::V::STATIC, SparseMatrixELL<double> > mcsse_q2_sort0_l7_sainv_v("MGBench mcsse | q2 | sort 0 | L7 | sainv | V", 2 , 0, 7, "sainv", 0.7);
MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::V::STATIC, SparseMatrixELL<double> > mcsse_q2_sort0_l8_sainv_v("MGBench mcsse | q2 | sort 0 | L8 | sainv | V", 2 , 0, 8, "sainv", 0.7);


MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::V::STATIC, SparseMatrixELL<double> > mcsse_q2_sort1_l6_sainv_v("MGBench mcsse | q2 | sort 1 | L6 | sainv | V", 2 , 1, 6, "sainv", 0.7);
MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::V::STATIC, SparseMatrixELL<double> > mcsse_q2_sort1_l7_sainv_v("MGBench mcsse | q2 | sort 1 | L7 | sainv | V", 2 , 1, 7, "sainv", 0.7);
MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::V::STATIC, SparseMatrixELL<double> > mcsse_q2_sort1_l8_sainv_v("MGBench mcsse | q2 | sort 1 | L8 | sainv | V", 2 , 1, 8, "sainv", 0.7);


MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::V::STATIC, SparseMatrixELL<double> > mcsse_q2_sort2_l6_sainv_v("MGBench mcsse | q2 | sort 2 | L6 | sainv | V", 2 , 2, 6, "sainv", 0.7);
MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::V::STATIC, SparseMatrixELL<double> > mcsse_q2_sort2_l7_sainv_v("MGBench mcsse | q2 | sort 2 | L7 | sainv | V", 2 , 2, 7, "sainv", 0.7);
MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::V::STATIC, SparseMatrixELL<double> > mcsse_q2_sort2_l8_sainv_v("MGBench mcsse | q2 | sort 2 | L8 | sainv | V", 2 , 2, 8, "sainv", 0.7);

MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::V::STATIC, SparseMatrixELL<double> > mcsse_q2_sort3_l6_sainv_v("MGBench mcsse | q2 | sort 3 | L6 | sainv | V", 2 , 3, 6, "sainv", 0.7);
MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::V::STATIC, SparseMatrixELL<double> > mcsse_q2_sort3_l7_sainv_v("MGBench mcsse | q2 | sort 3 | L7 | sainv | V", 2 , 3, 7, "sainv", 0.7);
MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::V::STATIC, SparseMatrixELL<double> > mcsse_q2_sort3_l8_sainv_v("MGBench mcsse | q2 | sort 3 | L8 | sainv | V", 2 , 3, 8, "sainv", 0.7);

MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::V::STATIC, SparseMatrixELL<double> > mcsse_q2_sort4_l6_sainv_v("MGBench mcsse | q2 | sort 4 | L6 | sainv | V", 2 , 4, 6, "sainv", 0.7);
MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::V::STATIC, SparseMatrixELL<double> > mcsse_q2_sort4_l7_sainv_v("MGBench mcsse | q2 | sort 4 | L7 | sainv | V", 2 , 4, 7, "sainv", 0.7);
MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::V::STATIC, SparseMatrixELL<double> > mcsse_q2_sort4_l8_sainv_v("MGBench mcsse | q2 | sort 4 | L8 | sainv | V", 2 , 4, 8, "sainv", 0.7);


//q1t
MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::V::STATIC, SparseMatrixELL<double> > mcsse_q1t_sort0_l6_sainv_v("MGBench mcsse | q1t | sort 0 | L6 | sainv | V", 3 , 0, 6, "sainv", 0.7);
MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::V::STATIC, SparseMatrixELL<double> > mcsse_q1t_sort0_l7_sainv_v("MGBench mcsse | q1t | sort 0 | L7 | sainv | V", 3 , 0, 7, "sainv", 0.7);
MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::V::STATIC, SparseMatrixELL<double> > mcsse_q1t_sort0_l8_sainv_v("MGBench mcsse | q1t | sort 0 | L8 | sainv | V", 3 , 0, 8, "sainv", 0.7);


MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::V::STATIC, SparseMatrixELL<double> > mcsse_q1t_sort1_l6_sainv_v("MGBench mcsse | q1t | sort 1 | L6 | sainv | V", 3 , 1, 6, "sainv", 0.7);
MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::V::STATIC, SparseMatrixELL<double> > mcsse_q1t_sort1_l7_sainv_v("MGBench mcsse | q1t | sort 1 | L7 | sainv | V", 3 , 1, 7, "sainv", 0.7);
MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::V::STATIC, SparseMatrixELL<double> > mcsse_q1t_sort1_l8_sainv_v("MGBench mcsse | q1t | sort 1 | L8 | sainv | V", 3 , 1, 8, "sainv", 0.7);


MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::V::STATIC, SparseMatrixELL<double> > mcsse_q1t_sort2_l6_sainv_v("MGBench mcsse | q1t | sort 2 | L6 | sainv | V", 3 , 2, 6, "sainv", 0.5);
MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::V::STATIC, SparseMatrixELL<double> > mcsse_q1t_sort2_l7_sainv_v("MGBench mcsse | q1t | sort 2 | L7 | sainv | V", 3 , 2, 7, "sainv", 0.5);
MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::V::STATIC, SparseMatrixELL<double> > mcsse_q1t_sort2_l8_sainv_v("MGBench mcsse | q1t | sort 2 | L8 | sainv | V", 3 , 2, 8, "sainv", 0.5);

MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::V::STATIC, SparseMatrixELL<double> > mcsse_q1t_sort3_l6_sainv_v("MGBench mcsse | q1t | sort 3 | L6 | sainv | V", 3 , 3, 6, "sainv", 0.7);
MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::V::STATIC, SparseMatrixELL<double> > mcsse_q1t_sort3_l7_sainv_v("MGBench mcsse | q1t | sort 3 | L7 | sainv | V", 3 , 3, 7, "sainv", 0.7);
MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::V::STATIC, SparseMatrixELL<double> > mcsse_q1t_sort3_l8_sainv_v("MGBench mcsse | q1t | sort 3 | L8 | sainv | V", 3 , 3, 8, "sainv", 0.7);

MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::V::STATIC, SparseMatrixELL<double> > mcsse_q1t_sort4_l6_sainv_v("MGBench mcsse | q1t | sort 4 | L6 | sainv | V", 3 , 4, 6, "sainv", 0.7);
MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::V::STATIC, SparseMatrixELL<double> > mcsse_q1t_sort4_l7_sainv_v("MGBench mcsse | q1t | sort 4 | L7 | sainv | V", 3 , 4, 7, "sainv", 0.7);
MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::V::STATIC, SparseMatrixELL<double> > mcsse_q1t_sort4_l8_sainv_v("MGBench mcsse | q1t | sort 4 | L8 | sainv | V", 3 , 4, 8, "sainv", 0.7);
#endif

#if defined HONEI_CUDA && defined HONEI_CUDA_DOUBLE
//JAC
//q1
MGBench<tags::GPU::CUDA, methods::CYCLE::V::STATIC, DenseVector<double> > cuda_q1_sort0_l7_jac_v("MGBench cuda | q1 | sort 0 | L7 | jac | V", 1 , 0, 7, "jac", 0.7);
MGBench<tags::GPU::CUDA, methods::CYCLE::V::STATIC, DenseVector<double> > cuda_q1_sort0_l8_jac_v("MGBench cuda | q1 | sort 0 | L8 | jac | V", 1 , 0, 8, "jac", 0.7);
MGBench<tags::GPU::CUDA, methods::CYCLE::V::STATIC, DenseVector<double> > cuda_q1_sort0_l9_jac_v("MGBench cuda | q1 | sort 0 | L9 | jac | V", 1 , 0, 9, "jac", 0.7);


MGBench<tags::GPU::CUDA, methods::CYCLE::V::STATIC, DenseVector<double> > cuda_q1_sort1_l7_jac_v("MGBench cuda | q1 | sort 1 | L7 | jac | V", 1 , 1, 7, "jac", 0.7);
MGBench<tags::GPU::CUDA, methods::CYCLE::V::STATIC, DenseVector<double> > cuda_q1_sort1_l8_jac_v("MGBench cuda | q1 | sort 1 | L8 | jac | V", 1 , 1, 8, "jac", 0.7);
MGBench<tags::GPU::CUDA, methods::CYCLE::V::STATIC, DenseVector<double> > cuda_q1_sort1_l9_jac_v("MGBench cuda | q1 | sort 1 | L9 | jac | V", 1 , 1, 9, "jac", 0.7);


MGBench<tags::GPU::CUDA, methods::CYCLE::V::STATIC, DenseVector<double> > cuda_q1_sort2_l7_jac_v("MGBench cuda | q1 | sort 2 | L7 | jac | V", 1 , 2, 7, "jac", 0.7);
MGBench<tags::GPU::CUDA, methods::CYCLE::V::STATIC, DenseVector<double> > cuda_q1_sort2_l8_jac_v("MGBench cuda | q1 | sort 2 | L8 | jac | V", 1 , 2, 8, "jac", 0.7);
MGBench<tags::GPU::CUDA, methods::CYCLE::V::STATIC, DenseVector<double> > cuda_q1_sort2_l9_jac_v("MGBench cuda | q1 | sort 2 | L9 | jac | V", 1 , 2, 9, "jac", 0.7);

MGBench<tags::GPU::CUDA, methods::CYCLE::V::STATIC, DenseVector<double> > cuda_q1_sort3_l7_jac_v("MGBench cuda | q1 | sort 3 | L7 | jac | V", 1 , 3, 7, "jac", 0.7);
MGBench<tags::GPU::CUDA, methods::CYCLE::V::STATIC, DenseVector<double> > cuda_q1_sort3_l8_jac_v("MGBench cuda | q1 | sort 3 | L8 | jac | V", 1 , 3, 8, "jac", 0.7);
MGBench<tags::GPU::CUDA, methods::CYCLE::V::STATIC, DenseVector<double> > cuda_q1_sort3_l9_jac_v("MGBench cuda | q1 | sort 3 | L9 | jac | V", 1 , 3, 9, "jac", 0.7);

MGBench<tags::GPU::CUDA, methods::CYCLE::V::STATIC, DenseVector<double> > cuda_q1_sort4_l7_jac_v("MGBench cuda | q1 | sort 4 | L7 | jac | V", 1 , 4, 7, "jac", 0.7);
MGBench<tags::GPU::CUDA, methods::CYCLE::V::STATIC, DenseVector<double> > cuda_q1_sort4_l8_jac_v("MGBench cuda | q1 | sort 4 | L8 | jac | V", 1 , 4, 8, "jac", 0.7);
MGBench<tags::GPU::CUDA, methods::CYCLE::V::STATIC, DenseVector<double> > cuda_q1_sort4_l9_jac_v("MGBench cuda | q1 | sort 4 | L9 | jac | V", 1 , 4, 9, "jac", 0.7);

//q2
MGBench<tags::GPU::CUDA, methods::CYCLE::V::STATIC, DenseVector<double> > cuda_q2_sort0_l6_jac_v("MGBench cuda | q2 | sort 0 | L6 | jac | V", 2 , 0, 6, "jac", 0.7);
MGBench<tags::GPU::CUDA, methods::CYCLE::V::STATIC, DenseVector<double> > cuda_q2_sort0_l7_jac_v("MGBench cuda | q2 | sort 0 | L7 | jac | V", 2 , 0, 7, "jac", 0.7);
MGBench<tags::GPU::CUDA, methods::CYCLE::V::STATIC, DenseVector<double> > cuda_q2_sort0_l8_jac_v("MGBench cuda | q2 | sort 0 | L8 | jac | V", 2 , 0, 8, "jac", 0.7);


MGBench<tags::GPU::CUDA, methods::CYCLE::V::STATIC, DenseVector<double> > cuda_q2_sort1_l6_jac_v("MGBench cuda | q2 | sort 1 | L6 | jac | V", 2 , 1, 6, "jac", 0.7);
MGBench<tags::GPU::CUDA, methods::CYCLE::V::STATIC, DenseVector<double> > cuda_q2_sort1_l7_jac_v("MGBench cuda | q2 | sort 1 | L7 | jac | V", 2 , 1, 7, "jac", 0.7);
MGBench<tags::GPU::CUDA, methods::CYCLE::V::STATIC, DenseVector<double> > cuda_q2_sort1_l8_jac_v("MGBench cuda | q2 | sort 1 | L8 | jac | V", 2 , 1, 8, "jac", 0.7);


MGBench<tags::GPU::CUDA, methods::CYCLE::V::STATIC, DenseVector<double> > cuda_q2_sort2_l6_jac_v("MGBench cuda | q2 | sort 2 | L6 | jac | V", 2 , 2, 6, "jac", 0.7);
MGBench<tags::GPU::CUDA, methods::CYCLE::V::STATIC, DenseVector<double> > cuda_q2_sort2_l7_jac_v("MGBench cuda | q2 | sort 2 | L7 | jac | V", 2 , 2, 7, "jac", 0.7);
MGBench<tags::GPU::CUDA, methods::CYCLE::V::STATIC, DenseVector<double> > cuda_q2_sort2_l8_jac_v("MGBench cuda | q2 | sort 2 | L8 | jac | V", 2 , 2, 8, "jac", 0.7);

MGBench<tags::GPU::CUDA, methods::CYCLE::V::STATIC, DenseVector<double> > cuda_q2_sort3_l6_jac_v("MGBench cuda | q2 | sort 3 | L6 | jac | V", 2 , 3, 6, "jac", 0.7);
MGBench<tags::GPU::CUDA, methods::CYCLE::V::STATIC, DenseVector<double> > cuda_q2_sort3_l7_jac_v("MGBench cuda | q2 | sort 3 | L7 | jac | V", 2 , 3, 7, "jac", 0.7);
MGBench<tags::GPU::CUDA, methods::CYCLE::V::STATIC, DenseVector<double> > cuda_q2_sort3_l8_jac_v("MGBench cuda | q2 | sort 3 | L8 | jac | V", 2 , 3, 8, "jac", 0.7);

MGBench<tags::GPU::CUDA, methods::CYCLE::V::STATIC, DenseVector<double> > cuda_q2_sort4_l6_jac_v("MGBench cuda | q2 | sort 4 | L6 | jac | V", 2 , 4, 6, "jac", 0.7);
MGBench<tags::GPU::CUDA, methods::CYCLE::V::STATIC, DenseVector<double> > cuda_q2_sort4_l7_jac_v("MGBench cuda | q2 | sort 4 | L7 | jac | V", 2 , 4, 7, "jac", 0.7);
MGBench<tags::GPU::CUDA, methods::CYCLE::V::STATIC, DenseVector<double> > cuda_q2_sort4_l8_jac_v("MGBench cuda | q2 | sort 4 | L8 | jac | V", 2 , 4, 8, "jac", 0.7);

//q1t
MGBench<tags::GPU::CUDA, methods::CYCLE::V::STATIC, DenseVector<double> > cuda_q1t_sort0_l6_jac_v("MGBench cuda | q1t | sort 0 | L6 | jac | V", 3 , 0, 6, "jac", 0.7);
MGBench<tags::GPU::CUDA, methods::CYCLE::V::STATIC, DenseVector<double> > cuda_q1t_sort0_l7_jac_v("MGBench cuda | q1t | sort 0 | L7 | jac | V", 3 , 0, 7, "jac", 0.7);
MGBench<tags::GPU::CUDA, methods::CYCLE::V::STATIC, DenseVector<double> > cuda_q1t_sort0_l8_jac_v("MGBench cuda | q1t | sort 0 | L8 | jac | V", 3 , 0, 8, "jac", 0.7);


MGBench<tags::GPU::CUDA, methods::CYCLE::V::STATIC, DenseVector<double> > cuda_q1t_sort1_l6_jac_v("MGBench cuda | q1t | sort 1 | L6 | jac | V", 3 , 1, 6, "jac", 0.7);
MGBench<tags::GPU::CUDA, methods::CYCLE::V::STATIC, DenseVector<double> > cuda_q1t_sort1_l7_jac_v("MGBench cuda | q1t | sort 1 | L7 | jac | V", 3 , 1, 7, "jac", 0.7);
MGBench<tags::GPU::CUDA, methods::CYCLE::V::STATIC, DenseVector<double> > cuda_q1t_sort1_l8_jac_v("MGBench cuda | q1t | sort 1 | L8 | jac | V", 3 , 1, 8, "jac", 0.7);


MGBench<tags::GPU::CUDA, methods::CYCLE::V::STATIC, DenseVector<double> > cuda_q1t_sort2_l6_jac_v("MGBench cuda | q1t | sort 2 | L6 | jac | V", 3 , 2, 6, "jac", 0.7);
MGBench<tags::GPU::CUDA, methods::CYCLE::V::STATIC, DenseVector<double> > cuda_q1t_sort2_l7_jac_v("MGBench cuda | q1t | sort 2 | L7 | jac | V", 3 , 2, 7, "jac", 0.7);
MGBench<tags::GPU::CUDA, methods::CYCLE::V::STATIC, DenseVector<double> > cuda_q1t_sort2_l8_jac_v("MGBench cuda | q1t | sort 2 | L8 | jac | V", 3 , 2, 8, "jac", 0.7);

MGBench<tags::GPU::CUDA, methods::CYCLE::V::STATIC, DenseVector<double> > cuda_q1t_sort3_l6_jac_v("MGBench cuda | q1t | sort 3 | L6 | jac | V", 3 , 3, 6, "jac", 0.7);
MGBench<tags::GPU::CUDA, methods::CYCLE::V::STATIC, DenseVector<double> > cuda_q1t_sort3_l7_jac_v("MGBench cuda | q1t | sort 3 | L7 | jac | V", 3 , 3, 7, "jac", 0.7);
MGBench<tags::GPU::CUDA, methods::CYCLE::V::STATIC, DenseVector<double> > cuda_q1t_sort3_l8_jac_v("MGBench cuda | q1t | sort 3 | L8 | jac | V", 3 , 3, 8, "jac", 0.7);

MGBench<tags::GPU::CUDA, methods::CYCLE::V::STATIC, DenseVector<double> > cuda_q1t_sort4_l6_jac_v("MGBench cuda | q1t | sort 4 | L6 | jac | V", 3 , 4, 6, "jac", 0.7);
MGBench<tags::GPU::CUDA, methods::CYCLE::V::STATIC, DenseVector<double> > cuda_q1t_sort4_l7_jac_v("MGBench cuda | q1t | sort 4 | L7 | jac | V", 3 , 4, 7, "jac", 0.7);
MGBench<tags::GPU::CUDA, methods::CYCLE::V::STATIC, DenseVector<double> > cuda_q1t_sort4_l8_jac_v("MGBench cuda | q1t | sort 4 | L8 | jac | V", 3 , 4, 8, "jac", 0.7);

//SPAI
//q1
MGBench<tags::GPU::CUDA, methods::CYCLE::V::STATIC, SparseMatrixELL<double> > cuda_q1_sort0_l7_spai_grote_v("MGBench cuda | q1 | sort 0 | L7 | spai_grote | V", 1 , 0, 7, "spai_grote", 1.);
MGBench<tags::GPU::CUDA, methods::CYCLE::V::STATIC, SparseMatrixELL<double> > cuda_q1_sort0_l8_spai_grote_v("MGBench cuda | q1 | sort 0 | L8 | spai_grote | V", 1 , 0, 8, "spai_grote", 1.);
MGBench<tags::GPU::CUDA, methods::CYCLE::V::STATIC, SparseMatrixELL<double> > cuda_q1_sort0_l9_spai_grote_v("MGBench cuda | q1 | sort 0 | L9 | spai_grote | V", 1 , 0, 9, "spai_grote", 1.);


MGBench<tags::GPU::CUDA, methods::CYCLE::V::STATIC, SparseMatrixELL<double> > cuda_q1_sort1_l7_spai_grote_v("MGBench cuda | q1 | sort 1 | L7 | spai_grote | V", 1 , 1, 7, "spai_grote", 1.);
MGBench<tags::GPU::CUDA, methods::CYCLE::V::STATIC, SparseMatrixELL<double> > cuda_q1_sort1_l8_spai_grote_v("MGBench cuda | q1 | sort 1 | L8 | spai_grote | V", 1 , 1, 8, "spai_grote", 1.);
MGBench<tags::GPU::CUDA, methods::CYCLE::V::STATIC, SparseMatrixELL<double> > cuda_q1_sort1_l9_spai_grote_v("MGBench cuda | q1 | sort 1 | L9 | spai_grote | V", 1 , 1, 9, "spai_grote", 1.);


MGBench<tags::GPU::CUDA, methods::CYCLE::V::STATIC, SparseMatrixELL<double> > cuda_q1_sort2_l7_spai_grote_v("MGBench cuda | q1 | sort 2 | L7 | spai_grote | V", 1 , 2, 7, "spai_grote", 1.);
MGBench<tags::GPU::CUDA, methods::CYCLE::V::STATIC, SparseMatrixELL<double> > cuda_q1_sort2_l8_spai_grote_v("MGBench cuda | q1 | sort 2 | L8 | spai_grote | V", 1 , 2, 8, "spai_grote", 1.);
MGBench<tags::GPU::CUDA, methods::CYCLE::V::STATIC, SparseMatrixELL<double> > cuda_q1_sort2_l9_spai_grote_v("MGBench cuda | q1 | sort 2 | L9 | spai_grote | V", 1 , 2, 9, "spai_grote", 1.);

MGBench<tags::GPU::CUDA, methods::CYCLE::V::STATIC, SparseMatrixELL<double> > cuda_q1_sort3_l7_spai_grote_v("MGBench cuda | q1 | sort 3 | L7 | spai_grote | V", 1 , 3, 7, "spai_grote", 1.);
MGBench<tags::GPU::CUDA, methods::CYCLE::V::STATIC, SparseMatrixELL<double> > cuda_q1_sort3_l8_spai_grote_v("MGBench cuda | q1 | sort 3 | L8 | spai_grote | V", 1 , 3, 8, "spai_grote", 1.);
MGBench<tags::GPU::CUDA, methods::CYCLE::V::STATIC, SparseMatrixELL<double> > cuda_q1_sort3_l9_spai_grote_v("MGBench cuda | q1 | sort 3 | L9 | spai_grote | V", 1 , 3, 9, "spai_grote", 1.);

MGBench<tags::GPU::CUDA, methods::CYCLE::V::STATIC, SparseMatrixELL<double> > cuda_q1_sort4_l7_spai_grote_v("MGBench cuda | q1 | sort 4 | L7 | spai_grote | V", 1 , 4, 7, "spai_grote", 1.);
MGBench<tags::GPU::CUDA, methods::CYCLE::V::STATIC, SparseMatrixELL<double> > cuda_q1_sort4_l8_spai_grote_v("MGBench cuda | q1 | sort 4 | L8 | spai_grote | V", 1 , 4, 8, "spai_grote", 1.);
MGBench<tags::GPU::CUDA, methods::CYCLE::V::STATIC, SparseMatrixELL<double> > cuda_q1_sort4_l9_spai_grote_v("MGBench cuda | q1 | sort 4 | L9 | spai_grote | V", 1 , 4, 9, "spai_grote", 1.);

//q2
MGBench<tags::GPU::CUDA, methods::CYCLE::V::STATIC, SparseMatrixELL<double> > cuda_q2_sort0_l6_spai_grote_v("MGBench cuda | q2 | sort 0 | L6 | spai_grote | V", 2 , 0, 6, "spai_grote", 1.);
MGBench<tags::GPU::CUDA, methods::CYCLE::V::STATIC, SparseMatrixELL<double> > cuda_q2_sort0_l7_spai_grote_v("MGBench cuda | q2 | sort 0 | L7 | spai_grote | V", 2 , 0, 7, "spai_grote", 1.);
MGBench<tags::GPU::CUDA, methods::CYCLE::V::STATIC, SparseMatrixELL<double> > cuda_q2_sort0_l8_spai_grote_v("MGBench cuda | q2 | sort 0 | L8 | spai_grote | V", 2 , 0, 8, "spai_grote", 1.);


MGBench<tags::GPU::CUDA, methods::CYCLE::V::STATIC, SparseMatrixELL<double> > cuda_q2_sort1_l6_spai_grote_v("MGBench cuda | q2 | sort 1 | L6 | spai_grote | V", 2 , 1, 6, "spai_grote", 1.);
MGBench<tags::GPU::CUDA, methods::CYCLE::V::STATIC, SparseMatrixELL<double> > cuda_q2_sort1_l7_spai_grote_v("MGBench cuda | q2 | sort 1 | L7 | spai_grote | V", 2 , 1, 7, "spai_grote", 1.);
MGBench<tags::GPU::CUDA, methods::CYCLE::V::STATIC, SparseMatrixELL<double> > cuda_q2_sort1_l8_spai_grote_v("MGBench cuda | q2 | sort 1 | L8 | spai_grote | V", 2 , 1, 8, "spai_grote", 1.);

MGBench<tags::GPU::CUDA, methods::CYCLE::V::STATIC, SparseMatrixELL<double> > cuda_q2_sort2_l6_spai_grote_v("MGBench cuda | q2 | sort 2 | L6 | spai_grote | V", 2 , 1, 6, "spai_grote", 1.);
MGBench<tags::GPU::CUDA, methods::CYCLE::V::STATIC, SparseMatrixELL<double> > cuda_q2_sort2_l7_spai_grote_v("MGBench cuda | q2 | sort 2 | L7 | spai_grote | V", 2 , 1, 7, "spai_grote", 1.);
MGBench<tags::GPU::CUDA, methods::CYCLE::V::STATIC, SparseMatrixELL<double> > cuda_q2_sort2_l8_spai_grote_v("MGBench cuda | q2 | sort 2 | L8 | spai_grote | V", 2 , 1, 8, "spai_grote", 1.);

MGBench<tags::GPU::CUDA, methods::CYCLE::V::STATIC, SparseMatrixELL<double> > cuda_q2_sort3_l6_spai_grote_v("MGBench cuda | q2 | sort 3 | L6 | spai_grote | V", 2 , 3, 6, "spai_grote", 1.);
MGBench<tags::GPU::CUDA, methods::CYCLE::V::STATIC, SparseMatrixELL<double> > cuda_q2_sort3_l7_spai_grote_v("MGBench cuda | q2 | sort 3 | L7 | spai_grote | V", 2 , 3, 7, "spai_grote", 1.);
MGBench<tags::GPU::CUDA, methods::CYCLE::V::STATIC, SparseMatrixELL<double> > cuda_q2_sort3_l8_spai_grote_v("MGBench cuda | q2 | sort 3 | L8 | spai_grote | V", 2 , 3, 8, "spai_grote", 1.);


MGBench<tags::GPU::CUDA, methods::CYCLE::V::STATIC, SparseMatrixELL<double> > cuda_q2_sort4_l6_spai_grote_v("MGBench cuda | q2 | sort 4 | L6 | spai_grote | V", 2 , 4, 6, "spai_grote", 1.);
MGBench<tags::GPU::CUDA, methods::CYCLE::V::STATIC, SparseMatrixELL<double> > cuda_q2_sort4_l7_spai_grote_v("MGBench cuda | q2 | sort 4 | L7 | spai_grote | V", 2 , 4, 7, "spai_grote", 1.);
MGBench<tags::GPU::CUDA, methods::CYCLE::V::STATIC, SparseMatrixELL<double> > cuda_q2_sort4_l8_spai_grote_v("MGBench cuda | q2 | sort 4 | L8 | spai_grote | V", 2 , 4, 8, "spai_grote", 1.);

//q1t
MGBench<tags::GPU::CUDA, methods::CYCLE::V::STATIC, SparseMatrixELL<double> > cuda_q1t_sort0_l6_spai_grote_v("MGBench cuda | q1t | sort 0 | L6 | spai_grote | V", 3 , 0, 6, "spai_grote", 1.);
MGBench<tags::GPU::CUDA, methods::CYCLE::V::STATIC, SparseMatrixELL<double> > cuda_q1t_sort0_l7_spai_grote_v("MGBench cuda | q1t | sort 0 | L7 | spai_grote | V", 3 , 0, 7, "spai_grote", 1.);
MGBench<tags::GPU::CUDA, methods::CYCLE::V::STATIC, SparseMatrixELL<double> > cuda_q1t_sort0_l8_spai_grote_v("MGBench cuda | q1t | sort 0 | L8 | spai_grote | V", 3 , 0, 8, "spai_grote", 1.);


MGBench<tags::GPU::CUDA, methods::CYCLE::V::STATIC, SparseMatrixELL<double> > cuda_q1t_sort1_l6_spai_grote_v("MGBench cuda | q1t | sort 1 | L6 | spai_grote | V", 3 , 1, 6, "spai_grote", 1.);
MGBench<tags::GPU::CUDA, methods::CYCLE::V::STATIC, SparseMatrixELL<double> > cuda_q1t_sort1_l7_spai_grote_v("MGBench cuda | q1t | sort 1 | L7 | spai_grote | V", 3 , 1, 7, "spai_grote", 1.);
MGBench<tags::GPU::CUDA, methods::CYCLE::V::STATIC, SparseMatrixELL<double> > cuda_q1t_sort1_l8_spai_grote_v("MGBench cuda | q1t | sort 1 | L8 | spai_grote | V", 3 , 1, 8, "spai_grote", 1.);

MGBench<tags::GPU::CUDA, methods::CYCLE::V::STATIC, SparseMatrixELL<double> > cuda_q1t_sort2_l6_spai_grote_v("MGBench cuda | q1t | sort 2 | L6 | spai_grote | V", 3 , 1, 6, "spai_grote", 1.);
MGBench<tags::GPU::CUDA, methods::CYCLE::V::STATIC, SparseMatrixELL<double> > cuda_q1t_sort2_l7_spai_grote_v("MGBench cuda | q1t | sort 2 | L7 | spai_grote | V", 3 , 1, 7, "spai_grote", 1.);
MGBench<tags::GPU::CUDA, methods::CYCLE::V::STATIC, SparseMatrixELL<double> > cuda_q1t_sort2_l8_spai_grote_v("MGBench cuda | q1t | sort 2 | L8 | spai_grote | V", 3 , 1, 8, "spai_grote", 1.);

MGBench<tags::GPU::CUDA, methods::CYCLE::V::STATIC, SparseMatrixELL<double> > cuda_q1t_sort3_l6_spai_grote_v("MGBench cuda | q1t | sort 3 | L6 | spai_grote | V", 3 , 3, 6, "spai_grote", 1.);
MGBench<tags::GPU::CUDA, methods::CYCLE::V::STATIC, SparseMatrixELL<double> > cuda_q1t_sort3_l7_spai_grote_v("MGBench cuda | q1t | sort 3 | L7 | spai_grote | V", 3 , 3, 7, "spai_grote", 1.);
MGBench<tags::GPU::CUDA, methods::CYCLE::V::STATIC, SparseMatrixELL<double> > cuda_q1t_sort3_l8_spai_grote_v("MGBench cuda | q1t | sort 3 | L8 | spai_grote | V", 3 , 3, 8, "spai_grote", 1.);


MGBench<tags::GPU::CUDA, methods::CYCLE::V::STATIC, SparseMatrixELL<double> > cuda_q1t_sort4_l6_spai_grote_v("MGBench cuda | q1t | sort 4 | L6 | spai_grote | V", 3 , 4, 6, "spai_grote", 1.);
MGBench<tags::GPU::CUDA, methods::CYCLE::V::STATIC, SparseMatrixELL<double> > cuda_q1t_sort4_l7_spai_grote_v("MGBench cuda | q1t | sort 4 | L7 | spai_grote | V", 3 , 4, 7, "spai_grote", 1.);
MGBench<tags::GPU::CUDA, methods::CYCLE::V::STATIC, SparseMatrixELL<double> > cuda_q1t_sort4_l8_spai_grote_v("MGBench cuda | q1t | sort 4 | L8 | spai_grote | V", 3 , 4, 8, "spai_grote", 1.);
//SAINV
//q1
MGBench<tags::GPU::CUDA, methods::CYCLE::V::STATIC, SparseMatrixELL<double> > cuda_q1_sort0_l7_sainv_v("MGBench cuda | q1 | sort 0 | L7 | sainv | V", 1 , 0, 7, "sainv", 0.7);
MGBench<tags::GPU::CUDA, methods::CYCLE::V::STATIC, SparseMatrixELL<double> > cuda_q1_sort0_l8_sainv_v("MGBench cuda | q1 | sort 0 | L8 | sainv | V", 1 , 0, 8, "sainv", 0.7);
MGBench<tags::GPU::CUDA, methods::CYCLE::V::STATIC, SparseMatrixELL<double> > cuda_q1_sort0_l9_sainv_v("MGBench cuda | q1 | sort 0 | L9 | sainv | V", 1 , 0, 9, "sainv", 0.7);


MGBench<tags::GPU::CUDA, methods::CYCLE::V::STATIC, SparseMatrixELL<double> > cuda_q1_sort1_l7_sainv_v("MGBench cuda | q1 | sort 1 | L7 | sainv | V", 1 , 1, 7, "sainv", 0.7);
MGBench<tags::GPU::CUDA, methods::CYCLE::V::STATIC, SparseMatrixELL<double> > cuda_q1_sort1_l8_sainv_v("MGBench cuda | q1 | sort 1 | L8 | sainv | V", 1 , 1, 8, "sainv", 0.7);
MGBench<tags::GPU::CUDA, methods::CYCLE::V::STATIC, SparseMatrixELL<double> > cuda_q1_sort1_l9_sainv_v("MGBench cuda | q1 | sort 1 | L9 | sainv | V", 1 , 1, 9, "sainv", 0.7);


MGBench<tags::GPU::CUDA, methods::CYCLE::V::STATIC, SparseMatrixELL<double> > cuda_q1_sort2_l7_sainv_v("MGBench cuda | q1 | sort 2 | L7 | sainv | V", 1 , 2, 7, "sainv", 0.7);
MGBench<tags::GPU::CUDA, methods::CYCLE::V::STATIC, SparseMatrixELL<double> > cuda_q1_sort2_l8_sainv_v("MGBench cuda | q1 | sort 2 | L8 | sainv | V", 1 , 2, 8, "sainv", 0.7);
MGBench<tags::GPU::CUDA, methods::CYCLE::V::STATIC, SparseMatrixELL<double> > cuda_q1_sort2_l9_sainv_v("MGBench cuda | q1 | sort 2 | L9 | sainv | V", 1 , 2, 9, "sainv", 0.7);

MGBench<tags::GPU::CUDA, methods::CYCLE::V::STATIC, SparseMatrixELL<double> > cuda_q1_sort3_l7_sainv_v("MGBench cuda | q1 | sort 3 | L7 | sainv | V", 1 , 3, 7, "sainv", 0.7);
MGBench<tags::GPU::CUDA, methods::CYCLE::V::STATIC, SparseMatrixELL<double> > cuda_q1_sort3_l8_sainv_v("MGBench cuda | q1 | sort 3 | L8 | sainv | V", 1 , 3, 8, "sainv", 0.7);
MGBench<tags::GPU::CUDA, methods::CYCLE::V::STATIC, SparseMatrixELL<double> > cuda_q1_sort3_l9_sainv_v("MGBench cuda | q1 | sort 3 | L9 | sainv | V", 1 , 3, 9, "sainv", 0.7);

MGBench<tags::GPU::CUDA, methods::CYCLE::V::STATIC, SparseMatrixELL<double> > cuda_q1_sort4_l7_sainv_v("MGBench cuda | q1 | sort 4 | L7 | sainv | V", 1 , 4, 7, "sainv", 0.7);
MGBench<tags::GPU::CUDA, methods::CYCLE::V::STATIC, SparseMatrixELL<double> > cuda_q1_sort4_l8_sainv_v("MGBench cuda | q1 | sort 4 | L8 | sainv | V", 1 , 4, 8, "sainv", 0.7);
MGBench<tags::GPU::CUDA, methods::CYCLE::V::STATIC, SparseMatrixELL<double> > cuda_q1_sort4_l9_sainv_v("MGBench cuda | q1 | sort 4 | L9 | sainv | V", 1 , 4, 9, "sainv", 0.7);

//q2
MGBench<tags::GPU::CUDA, methods::CYCLE::V::STATIC, SparseMatrixELL<double> > cuda_q2_sort0_l6_sainv_v("MGBench cuda | q2 | sort 0 | L6 | sainv | V", 2 , 0, 6, "sainv", 0.7);
MGBench<tags::GPU::CUDA, methods::CYCLE::V::STATIC, SparseMatrixELL<double> > cuda_q2_sort0_l7_sainv_v("MGBench cuda | q2 | sort 0 | L7 | sainv | V", 2 , 0, 7, "sainv", 0.7);
MGBench<tags::GPU::CUDA, methods::CYCLE::V::STATIC, SparseMatrixELL<double> > cuda_q2_sort0_l8_sainv_v("MGBench cuda | q2 | sort 0 | L8 | sainv | V", 2 , 0, 8, "sainv", 0.7);


MGBench<tags::GPU::CUDA, methods::CYCLE::V::STATIC, SparseMatrixELL<double> > cuda_q2_sort1_l6_sainv_v("MGBench cuda | q2 | sort 1 | L6 | sainv | V", 2 , 1, 6, "sainv", 0.7);
MGBench<tags::GPU::CUDA, methods::CYCLE::V::STATIC, SparseMatrixELL<double> > cuda_q2_sort1_l7_sainv_v("MGBench cuda | q2 | sort 1 | L7 | sainv | V", 2 , 1, 7, "sainv", 0.7);
MGBench<tags::GPU::CUDA, methods::CYCLE::V::STATIC, SparseMatrixELL<double> > cuda_q2_sort1_l8_sainv_v("MGBench cuda | q2 | sort 1 | L8 | sainv | V", 2 , 1, 8, "sainv", 0.7);


MGBench<tags::GPU::CUDA, methods::CYCLE::V::STATIC, SparseMatrixELL<double> > cuda_q2_sort2_l6_sainv_v("MGBench cuda | q2 | sort 2 | L6 | sainv | V", 2 , 2, 6, "sainv", 0.7);
MGBench<tags::GPU::CUDA, methods::CYCLE::V::STATIC, SparseMatrixELL<double> > cuda_q2_sort2_l7_sainv_v("MGBench cuda | q2 | sort 2 | L7 | sainv | V", 2 , 2, 7, "sainv", 0.7);
MGBench<tags::GPU::CUDA, methods::CYCLE::V::STATIC, SparseMatrixELL<double> > cuda_q2_sort2_l8_sainv_v("MGBench cuda | q2 | sort 2 | L8 | sainv | V", 2 , 2, 8, "sainv", 0.7);

MGBench<tags::GPU::CUDA, methods::CYCLE::V::STATIC, SparseMatrixELL<double> > cuda_q2_sort3_l6_sainv_v("MGBench cuda | q2 | sort 3 | L6 | sainv | V", 2 , 3, 6, "sainv", 0.7);
MGBench<tags::GPU::CUDA, methods::CYCLE::V::STATIC, SparseMatrixELL<double> > cuda_q2_sort3_l7_sainv_v("MGBench cuda | q2 | sort 3 | L7 | sainv | V", 2 , 3, 7, "sainv", 0.7);
MGBench<tags::GPU::CUDA, methods::CYCLE::V::STATIC, SparseMatrixELL<double> > cuda_q2_sort3_l8_sainv_v("MGBench cuda | q2 | sort 3 | L8 | sainv | V", 2 , 3, 8, "sainv", 0.7);


MGBench<tags::GPU::CUDA, methods::CYCLE::V::STATIC, SparseMatrixELL<double> > cuda_q2_sort4_l6_sainv_v("MGBench cuda | q2 | sort 4 | L6 | sainv | V", 2 , 4, 6, "sainv", 0.7);
MGBench<tags::GPU::CUDA, methods::CYCLE::V::STATIC, SparseMatrixELL<double> > cuda_q2_sort4_l7_sainv_v("MGBench cuda | q2 | sort 4 | L7 | sainv | V", 2 , 4, 7, "sainv", 0.7);
MGBench<tags::GPU::CUDA, methods::CYCLE::V::STATIC, SparseMatrixELL<double> > cuda_q2_sort4_l8_sainv_v("MGBench cuda | q2 | sort 4 | L8 | sainv | V", 2 , 4, 8, "sainv", 0.7);

//q1t
MGBench<tags::GPU::CUDA, methods::CYCLE::V::STATIC, SparseMatrixELL<double> > cuda_q1t_sort0_l6_sainv_v("MGBench cuda | q1t | sort 0 | L6 | sainv | V", 3 , 0, 6, "sainv", 0.7);
MGBench<tags::GPU::CUDA, methods::CYCLE::V::STATIC, SparseMatrixELL<double> > cuda_q1t_sort0_l7_sainv_v("MGBench cuda | q1t | sort 0 | L7 | sainv | V", 3 , 0, 7, "sainv", 0.7);
MGBench<tags::GPU::CUDA, methods::CYCLE::V::STATIC, SparseMatrixELL<double> > cuda_q1t_sort0_l8_sainv_v("MGBench cuda | q1t | sort 0 | L8 | sainv | V", 3 , 0, 8, "sainv", 0.7);


MGBench<tags::GPU::CUDA, methods::CYCLE::V::STATIC, SparseMatrixELL<double> > cuda_q1t_sort1_l6_sainv_v("MGBench cuda | q1t | sort 1 | L6 | sainv | V", 3 , 1, 6, "sainv", 0.7);
MGBench<tags::GPU::CUDA, methods::CYCLE::V::STATIC, SparseMatrixELL<double> > cuda_q1t_sort1_l7_sainv_v("MGBench cuda | q1t | sort 1 | L7 | sainv | V", 3 , 1, 7, "sainv", 0.7);
MGBench<tags::GPU::CUDA, methods::CYCLE::V::STATIC, SparseMatrixELL<double> > cuda_q1t_sort1_l8_sainv_v("MGBench cuda | q1t | sort 1 | L8 | sainv | V", 3 , 1, 8, "sainv", 0.7);


MGBench<tags::GPU::CUDA, methods::CYCLE::V::STATIC, SparseMatrixELL<double> > cuda_q1t_sort2_l6_sainv_v("MGBench cuda | q1t | sort 2 | L6 | sainv | V", 3 , 2, 6, "sainv", 0.5);
MGBench<tags::GPU::CUDA, methods::CYCLE::V::STATIC, SparseMatrixELL<double> > cuda_q1t_sort2_l7_sainv_v("MGBench cuda | q1t | sort 2 | L7 | sainv | V", 3 , 2, 7, "sainv", 0.5);
MGBench<tags::GPU::CUDA, methods::CYCLE::V::STATIC, SparseMatrixELL<double> > cuda_q1t_sort2_l8_sainv_v("MGBench cuda | q1t | sort 2 | L8 | sainv | V", 3 , 2, 8, "sainv", 0.5);

MGBench<tags::GPU::CUDA, methods::CYCLE::V::STATIC, SparseMatrixELL<double> > cuda_q1t_sort3_l6_sainv_v("MGBench cuda | q1t | sort 3 | L6 | sainv | V", 3 , 3, 6, "sainv", 0.7);
MGBench<tags::GPU::CUDA, methods::CYCLE::V::STATIC, SparseMatrixELL<double> > cuda_q1t_sort3_l7_sainv_v("MGBench cuda | q1t | sort 3 | L7 | sainv | V", 3 , 3, 7, "sainv", 0.7);
MGBench<tags::GPU::CUDA, methods::CYCLE::V::STATIC, SparseMatrixELL<double> > cuda_q1t_sort3_l8_sainv_v("MGBench cuda | q1t | sort 3 | L8 | sainv | V", 3 , 3, 8, "sainv", 0.7);


MGBench<tags::GPU::CUDA, methods::CYCLE::V::STATIC, SparseMatrixELL<double> > cuda_q1t_sort4_l6_sainv_v("MGBench cuda | q1t | sort 4 | L6 | sainv | V", 3 , 4, 6, "sainv", 0.7);
MGBench<tags::GPU::CUDA, methods::CYCLE::V::STATIC, SparseMatrixELL<double> > cuda_q1t_sort4_l7_sainv_v("MGBench cuda | q1t | sort 4 | L7 | sainv | V", 3 , 4, 7, "sainv", 0.7);
MGBench<tags::GPU::CUDA, methods::CYCLE::V::STATIC, SparseMatrixELL<double> > cuda_q1t_sort4_l8_sainv_v("MGBench cuda | q1t | sort 4 | L8 | sainv | V", 3 , 4, 8, "sainv", 0.7);
#endif

//W-cycle
#ifdef HONEI_SSE
//JAC
//q1
MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::W::STATIC, DenseVector<double> > mcsse_q1_sort0_l7_jac_w("MGBench mcsse | q1 | sort 0 | L7 | jac | W", 1 , 0, 7, "jac", 0.7);
MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::W::STATIC, DenseVector<double> > mcsse_q1_sort0_l8_jac_w("MGBench mcsse | q1 | sort 0 | L8 | jac | W", 1 , 0, 8, "jac", 0.7);
MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::W::STATIC, DenseVector<double> > mcsse_q1_sort0_l9_jac_w("MGBench mcsse | q1 | sort 0 | L9 | jac | W", 1 , 0, 9, "jac", 0.7);


MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::W::STATIC, DenseVector<double> > mcsse_q1_sort1_l7_jac_w("MGBench mcsse | q1 | sort 1 | L7 | jac | W", 1 , 1, 7, "jac", 0.7);
MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::W::STATIC, DenseVector<double> > mcsse_q1_sort1_l8_jac_w("MGBench mcsse | q1 | sort 1 | L8 | jac | W", 1 , 1, 8, "jac", 0.7);
MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::W::STATIC, DenseVector<double> > mcsse_q1_sort1_l9_jac_w("MGBench mcsse | q1 | sort 1 | L9 | jac | W", 1 , 1, 9, "jac", 0.7);


MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::W::STATIC, DenseVector<double> > mcsse_q1_sort2_l7_jac_w("MGBench mcsse | q1 | sort 2 | L7 | jac | W", 1 , 2, 7, "jac", 0.7);
MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::W::STATIC, DenseVector<double> > mcsse_q1_sort2_l8_jac_w("MGBench mcsse | q1 | sort 2 | L8 | jac | W", 1 , 2, 8, "jac", 0.7);
MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::W::STATIC, DenseVector<double> > mcsse_q1_sort2_l9_jac_w("MGBench mcsse | q1 | sort 2 | L9 | jac | W", 1 , 2, 9, "jac", 0.7);

MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::W::STATIC, DenseVector<double> > mcsse_q1_sort3_l7_jac_w("MGBench mcsse | q1 | sort 3 | L7 | jac | W", 1 , 3, 7, "jac", 0.7);
MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::W::STATIC, DenseVector<double> > mcsse_q1_sort3_l8_jac_w("MGBench mcsse | q1 | sort 3 | L8 | jac | W", 1 , 3, 8, "jac", 0.7);
MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::W::STATIC, DenseVector<double> > mcsse_q1_sort3_l9_jac_w("MGBench mcsse | q1 | sort 3 | L9 | jac | W", 1 , 3, 9, "jac", 0.7);


MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::W::STATIC, DenseVector<double> > mcsse_q1_sort4_l7_jac_w("MGBench mcsse | q1 | sort 4 | L7 | jac | W", 1 , 4, 7, "jac", 0.7);
MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::W::STATIC, DenseVector<double> > mcsse_q1_sort4_l8_jac_w("MGBench mcsse | q1 | sort 4 | L8 | jac | W", 1 , 4, 8, "jac", 0.7);
MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::W::STATIC, DenseVector<double> > mcsse_q1_sort4_l9_jac_w("MGBench mcsse | q1 | sort 4 | L9 | jac | W", 1 , 4, 9, "jac", 0.7);

//q2
MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::W::STATIC, DenseVector<double> > mcsse_q2_sort0_l6_jac_w("MGBench mcsse | q2 | sort 0 | L6 | jac | W", 2 , 0, 6, "jac", 0.7);
MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::W::STATIC, DenseVector<double> > mcsse_q2_sort0_l7_jac_w("MGBench mcsse | q2 | sort 0 | L7 | jac | W", 2 , 0, 7, "jac", 0.7);
MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::W::STATIC, DenseVector<double> > mcsse_q2_sort0_l8_jac_w("MGBench mcsse | q2 | sort 0 | L8 | jac | W", 2 , 0, 8, "jac", 0.7);


MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::W::STATIC, DenseVector<double> > mcsse_q2_sort1_l6_jac_w("MGBench mcsse | q2 | sort 1 | L6 | jac | W", 2 , 1, 6, "jac", 0.7);
MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::W::STATIC, DenseVector<double> > mcsse_q2_sort1_l7_jac_w("MGBench mcsse | q2 | sort 1 | L7 | jac | W", 2 , 1, 7, "jac", 0.7);
MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::W::STATIC, DenseVector<double> > mcsse_q2_sort1_l8_jac_w("MGBench mcsse | q2 | sort 1 | L8 | jac | W", 2 , 1, 8, "jac", 0.7);


MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::W::STATIC, DenseVector<double> > mcsse_q2_sort2_l6_jac_w("MGBench mcsse | q2 | sort 2 | L6 | jac | W", 2 , 2, 6, "jac", 0.7);
MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::W::STATIC, DenseVector<double> > mcsse_q2_sort2_l7_jac_w("MGBench mcsse | q2 | sort 2 | L7 | jac | W", 2 , 2, 7, "jac", 0.7);
MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::W::STATIC, DenseVector<double> > mcsse_q2_sort2_l8_jac_w("MGBench mcsse | q2 | sort 2 | L8 | jac | W", 2 , 2, 8, "jac", 0.7);

MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::W::STATIC, DenseVector<double> > mcsse_q2_sort3_l6_jac_w("MGBench mcsse | q2 | sort 3 | L6 | jac | W", 2 , 3, 6, "jac", 0.7);
MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::W::STATIC, DenseVector<double> > mcsse_q2_sort3_l7_jac_w("MGBench mcsse | q2 | sort 3 | L7 | jac | W", 2 , 3, 7, "jac", 0.7);
MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::W::STATIC, DenseVector<double> > mcsse_q2_sort3_l8_jac_w("MGBench mcsse | q2 | sort 3 | L8 | jac | W", 2 , 3, 8, "jac", 0.7);

MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::W::STATIC, DenseVector<double> > mcsse_q2_sort4_l6_jac_w("MGBench mcsse | q2 | sort 4 | L6 | jac | W", 2 , 4, 6, "jac", 0.7);
MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::W::STATIC, DenseVector<double> > mcsse_q2_sort4_l7_jac_w("MGBench mcsse | q2 | sort 4 | L7 | jac | W", 2 , 4, 7, "jac", 0.7);
MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::W::STATIC, DenseVector<double> > mcsse_q2_sort4_l8_jac_w("MGBench mcsse | q2 | sort 4 | L8 | jac | W", 2 , 4, 8, "jac", 0.7);

//q1t
MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::W::STATIC, DenseVector<double> > mcsse_q1t_sort0_l6_jac_w("MGBench mcsse | q1t | sort 0 | L6 | jac | W", 3, 0, 6, "jac", 0.7);
MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::W::STATIC, DenseVector<double> > mcsse_q1t_sort0_l7_jac_w("MGBench mcsse | q1t | sort 0 | L7 | jac | W", 3 , 0, 7, "jac", 0.7);
MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::W::STATIC, DenseVector<double> > mcsse_q1t_sort0_l8_jac_w("MGBench mcsse | q1t | sort 0 | L8 | jac | W", 3 , 0, 8, "jac", 0.7);


MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::W::STATIC, DenseVector<double> > mcsse_q1t_sort1_l6_jac_w("MGBench mcsse | q1t | sort 1 | L6 | jac | W", 3 , 1, 6, "jac", 0.7);
MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::W::STATIC, DenseVector<double> > mcsse_q1t_sort1_l7_jac_w("MGBench mcsse | q1t | sort 1 | L7 | jac | W", 3 , 1, 7, "jac", 0.7);
MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::W::STATIC, DenseVector<double> > mcsse_q1t_sort1_l8_jac_w("MGBench mcsse | q1t | sort 1 | L8 | jac | W", 3 , 1, 8, "jac", 0.7);


MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::W::STATIC, DenseVector<double> > mcsse_q1t_sort2_l6_jac_w("MGBench mcsse | q1t | sort 2 | L6 | jac | W", 3 , 2, 6, "jac", 0.7);
MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::W::STATIC, DenseVector<double> > mcsse_q1t_sort2_l7_jac_w("MGBench mcsse | q1t | sort 2 | L7 | jac | W", 3 , 2, 7, "jac", 0.7);
MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::W::STATIC, DenseVector<double> > mcsse_q1t_sort2_l8_jac_w("MGBench mcsse | q1t | sort 2 | L8 | jac | W", 3 , 2, 8, "jac", 0.7);

MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::W::STATIC, DenseVector<double> > mcsse_q1t_sort3_l6_jac_w("MGBench mcsse | q1t | sort 3 | L6 | jac | W", 3 , 3, 6, "jac", 0.7);
MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::W::STATIC, DenseVector<double> > mcsse_q1t_sort3_l7_jac_w("MGBench mcsse | q1t | sort 3 | L7 | jac | W", 3 , 3, 7, "jac", 0.7);
MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::W::STATIC, DenseVector<double> > mcsse_q1t_sort3_l8_jac_w("MGBench mcsse | q1t | sort 3 | L8 | jac | W", 3 , 3, 8, "jac", 0.7);

MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::W::STATIC, DenseVector<double> > mcsse_q1t_sort4_l6_jac_w("MGBench mcsse | q1t | sort 4 | L6 | jac | W", 3 , 4, 6, "jac", 0.7);
MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::W::STATIC, DenseVector<double> > mcsse_q1t_sort4_l7_jac_w("MGBench mcsse | q1t | sort 4 | L7 | jac | W", 3 , 4, 7, "jac", 0.7);
MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::W::STATIC, DenseVector<double> > mcsse_q1t_sort4_l8_jac_w("MGBench mcsse | q1t | sort 4 | L8 | jac | W", 3 , 4, 8, "jac", 0.7);

//SPAI
//q1
MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::W::STATIC, SparseMatrixELL<double> > mcsse_q1_sort0_l7_spai_grote_w("MGBench mcsse | q1 | sort 0 | L7 | spai_grote | W", 1 , 0, 7, "spai_grote", 1.);
MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::W::STATIC, SparseMatrixELL<double> > mcsse_q1_sort0_l8_spai_grote_w("MGBench mcsse | q1 | sort 0 | L8 | spai_grote | W", 1 , 0, 8, "spai_grote", 1.);
MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::W::STATIC, SparseMatrixELL<double> > mcsse_q1_sort0_l9_spai_grote_w("MGBench mcsse | q1 | sort 0 | L9 | spai_grote | W", 1 , 0, 9, "spai_grote", 1.);


MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::W::STATIC, SparseMatrixELL<double> > mcsse_q1_sort1_l7_spai_grote_w("MGBench mcsse | q1 | sort 1 | L7 | spai_grote | W", 1 , 1, 7, "spai_grote", 1.);
MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::W::STATIC, SparseMatrixELL<double> > mcsse_q1_sort1_l8_spai_grote_w("MGBench mcsse | q1 | sort 1 | L8 | spai_grote | W", 1 , 1, 8, "spai_grote", 1.);
MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::W::STATIC, SparseMatrixELL<double> > mcsse_q1_sort1_l9_spai_grote_w("MGBench mcsse | q1 | sort 1 | L9 | spai_grote | W", 1 , 1, 9, "spai_grote", 1.);


MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::W::STATIC, SparseMatrixELL<double> > mcsse_q1_sort2_l7_spai_grote_w("MGBench mcsse | q1 | sort 2 | L7 | spai_grote | W", 1 , 2, 7, "spai_grote", 1.);
MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::W::STATIC, SparseMatrixELL<double> > mcsse_q1_sort2_l8_spai_grote_w("MGBench mcsse | q1 | sort 2 | L8 | spai_grote | W", 1 , 2, 8, "spai_grote", 1.);
MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::W::STATIC, SparseMatrixELL<double> > mcsse_q1_sort2_l9_spai_grote_w("MGBench mcsse | q1 | sort 2 | L9 | spai_grote | W", 1 , 2, 9, "spai_grote", 1.);

MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::W::STATIC, SparseMatrixELL<double> > mcsse_q1_sort3_l7_spai_grote_w("MGBench mcsse | q1 | sort 3 | L7 | spai_grote | W", 1 , 3, 7, "spai_grote", 1.);
MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::W::STATIC, SparseMatrixELL<double> > mcsse_q1_sort3_l8_spai_grote_w("MGBench mcsse | q1 | sort 3 | L8 | spai_grote | W", 1 , 3, 8, "spai_grote", 1.);
MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::W::STATIC, SparseMatrixELL<double> > mcsse_q1_sort3_l9_spai_grote_w("MGBench mcsse | q1 | sort 3 | L9 | spai_grote | W", 1 , 3, 9, "spai_grote", 1.);

MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::W::STATIC, SparseMatrixELL<double> > mcsse_q1_sort4_l7_spai_grote_w("MGBench mcsse | q1 | sort 4 | L7 | spai_grote | W", 1 , 4, 7, "spai_grote", 1.);
MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::W::STATIC, SparseMatrixELL<double> > mcsse_q1_sort4_l8_spai_grote_w("MGBench mcsse | q1 | sort 4 | L8 | spai_grote | W", 1 , 4, 8, "spai_grote", 1.);
MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::W::STATIC, SparseMatrixELL<double> > mcsse_q1_sort4_l9_spai_grote_w("MGBench mcsse | q1 | sort 4 | L9 | spai_grote | W", 1 , 4, 9, "spai_grote", 1.);

//q2
MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::W::STATIC, SparseMatrixELL<double> > mcsse_q2_sort0_l6_spai_grote_w("MGBench mcsse | q2 | sort 0 | L6 | spai_grote | W", 2 , 0, 6, "spai_grote", 1.);
MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::W::STATIC, SparseMatrixELL<double> > mcsse_q2_sort0_l7_spai_grote_w("MGBench mcsse | q2 | sort 0 | L7 | spai_grote | W", 2 , 0, 7, "spai_grote", 1.);
MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::W::STATIC, SparseMatrixELL<double> > mcsse_q2_sort0_l8_spai_grote_w("MGBench mcsse | q2 | sort 0 | L8 | spai_grote | W", 2 , 0, 8, "spai_grote", 1.);


MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::W::STATIC, SparseMatrixELL<double> > mcsse_q2_sort1_l6_spai_grote_w("MGBench mcsse | q2 | sort 1 | L6 | spai_grote | W", 2 , 1, 6, "spai_grote", 1.);
MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::W::STATIC, SparseMatrixELL<double> > mcsse_q2_sort1_l7_spai_grote_w("MGBench mcsse | q2 | sort 1 | L7 | spai_grote | W", 2 , 1, 7, "spai_grote", 1.);
MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::W::STATIC, SparseMatrixELL<double> > mcsse_q2_sort1_l8_spai_grote_w("MGBench mcsse | q2 | sort 1 | L8 | spai_grote | W", 2 , 1, 8, "spai_grote", 1.);


MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::W::STATIC, SparseMatrixELL<double> > mcsse_q2_sort2_l6_spai_grote_w("MGBench mcsse | q2 | sort 2 | L6 | spai_grote | W", 2 , 2, 6, "spai_grote", 1.);
MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::W::STATIC, SparseMatrixELL<double> > mcsse_q2_sort2_l7_spai_grote_w("MGBench mcsse | q2 | sort 2 | L7 | spai_grote | W", 2 , 2, 7, "spai_grote", 1.);
MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::W::STATIC, SparseMatrixELL<double> > mcsse_q2_sort2_l8_spai_grote_w("MGBench mcsse | q2 | sort 2 | L8 | spai_grote | W", 2 , 2, 8, "spai_grote", 1.);

MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::W::STATIC, SparseMatrixELL<double> > mcsse_q2_sort3_l6_spai_grote_w("MGBench mcsse | q2 | sort 3 | L6 | spai_grote | W", 2 , 3, 6, "spai_grote", 1.);
MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::W::STATIC, SparseMatrixELL<double> > mcsse_q2_sort3_l7_spai_grote_w("MGBench mcsse | q2 | sort 3 | L7 | spai_grote | W", 2 , 3, 7, "spai_grote", 1.);
MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::W::STATIC, SparseMatrixELL<double> > mcsse_q2_sort3_l8_spai_grote_w("MGBench mcsse | q2 | sort 3 | L8 | spai_grote | W", 2 , 3, 8, "spai_grote", 1.);

MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::W::STATIC, SparseMatrixELL<double> > mcsse_q2_sort4_l6_spai_grote_w("MGBench mcsse | q2 | sort 4 | L6 | spai_grote | W", 2 , 4, 6, "spai_grote", 1.);
MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::W::STATIC, SparseMatrixELL<double> > mcsse_q2_sort4_l7_spai_grote_w("MGBench mcsse | q2 | sort 4 | L7 | spai_grote | W", 2 , 4, 7, "spai_grote", 1.);
MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::W::STATIC, SparseMatrixELL<double> > mcsse_q2_sort4_l8_spai_grote_w("MGBench mcsse | q2 | sort 4 | L8 | spai_grote | W", 2 , 4, 8, "spai_grote", 1.);

//q1t
MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::W::STATIC, SparseMatrixELL<double> > mcsse_q1t_sort0_l6_spai_grote_w("MGBench mcsse | q1t | sort 0 | L6 | spai_grote | W", 3, 0, 6, "spai_grote", 1.);
MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::W::STATIC, SparseMatrixELL<double> > mcsse_q1t_sort0_l7_spai_grote_w("MGBench mcsse | q1t | sort 0 | L7 | spai_grote | W", 3 , 0, 7, "spai_grote", 1.);
MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::W::STATIC, SparseMatrixELL<double> > mcsse_q1t_sort0_l8_spai_grote_w("MGBench mcsse | q1t | sort 0 | L8 | spai_grote | W", 3 , 0, 8, "spai_grote", 1.);


MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::W::STATIC, SparseMatrixELL<double> > mcsse_q1t_sort1_l6_spai_grote_w("MGBench mcsse | q1t | sort 1 | L6 | spai_grote | W", 3 , 1, 6, "spai_grote", 1.);
MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::W::STATIC, SparseMatrixELL<double> > mcsse_q1t_sort1_l7_spai_grote_w("MGBench mcsse | q1t | sort 1 | L7 | spai_grote | W", 3 , 1, 7, "spai_grote", 1.);
MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::W::STATIC, SparseMatrixELL<double> > mcsse_q1t_sort1_l8_spai_grote_w("MGBench mcsse | q1t | sort 1 | L8 | spai_grote | W", 3 , 1, 8, "spai_grote", 1.);


MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::W::STATIC, SparseMatrixELL<double> > mcsse_q1t_sort2_l6_spai_grote_w("MGBench mcsse | q1t | sort 2 | L6 | spai_grote | W", 3 , 2, 6, "spai_grote", 1.);
MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::W::STATIC, SparseMatrixELL<double> > mcsse_q1t_sort2_l7_spai_grote_w("MGBench mcsse | q1t | sort 2 | L7 | spai_grote | W", 3 , 2, 7, "spai_grote", 1.);
MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::W::STATIC, SparseMatrixELL<double> > mcsse_q1t_sort2_l8_spai_grote_w("MGBench mcsse | q1t | sort 2 | L8 | spai_grote | W", 3 , 2, 8, "spai_grote", 1.);

MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::W::STATIC, SparseMatrixELL<double> > mcsse_q1t_sort3_l6_spai_grote_w("MGBench mcsse | q1t | sort 3 | L6 | spai_grote | W", 3 , 3, 6, "spai_grote", 1.);
MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::W::STATIC, SparseMatrixELL<double> > mcsse_q1t_sort3_l7_spai_grote_w("MGBench mcsse | q1t | sort 3 | L7 | spai_grote | W", 3 , 3, 7, "spai_grote", 1.);
MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::W::STATIC, SparseMatrixELL<double> > mcsse_q1t_sort3_l8_spai_grote_w("MGBench mcsse | q1t | sort 3 | L8 | spai_grote | W", 3 , 3, 8, "spai_grote", 1.);

MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::W::STATIC, SparseMatrixELL<double> > mcsse_q1t_sort4_l6_spai_grote_w("MGBench mcsse | q1t | sort 4 | L6 | spai_grote | W", 3 , 4, 6, "spai_grote", 1.);
MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::W::STATIC, SparseMatrixELL<double> > mcsse_q1t_sort4_l7_spai_grote_w("MGBench mcsse | q1t | sort 4 | L7 | spai_grote | W", 3 , 4, 7, "spai_grote", 1.);
MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::W::STATIC, SparseMatrixELL<double> > mcsse_q1t_sort4_l8_spai_grote_w("MGBench mcsse | q1t | sort 4 | L8 | spai_grote | W", 3 , 4, 8, "spai_grote", 1.);

//SAINV
//q1
MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::W::STATIC, SparseMatrixELL<double> > mcsse_q1_sort0_l7_sainv_w("MGBench mcsse | q1 | sort 0 | L7 | sainv | W", 1 , 0, 7, "sainv", 0.7);
MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::W::STATIC, SparseMatrixELL<double> > mcsse_q1_sort0_l8_sainv_w("MGBench mcsse | q1 | sort 0 | L8 | sainv | W", 1 , 0, 8, "sainv", 0.7);
MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::W::STATIC, SparseMatrixELL<double> > mcsse_q1_sort0_l9_sainv_w("MGBench mcsse | q1 | sort 0 | L9 | sainv | W", 1 , 0, 9, "sainv", 0.7);


MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::W::STATIC, SparseMatrixELL<double> > mcsse_q1_sort1_l7_sainv_w("MGBench mcsse | q1 | sort 1 | L7 | sainv | W", 1 , 1, 7, "sainv", 0.7);
MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::W::STATIC, SparseMatrixELL<double> > mcsse_q1_sort1_l8_sainv_w("MGBench mcsse | q1 | sort 1 | L8 | sainv | W", 1 , 1, 8, "sainv", 0.7);
MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::W::STATIC, SparseMatrixELL<double> > mcsse_q1_sort1_l9_sainv_w("MGBench mcsse | q1 | sort 1 | L9 | sainv | W", 1 , 1, 9, "sainv", 0.7);


MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::W::STATIC, SparseMatrixELL<double> > mcsse_q1_sort2_l7_sainv_w("MGBench mcsse | q1 | sort 2 | L7 | sainv | W", 1 , 2, 7, "sainv", 0.7);
MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::W::STATIC, SparseMatrixELL<double> > mcsse_q1_sort2_l8_sainv_w("MGBench mcsse | q1 | sort 2 | L8 | sainv | W", 1 , 2, 8, "sainv", 0.7);
MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::W::STATIC, SparseMatrixELL<double> > mcsse_q1_sort2_l9_sainv_w("MGBench mcsse | q1 | sort 2 | L9 | sainv | W", 1 , 2, 9, "sainv", 0.7);

MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::W::STATIC, SparseMatrixELL<double> > mcsse_q1_sort3_l7_sainv_w("MGBench mcsse | q1 | sort 3 | L7 | sainv | W", 1 , 3, 7, "sainv", 0.7);
MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::W::STATIC, SparseMatrixELL<double> > mcsse_q1_sort3_l8_sainv_w("MGBench mcsse | q1 | sort 3 | L8 | sainv | W", 1 , 3, 8, "sainv", 0.7);
MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::W::STATIC, SparseMatrixELL<double> > mcsse_q1_sort3_l9_sainv_w("MGBench mcsse | q1 | sort 3 | L9 | sainv | W", 1 , 3, 9, "sainv", 0.7);

MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::W::STATIC, SparseMatrixELL<double> > mcsse_q1_sort4_l7_sainv_w("MGBench mcsse | q1 | sort 4 | L7 | sainv | W", 1 , 4, 7, "sainv", 0.7);
MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::W::STATIC, SparseMatrixELL<double> > mcsse_q1_sort4_l8_sainv_w("MGBench mcsse | q1 | sort 4 | L8 | sainv | W", 1 , 4, 8, "sainv", 0.7);
MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::W::STATIC, SparseMatrixELL<double> > mcsse_q1_sort4_l9_sainv_w("MGBench mcsse | q1 | sort 4 | L9 | sainv | W", 1 , 4, 9, "sainv", 0.7);

//q2
MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::W::STATIC, SparseMatrixELL<double> > mcsse_q2_sort0_l6_sainv_w("MGBench mcsse | q2 | sort 0 | L6 | sainv | W", 2 , 0, 6, "sainv", 0.7);
MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::W::STATIC, SparseMatrixELL<double> > mcsse_q2_sort0_l7_sainv_w("MGBench mcsse | q2 | sort 0 | L7 | sainv | W", 2 , 0, 7, "sainv", 0.7);
MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::W::STATIC, SparseMatrixELL<double> > mcsse_q2_sort0_l8_sainv_w("MGBench mcsse | q2 | sort 0 | L8 | sainv | W", 2 , 0, 8, "sainv", 0.7);


MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::W::STATIC, SparseMatrixELL<double> > mcsse_q2_sort1_l6_sainv_w("MGBench mcsse | q2 | sort 1 | L6 | sainv | W", 2 , 1, 6, "sainv", 0.7);
MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::W::STATIC, SparseMatrixELL<double> > mcsse_q2_sort1_l7_sainv_w("MGBench mcsse | q2 | sort 1 | L7 | sainv | W", 2 , 1, 7, "sainv", 0.7);
MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::W::STATIC, SparseMatrixELL<double> > mcsse_q2_sort1_l8_sainv_w("MGBench mcsse | q2 | sort 1 | L8 | sainv | W", 2 , 1, 8, "sainv", 0.7);


MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::W::STATIC, SparseMatrixELL<double> > mcsse_q2_sort2_l6_sainv_w("MGBench mcsse | q2 | sort 2 | L6 | sainv | W", 2 , 2, 6, "sainv", 0.7);
MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::W::STATIC, SparseMatrixELL<double> > mcsse_q2_sort2_l7_sainv_w("MGBench mcsse | q2 | sort 2 | L7 | sainv | W", 2 , 2, 7, "sainv", 0.7);
MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::W::STATIC, SparseMatrixELL<double> > mcsse_q2_sort2_l8_sainv_w("MGBench mcsse | q2 | sort 2 | L8 | sainv | W", 2 , 2, 8, "sainv", 0.7);

MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::W::STATIC, SparseMatrixELL<double> > mcsse_q2_sort3_l6_sainv_w("MGBench mcsse | q2 | sort 3 | L6 | sainv | W", 2 , 3, 6, "sainv", 0.7);
MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::W::STATIC, SparseMatrixELL<double> > mcsse_q2_sort3_l7_sainv_w("MGBench mcsse | q2 | sort 3 | L7 | sainv | W", 2 , 3, 7, "sainv", 0.7);
MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::W::STATIC, SparseMatrixELL<double> > mcsse_q2_sort3_l8_sainv_w("MGBench mcsse | q2 | sort 3 | L8 | sainv | W", 2 , 3, 8, "sainv", 0.7);

MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::W::STATIC, SparseMatrixELL<double> > mcsse_q2_sort4_l6_sainv_w("MGBench mcsse | q2 | sort 4 | L6 | sainv | W", 2 , 4, 6, "sainv", 0.7);
MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::W::STATIC, SparseMatrixELL<double> > mcsse_q2_sort4_l7_sainv_w("MGBench mcsse | q2 | sort 4 | L7 | sainv | W", 2 , 4, 7, "sainv", 0.7);
MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::W::STATIC, SparseMatrixELL<double> > mcsse_q2_sort4_l8_sainv_w("MGBench mcsse | q2 | sort 4 | L8 | sainv | W", 2 , 4, 8, "sainv", 0.7);


//q1t
MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::W::STATIC, SparseMatrixELL<double> > mcsse_q1t_sort0_l6_sainv_w("MGBench mcsse | q1t | sort 0 | L6 | sainv | W", 3 , 0, 6, "sainv", 0.7);
MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::W::STATIC, SparseMatrixELL<double> > mcsse_q1t_sort0_l7_sainv_w("MGBench mcsse | q1t | sort 0 | L7 | sainv | W", 3 , 0, 7, "sainv", 0.7);
MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::W::STATIC, SparseMatrixELL<double> > mcsse_q1t_sort0_l8_sainv_w("MGBench mcsse | q1t | sort 0 | L8 | sainv | W", 3 , 0, 8, "sainv", 0.7);


MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::W::STATIC, SparseMatrixELL<double> > mcsse_q1t_sort1_l6_sainv_w("MGBench mcsse | q1t | sort 1 | L6 | sainv | W", 3 , 1, 6, "sainv", 0.7);
MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::W::STATIC, SparseMatrixELL<double> > mcsse_q1t_sort1_l7_sainv_w("MGBench mcsse | q1t | sort 1 | L7 | sainv | W", 3 , 1, 7, "sainv", 0.7);
MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::W::STATIC, SparseMatrixELL<double> > mcsse_q1t_sort1_l8_sainv_w("MGBench mcsse | q1t | sort 1 | L8 | sainv | W", 3 , 1, 8, "sainv", 0.7);


MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::W::STATIC, SparseMatrixELL<double> > mcsse_q1t_sort2_l6_sainv_w("MGBench mcsse | q1t | sort 2 | L6 | sainv | W", 3 , 2, 6, "sainv", 0.7);
MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::W::STATIC, SparseMatrixELL<double> > mcsse_q1t_sort2_l7_sainv_w("MGBench mcsse | q1t | sort 2 | L7 | sainv | W", 3 , 2, 7, "sainv", 0.7);
MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::W::STATIC, SparseMatrixELL<double> > mcsse_q1t_sort2_l8_sainv_w("MGBench mcsse | q1t | sort 2 | L8 | sainv | W", 3 , 2, 8, "sainv", 0.7);

MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::W::STATIC, SparseMatrixELL<double> > mcsse_q1t_sort3_l6_sainv_w("MGBench mcsse | q1t | sort 3 | L6 | sainv | W", 3 , 3, 6, "sainv", 0.7);
MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::W::STATIC, SparseMatrixELL<double> > mcsse_q1t_sort3_l7_sainv_w("MGBench mcsse | q1t | sort 3 | L7 | sainv | W", 3 , 3, 7, "sainv", 0.7);
MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::W::STATIC, SparseMatrixELL<double> > mcsse_q1t_sort3_l8_sainv_w("MGBench mcsse | q1t | sort 3 | L8 | sainv | W", 3 , 3, 8, "sainv", 0.7);

MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::W::STATIC, SparseMatrixELL<double> > mcsse_q1t_sort4_l6_sainv_w("MGBench mcsse | q1t | sort 4 | L6 | sainv | W", 3 , 4, 6, "sainv", 0.7);
MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::W::STATIC, SparseMatrixELL<double> > mcsse_q1t_sort4_l7_sainv_w("MGBench mcsse | q1t | sort 4 | L7 | sainv | W", 3 , 4, 7, "sainv", 0.7);
MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::W::STATIC, SparseMatrixELL<double> > mcsse_q1t_sort4_l8_sainv_w("MGBench mcsse | q1t | sort 4 | L8 | sainv | W", 3 , 4, 8, "sainv", 0.7);
#endif

#if defined HONEI_CUDA && defined HONEI_CUDA_DOUBLE
//JAC
//q1
MGBench<tags::GPU::CUDA, methods::CYCLE::W::STATIC, DenseVector<double> > cuda_q1_sort0_l7_jac_w("MGBench cuda | q1 | sort 0 | L7 | jac | W", 1 , 0, 7, "jac", 0.7);
MGBench<tags::GPU::CUDA, methods::CYCLE::W::STATIC, DenseVector<double> > cuda_q1_sort0_l8_jac_w("MGBench cuda | q1 | sort 0 | L8 | jac | W", 1 , 0, 8, "jac", 0.7);
MGBench<tags::GPU::CUDA, methods::CYCLE::W::STATIC, DenseVector<double> > cuda_q1_sort0_l9_jac_w("MGBench cuda | q1 | sort 0 | L9 | jac | W", 1 , 0, 9, "jac", 0.7);


MGBench<tags::GPU::CUDA, methods::CYCLE::W::STATIC, DenseVector<double> > cuda_q1_sort1_l7_jac_w("MGBench cuda | q1 | sort 1 | L7 | jac | W", 1 , 1, 7, "jac", 0.7);
MGBench<tags::GPU::CUDA, methods::CYCLE::W::STATIC, DenseVector<double> > cuda_q1_sort1_l8_jac_w("MGBench cuda | q1 | sort 1 | L8 | jac | W", 1 , 1, 8, "jac", 0.7);
MGBench<tags::GPU::CUDA, methods::CYCLE::W::STATIC, DenseVector<double> > cuda_q1_sort1_l9_jac_w("MGBench cuda | q1 | sort 1 | L9 | jac | W", 1 , 1, 9, "jac", 0.7);


MGBench<tags::GPU::CUDA, methods::CYCLE::W::STATIC, DenseVector<double> > cuda_q1_sort2_l7_jac_w("MGBench cuda | q1 | sort 2 | L7 | jac | W", 1 , 2, 7, "jac", 0.7);
MGBench<tags::GPU::CUDA, methods::CYCLE::W::STATIC, DenseVector<double> > cuda_q1_sort2_l8_jac_w("MGBench cuda | q1 | sort 2 | L8 | jac | W", 1 , 2, 8, "jac", 0.7);
MGBench<tags::GPU::CUDA, methods::CYCLE::W::STATIC, DenseVector<double> > cuda_q1_sort2_l9_jac_w("MGBench cuda | q1 | sort 2 | L9 | jac | W", 1 , 2, 9, "jac", 0.7);

MGBench<tags::GPU::CUDA, methods::CYCLE::W::STATIC, DenseVector<double> > cuda_q1_sort3_l7_jac_w("MGBench cuda | q1 | sort 3 | L7 | jac | W", 1 , 3, 7, "jac", 0.7);
MGBench<tags::GPU::CUDA, methods::CYCLE::W::STATIC, DenseVector<double> > cuda_q1_sort3_l8_jac_w("MGBench cuda | q1 | sort 3 | L8 | jac | W", 1 , 3, 8, "jac", 0.7);
MGBench<tags::GPU::CUDA, methods::CYCLE::W::STATIC, DenseVector<double> > cuda_q1_sort3_l9_jac_w("MGBench cuda | q1 | sort 3 | L9 | jac | W", 1 , 3, 9, "jac", 0.7);

MGBench<tags::GPU::CUDA, methods::CYCLE::W::STATIC, DenseVector<double> > cuda_q1_sort4_l7_jac_w("MGBench cuda | q1 | sort 4 | L7 | jac | W", 1 , 4, 7, "jac", 0.7);
MGBench<tags::GPU::CUDA, methods::CYCLE::W::STATIC, DenseVector<double> > cuda_q1_sort4_l8_jac_w("MGBench cuda | q1 | sort 4 | L8 | jac | W", 1 , 4, 8, "jac", 0.7);
MGBench<tags::GPU::CUDA, methods::CYCLE::W::STATIC, DenseVector<double> > cuda_q1_sort4_l9_jac_w("MGBench cuda | q1 | sort 4 | L9 | jac | W", 1 , 4, 9, "jac", 0.7);

//q2
MGBench<tags::GPU::CUDA, methods::CYCLE::W::STATIC, DenseVector<double> > cuda_q2_sort0_l6_jac_w("MGBench cuda | q2 | sort 0 | L6 | jac | W", 2 , 0, 6, "jac", 0.7);
MGBench<tags::GPU::CUDA, methods::CYCLE::W::STATIC, DenseVector<double> > cuda_q2_sort0_l7_jac_w("MGBench cuda | q2 | sort 0 | L7 | jac | W", 2 , 0, 7, "jac", 0.7);
MGBench<tags::GPU::CUDA, methods::CYCLE::W::STATIC, DenseVector<double> > cuda_q2_sort0_l8_jac_w("MGBench cuda | q2 | sort 0 | L8 | jac | W", 2 , 0, 8, "jac", 0.7);


MGBench<tags::GPU::CUDA, methods::CYCLE::W::STATIC, DenseVector<double> > cuda_q2_sort1_l6_jac_w("MGBench cuda | q2 | sort 1 | L6 | jac | W", 2 , 1, 6, "jac", 0.7);
MGBench<tags::GPU::CUDA, methods::CYCLE::W::STATIC, DenseVector<double> > cuda_q2_sort1_l7_jac_w("MGBench cuda | q2 | sort 1 | L7 | jac | W", 2 , 1, 7, "jac", 0.7);
MGBench<tags::GPU::CUDA, methods::CYCLE::W::STATIC, DenseVector<double> > cuda_q2_sort1_l8_jac_w("MGBench cuda | q2 | sort 1 | L8 | jac | W", 2 , 1, 8, "jac", 0.7);


MGBench<tags::GPU::CUDA, methods::CYCLE::W::STATIC, DenseVector<double> > cuda_q2_sort2_l6_jac_w("MGBench cuda | q2 | sort 2 | L6 | jac | W", 2 , 2, 6, "jac", 0.7);
MGBench<tags::GPU::CUDA, methods::CYCLE::W::STATIC, DenseVector<double> > cuda_q2_sort2_l7_jac_w("MGBench cuda | q2 | sort 2 | L7 | jac | W", 2 , 2, 7, "jac", 0.7);
MGBench<tags::GPU::CUDA, methods::CYCLE::W::STATIC, DenseVector<double> > cuda_q2_sort2_l8_jac_w("MGBench cuda | q2 | sort 2 | L8 | jac | W", 2 , 2, 8, "jac", 0.7);

MGBench<tags::GPU::CUDA, methods::CYCLE::W::STATIC, DenseVector<double> > cuda_q2_sort3_l6_jac_w("MGBench cuda | q2 | sort 3 | L6 | jac | W", 2 , 3, 6, "jac", 0.7);
MGBench<tags::GPU::CUDA, methods::CYCLE::W::STATIC, DenseVector<double> > cuda_q2_sort3_l7_jac_w("MGBench cuda | q2 | sort 3 | L7 | jac | W", 2 , 3, 7, "jac", 0.7);
MGBench<tags::GPU::CUDA, methods::CYCLE::W::STATIC, DenseVector<double> > cuda_q2_sort3_l8_jac_w("MGBench cuda | q2 | sort 3 | L8 | jac | W", 2 , 3, 8, "jac", 0.7);

MGBench<tags::GPU::CUDA, methods::CYCLE::W::STATIC, DenseVector<double> > cuda_q2_sort4_l6_jac_w("MGBench cuda | q2 | sort 4 | L6 | jac | W", 2 , 4, 6, "jac", 0.7);
MGBench<tags::GPU::CUDA, methods::CYCLE::W::STATIC, DenseVector<double> > cuda_q2_sort4_l7_jac_w("MGBench cuda | q2 | sort 4 | L7 | jac | W", 2 , 4, 7, "jac", 0.7);
MGBench<tags::GPU::CUDA, methods::CYCLE::W::STATIC, DenseVector<double> > cuda_q2_sort4_l8_jac_w("MGBench cuda | q2 | sort 4 | L8 | jac | W", 2 , 4, 8, "jac", 0.7);

//q1t
MGBench<tags::GPU::CUDA, methods::CYCLE::W::STATIC, DenseVector<double> > cuda_q1t_sort0_l6_jac_w("MGBench cuda | q1t | sort 0 | L6 | jac | W", 3 , 0, 6, "jac", 0.7);
MGBench<tags::GPU::CUDA, methods::CYCLE::W::STATIC, DenseVector<double> > cuda_q1t_sort0_l7_jac_w("MGBench cuda | q1t | sort 0 | L7 | jac | W", 3 , 0, 7, "jac", 0.7);
MGBench<tags::GPU::CUDA, methods::CYCLE::W::STATIC, DenseVector<double> > cuda_q1t_sort0_l8_jac_w("MGBench cuda | q1t | sort 0 | L8 | jac | W", 3 , 0, 8, "jac", 0.7);


MGBench<tags::GPU::CUDA, methods::CYCLE::W::STATIC, DenseVector<double> > cuda_q1t_sort1_l6_jac_w("MGBench cuda | q1t | sort 1 | L6 | jac | W", 3 , 1, 6, "jac", 0.7);
MGBench<tags::GPU::CUDA, methods::CYCLE::W::STATIC, DenseVector<double> > cuda_q1t_sort1_l7_jac_w("MGBench cuda | q1t | sort 1 | L7 | jac | W", 3 , 1, 7, "jac", 0.7);
MGBench<tags::GPU::CUDA, methods::CYCLE::W::STATIC, DenseVector<double> > cuda_q1t_sort1_l8_jac_w("MGBench cuda | q1t | sort 1 | L8 | jac | W", 3 , 1, 8, "jac", 0.7);


MGBench<tags::GPU::CUDA, methods::CYCLE::W::STATIC, DenseVector<double> > cuda_q1t_sort2_l6_jac_w("MGBench cuda | q1t | sort 2 | L6 | jac | W", 3 , 2, 6, "jac", 0.7);
MGBench<tags::GPU::CUDA, methods::CYCLE::W::STATIC, DenseVector<double> > cuda_q1t_sort2_l7_jac_w("MGBench cuda | q1t | sort 2 | L7 | jac | W", 3 , 2, 7, "jac", 0.7);
MGBench<tags::GPU::CUDA, methods::CYCLE::W::STATIC, DenseVector<double> > cuda_q1t_sort2_l8_jac_w("MGBench cuda | q1t | sort 2 | L8 | jac | W", 3 , 2, 8, "jac", 0.7);

MGBench<tags::GPU::CUDA, methods::CYCLE::W::STATIC, DenseVector<double> > cuda_q1t_sort3_l6_jac_w("MGBench cuda | q1t | sort 3 | L6 | jac | W", 3 , 3, 6, "jac", 0.7);
MGBench<tags::GPU::CUDA, methods::CYCLE::W::STATIC, DenseVector<double> > cuda_q1t_sort3_l7_jac_w("MGBench cuda | q1t | sort 3 | L7 | jac | W", 3 , 3, 7, "jac", 0.7);
MGBench<tags::GPU::CUDA, methods::CYCLE::W::STATIC, DenseVector<double> > cuda_q1t_sort3_l8_jac_w("MGBench cuda | q1t | sort 3 | L8 | jac | W", 3 , 3, 8, "jac", 0.7);

MGBench<tags::GPU::CUDA, methods::CYCLE::W::STATIC, DenseVector<double> > cuda_q1t_sort4_l6_jac_w("MGBench cuda | q1t | sort 4 | L6 | jac | W", 3 , 4, 6, "jac", 0.7);
MGBench<tags::GPU::CUDA, methods::CYCLE::W::STATIC, DenseVector<double> > cuda_q1t_sort4_l7_jac_w("MGBench cuda | q1t | sort 4 | L7 | jac | W", 3 , 4, 7, "jac", 0.7);
MGBench<tags::GPU::CUDA, methods::CYCLE::W::STATIC, DenseVector<double> > cuda_q1t_sort4_l8_jac_w("MGBench cuda | q1t | sort 4 | L8 | jac | W", 3 , 4, 8, "jac", 0.7);

//SPAI
//q1
MGBench<tags::GPU::CUDA, methods::CYCLE::W::STATIC, SparseMatrixELL<double> > cuda_q1_sort0_l7_spai_grote_w("MGBench cuda | q1 | sort 0 | L7 | spai_grote | W", 1 , 0, 7, "spai_grote", 1.);
MGBench<tags::GPU::CUDA, methods::CYCLE::W::STATIC, SparseMatrixELL<double> > cuda_q1_sort0_l8_spai_grote_w("MGBench cuda | q1 | sort 0 | L8 | spai_grote | W", 1 , 0, 8, "spai_grote", 1.);
MGBench<tags::GPU::CUDA, methods::CYCLE::W::STATIC, SparseMatrixELL<double> > cuda_q1_sort0_l9_spai_grote_w("MGBench cuda | q1 | sort 0 | L9 | spai_grote | W", 1 , 0, 9, "spai_grote", 1.);


MGBench<tags::GPU::CUDA, methods::CYCLE::W::STATIC, SparseMatrixELL<double> > cuda_q1_sort1_l7_spai_grote_w("MGBench cuda | q1 | sort 1 | L7 | spai_grote | W", 1 , 1, 7, "spai_grote", 1.);
MGBench<tags::GPU::CUDA, methods::CYCLE::W::STATIC, SparseMatrixELL<double> > cuda_q1_sort1_l8_spai_grote_w("MGBench cuda | q1 | sort 1 | L8 | spai_grote | W", 1 , 1, 8, "spai_grote", 1.);
MGBench<tags::GPU::CUDA, methods::CYCLE::W::STATIC, SparseMatrixELL<double> > cuda_q1_sort1_l9_spai_grote_w("MGBench cuda | q1 | sort 1 | L9 | spai_grote | W", 1 , 1, 9, "spai_grote", 1.);


MGBench<tags::GPU::CUDA, methods::CYCLE::W::STATIC, SparseMatrixELL<double> > cuda_q1_sort2_l7_spai_grote_w("MGBench cuda | q1 | sort 2 | L7 | spai_grote | W", 1 , 2, 7, "spai_grote", 1.);
MGBench<tags::GPU::CUDA, methods::CYCLE::W::STATIC, SparseMatrixELL<double> > cuda_q1_sort2_l8_spai_grote_w("MGBench cuda | q1 | sort 2 | L8 | spai_grote | W", 1 , 2, 8, "spai_grote", 1.);
MGBench<tags::GPU::CUDA, methods::CYCLE::W::STATIC, SparseMatrixELL<double> > cuda_q1_sort2_l9_spai_grote_w("MGBench cuda | q1 | sort 2 | L9 | spai_grote | W", 1 , 2, 9, "spai_grote", 1.);

MGBench<tags::GPU::CUDA, methods::CYCLE::W::STATIC, SparseMatrixELL<double> > cuda_q1_sort3_l7_spai_grote_w("MGBench cuda | q1 | sort 3 | L7 | spai_grote | W", 1 , 3, 7, "spai_grote", 1.);
MGBench<tags::GPU::CUDA, methods::CYCLE::W::STATIC, SparseMatrixELL<double> > cuda_q1_sort3_l8_spai_grote_w("MGBench cuda | q1 | sort 3 | L8 | spai_grote | W", 1 , 3, 8, "spai_grote", 1.);
MGBench<tags::GPU::CUDA, methods::CYCLE::W::STATIC, SparseMatrixELL<double> > cuda_q1_sort3_l9_spai_grote_w("MGBench cuda | q1 | sort 3 | L9 | spai_grote | W", 1 , 3, 9, "spai_grote", 1.);

MGBench<tags::GPU::CUDA, methods::CYCLE::W::STATIC, SparseMatrixELL<double> > cuda_q1_sort4_l7_spai_grote_w("MGBench cuda | q1 | sort 4 | L7 | spai_grote | W", 1 , 4, 7, "spai_grote", 1.);
MGBench<tags::GPU::CUDA, methods::CYCLE::W::STATIC, SparseMatrixELL<double> > cuda_q1_sort4_l8_spai_grote_w("MGBench cuda | q1 | sort 4 | L8 | spai_grote | W", 1 , 4, 8, "spai_grote", 1.);
MGBench<tags::GPU::CUDA, methods::CYCLE::W::STATIC, SparseMatrixELL<double> > cuda_q1_sort4_l9_spai_grote_w("MGBench cuda | q1 | sort 4 | L9 | spai_grote | W", 1 , 4, 9, "spai_grote", 1.);

//q2
MGBench<tags::GPU::CUDA, methods::CYCLE::W::STATIC, SparseMatrixELL<double> > cuda_q2_sort0_l6_spai_grote_w("MGBench cuda | q2 | sort 0 | L6 | spai_grote | W", 2 , 0, 6, "spai_grote", 1.);
MGBench<tags::GPU::CUDA, methods::CYCLE::W::STATIC, SparseMatrixELL<double> > cuda_q2_sort0_l7_spai_grote_w("MGBench cuda | q2 | sort 0 | L7 | spai_grote | W", 2 , 0, 7, "spai_grote", 1.);
MGBench<tags::GPU::CUDA, methods::CYCLE::W::STATIC, SparseMatrixELL<double> > cuda_q2_sort0_l8_spai_grote_w("MGBench cuda | q2 | sort 0 | L8 | spai_grote | W", 2 , 0, 8, "spai_grote", 1.);


MGBench<tags::GPU::CUDA, methods::CYCLE::W::STATIC, SparseMatrixELL<double> > cuda_q2_sort1_l6_spai_grote_w("MGBench cuda | q2 | sort 1 | L6 | spai_grote | W", 2 , 1, 6, "spai_grote", 1.);
MGBench<tags::GPU::CUDA, methods::CYCLE::W::STATIC, SparseMatrixELL<double> > cuda_q2_sort1_l7_spai_grote_w("MGBench cuda | q2 | sort 1 | L7 | spai_grote | W", 2 , 1, 7, "spai_grote", 1.);
MGBench<tags::GPU::CUDA, methods::CYCLE::W::STATIC, SparseMatrixELL<double> > cuda_q2_sort1_l8_spai_grote_w("MGBench cuda | q2 | sort 1 | L8 | spai_grote | W", 2 , 1, 8, "spai_grote", 1.);

MGBench<tags::GPU::CUDA, methods::CYCLE::W::STATIC, SparseMatrixELL<double> > cuda_q2_sort2_l6_spai_grote_w("MGBench cuda | q2 | sort 2 | L6 | spai_grote | W", 2 , 1, 6, "spai_grote", 1.);
MGBench<tags::GPU::CUDA, methods::CYCLE::W::STATIC, SparseMatrixELL<double> > cuda_q2_sort2_l7_spai_grote_w("MGBench cuda | q2 | sort 2 | L7 | spai_grote | W", 2 , 1, 7, "spai_grote", 1.);
MGBench<tags::GPU::CUDA, methods::CYCLE::W::STATIC, SparseMatrixELL<double> > cuda_q2_sort2_l8_spai_grote_w("MGBench cuda | q2 | sort 2 | L8 | spai_grote | W", 2 , 1, 8, "spai_grote", 1.);

MGBench<tags::GPU::CUDA, methods::CYCLE::W::STATIC, SparseMatrixELL<double> > cuda_q2_sort3_l6_spai_grote_w("MGBench cuda | q2 | sort 3 | L6 | spai_grote | W", 2 , 3, 6, "spai_grote", 1.);
MGBench<tags::GPU::CUDA, methods::CYCLE::W::STATIC, SparseMatrixELL<double> > cuda_q2_sort3_l7_spai_grote_w("MGBench cuda | q2 | sort 3 | L7 | spai_grote | W", 2 , 3, 7, "spai_grote", 1.);
MGBench<tags::GPU::CUDA, methods::CYCLE::W::STATIC, SparseMatrixELL<double> > cuda_q2_sort3_l8_spai_grote_w("MGBench cuda | q2 | sort 3 | L8 | spai_grote | W", 2 , 3, 8, "spai_grote", 1.);


MGBench<tags::GPU::CUDA, methods::CYCLE::W::STATIC, SparseMatrixELL<double> > cuda_q2_sort4_l6_spai_grote_w("MGBench cuda | q2 | sort 4 | L6 | spai_grote | W", 2 , 4, 6, "spai_grote", 1.);
MGBench<tags::GPU::CUDA, methods::CYCLE::W::STATIC, SparseMatrixELL<double> > cuda_q2_sort4_l7_spai_grote_w("MGBench cuda | q2 | sort 4 | L7 | spai_grote | W", 2 , 4, 7, "spai_grote", 1.);
MGBench<tags::GPU::CUDA, methods::CYCLE::W::STATIC, SparseMatrixELL<double> > cuda_q2_sort4_l8_spai_grote_w("MGBench cuda | q2 | sort 4 | L8 | spai_grote | W", 2 , 4, 8, "spai_grote", 1.);

//q1t
MGBench<tags::GPU::CUDA, methods::CYCLE::W::STATIC, SparseMatrixELL<double> > cuda_q1t_sort0_l6_spai_grote_w("MGBench cuda | q1t | sort 0 | L6 | spai_grote | W", 3 , 0, 6, "spai_grote", 1.);
MGBench<tags::GPU::CUDA, methods::CYCLE::W::STATIC, SparseMatrixELL<double> > cuda_q1t_sort0_l7_spai_grote_w("MGBench cuda | q1t | sort 0 | L7 | spai_grote | W", 3 , 0, 7, "spai_grote", 1.);
MGBench<tags::GPU::CUDA, methods::CYCLE::W::STATIC, SparseMatrixELL<double> > cuda_q1t_sort0_l8_spai_grote_w("MGBench cuda | q1t | sort 0 | L8 | spai_grote | W", 3 , 0, 8, "spai_grote", 1.);


MGBench<tags::GPU::CUDA, methods::CYCLE::W::STATIC, SparseMatrixELL<double> > cuda_q1t_sort1_l6_spai_grote_w("MGBench cuda | q1t | sort 1 | L6 | spai_grote | W", 3 , 1, 6, "spai_grote", 1.);
MGBench<tags::GPU::CUDA, methods::CYCLE::W::STATIC, SparseMatrixELL<double> > cuda_q1t_sort1_l7_spai_grote_w("MGBench cuda | q1t | sort 1 | L7 | spai_grote | W", 3 , 1, 7, "spai_grote", 1.);
MGBench<tags::GPU::CUDA, methods::CYCLE::W::STATIC, SparseMatrixELL<double> > cuda_q1t_sort1_l8_spai_grote_w("MGBench cuda | q1t | sort 1 | L8 | spai_grote | W", 3 , 1, 8, "spai_grote", 1.);

MGBench<tags::GPU::CUDA, methods::CYCLE::W::STATIC, SparseMatrixELL<double> > cuda_q1t_sort2_l6_spai_grote_w("MGBench cuda | q1t | sort 2 | L6 | spai_grote | W", 3 , 1, 6, "spai_grote", 1.);
MGBench<tags::GPU::CUDA, methods::CYCLE::W::STATIC, SparseMatrixELL<double> > cuda_q1t_sort2_l7_spai_grote_w("MGBench cuda | q1t | sort 2 | L7 | spai_grote | W", 3 , 1, 7, "spai_grote", 1.);
MGBench<tags::GPU::CUDA, methods::CYCLE::W::STATIC, SparseMatrixELL<double> > cuda_q1t_sort2_l8_spai_grote_w("MGBench cuda | q1t | sort 2 | L8 | spai_grote | W", 3 , 1, 8, "spai_grote", 1.);

MGBench<tags::GPU::CUDA, methods::CYCLE::W::STATIC, SparseMatrixELL<double> > cuda_q1t_sort3_l6_spai_grote_w("MGBench cuda | q1t | sort 3 | L6 | spai_grote | W", 3 , 3, 6, "spai_grote", 1.);
MGBench<tags::GPU::CUDA, methods::CYCLE::W::STATIC, SparseMatrixELL<double> > cuda_q1t_sort3_l7_spai_grote_w("MGBench cuda | q1t | sort 3 | L7 | spai_grote | W", 3 , 3, 7, "spai_grote", 1.);
MGBench<tags::GPU::CUDA, methods::CYCLE::W::STATIC, SparseMatrixELL<double> > cuda_q1t_sort3_l8_spai_grote_w("MGBench cuda | q1t | sort 3 | L8 | spai_grote | W", 3 , 3, 8, "spai_grote", 1.);


MGBench<tags::GPU::CUDA, methods::CYCLE::W::STATIC, SparseMatrixELL<double> > cuda_q1t_sort4_l6_spai_grote_w("MGBench cuda | q1t | sort 4 | L6 | spai_grote | W", 3 , 4, 6, "spai_grote", 1.);
MGBench<tags::GPU::CUDA, methods::CYCLE::W::STATIC, SparseMatrixELL<double> > cuda_q1t_sort4_l7_spai_grote_w("MGBench cuda | q1t | sort 4 | L7 | spai_grote | W", 3 , 4, 7, "spai_grote", 1.);
MGBench<tags::GPU::CUDA, methods::CYCLE::W::STATIC, SparseMatrixELL<double> > cuda_q1t_sort4_l8_spai_grote_w("MGBench cuda | q1t | sort 4 | L8 | spai_grote | W", 3 , 4, 8, "spai_grote", 1.);
//SAINV
//q1
MGBench<tags::GPU::CUDA, methods::CYCLE::W::STATIC, SparseMatrixELL<double> > cuda_q1_sort0_l7_sainv_w("MGBench cuda | q1 | sort 0 | L7 | sainv | W", 1 , 0, 7, "sainv", 0.7);
MGBench<tags::GPU::CUDA, methods::CYCLE::W::STATIC, SparseMatrixELL<double> > cuda_q1_sort0_l8_sainv_w("MGBench cuda | q1 | sort 0 | L8 | sainv | W", 1 , 0, 8, "sainv", 0.7);
MGBench<tags::GPU::CUDA, methods::CYCLE::W::STATIC, SparseMatrixELL<double> > cuda_q1_sort0_l9_sainv_w("MGBench cuda | q1 | sort 0 | L9 | sainv | W", 1 , 0, 9, "sainv", 0.7);


MGBench<tags::GPU::CUDA, methods::CYCLE::W::STATIC, SparseMatrixELL<double> > cuda_q1_sort1_l7_sainv_w("MGBench cuda | q1 | sort 1 | L7 | sainv | W", 1 , 1, 7, "sainv", 0.7);
MGBench<tags::GPU::CUDA, methods::CYCLE::W::STATIC, SparseMatrixELL<double> > cuda_q1_sort1_l8_sainv_w("MGBench cuda | q1 | sort 1 | L8 | sainv | W", 1 , 1, 8, "sainv", 0.7);
MGBench<tags::GPU::CUDA, methods::CYCLE::W::STATIC, SparseMatrixELL<double> > cuda_q1_sort1_l9_sainv_w("MGBench cuda | q1 | sort 1 | L9 | sainv | W", 1 , 1, 9, "sainv", 0.7);


MGBench<tags::GPU::CUDA, methods::CYCLE::W::STATIC, SparseMatrixELL<double> > cuda_q1_sort2_l7_sainv_w("MGBench cuda | q1 | sort 2 | L7 | sainv | W", 1 , 2, 7, "sainv", 0.7);
MGBench<tags::GPU::CUDA, methods::CYCLE::W::STATIC, SparseMatrixELL<double> > cuda_q1_sort2_l8_sainv_w("MGBench cuda | q1 | sort 2 | L8 | sainv | W", 1 , 2, 8, "sainv", 0.7);
MGBench<tags::GPU::CUDA, methods::CYCLE::W::STATIC, SparseMatrixELL<double> > cuda_q1_sort2_l9_sainv_w("MGBench cuda | q1 | sort 2 | L9 | sainv | W", 1 , 2, 9, "sainv", 0.7);

MGBench<tags::GPU::CUDA, methods::CYCLE::W::STATIC, SparseMatrixELL<double> > cuda_q1_sort3_l7_sainv_w("MGBench cuda | q1 | sort 3 | L7 | sainv | W", 1 , 3, 7, "sainv", 0.7);
MGBench<tags::GPU::CUDA, methods::CYCLE::W::STATIC, SparseMatrixELL<double> > cuda_q1_sort3_l8_sainv_w("MGBench cuda | q1 | sort 3 | L8 | sainv | W", 1 , 3, 8, "sainv", 0.7);
MGBench<tags::GPU::CUDA, methods::CYCLE::W::STATIC, SparseMatrixELL<double> > cuda_q1_sort3_l9_sainv_w("MGBench cuda | q1 | sort 3 | L9 | sainv | W", 1 , 3, 9, "sainv", 0.7);

MGBench<tags::GPU::CUDA, methods::CYCLE::W::STATIC, SparseMatrixELL<double> > cuda_q1_sort4_l7_sainv_w("MGBench cuda | q1 | sort 4 | L7 | sainv | W", 1 , 4, 7, "sainv", 0.7);
MGBench<tags::GPU::CUDA, methods::CYCLE::W::STATIC, SparseMatrixELL<double> > cuda_q1_sort4_l8_sainv_w("MGBench cuda | q1 | sort 4 | L8 | sainv | W", 1 , 4, 8, "sainv", 0.7);
MGBench<tags::GPU::CUDA, methods::CYCLE::W::STATIC, SparseMatrixELL<double> > cuda_q1_sort4_l9_sainv_w("MGBench cuda | q1 | sort 4 | L9 | sainv | W", 1 , 4, 9, "sainv", 0.7);

//q2
MGBench<tags::GPU::CUDA, methods::CYCLE::W::STATIC, SparseMatrixELL<double> > cuda_q2_sort0_l6_sainv_w("MGBench cuda | q2 | sort 0 | L6 | sainv | W", 2 , 0, 6, "sainv", 0.7);
MGBench<tags::GPU::CUDA, methods::CYCLE::W::STATIC, SparseMatrixELL<double> > cuda_q2_sort0_l7_sainv_w("MGBench cuda | q2 | sort 0 | L7 | sainv | W", 2 , 0, 7, "sainv", 0.7);
MGBench<tags::GPU::CUDA, methods::CYCLE::W::STATIC, SparseMatrixELL<double> > cuda_q2_sort0_l8_sainv_w("MGBench cuda | q2 | sort 0 | L8 | sainv | W", 2 , 0, 8, "sainv", 0.7);


MGBench<tags::GPU::CUDA, methods::CYCLE::W::STATIC, SparseMatrixELL<double> > cuda_q2_sort1_l6_sainv_w("MGBench cuda | q2 | sort 1 | L6 | sainv | W", 2 , 1, 6, "sainv", 0.7);
MGBench<tags::GPU::CUDA, methods::CYCLE::W::STATIC, SparseMatrixELL<double> > cuda_q2_sort1_l7_sainv_w("MGBench cuda | q2 | sort 1 | L7 | sainv | W", 2 , 1, 7, "sainv", 0.7);
MGBench<tags::GPU::CUDA, methods::CYCLE::W::STATIC, SparseMatrixELL<double> > cuda_q2_sort1_l8_sainv_w("MGBench cuda | q2 | sort 1 | L8 | sainv | W", 2 , 1, 8, "sainv", 0.7);


MGBench<tags::GPU::CUDA, methods::CYCLE::W::STATIC, SparseMatrixELL<double> > cuda_q2_sort2_l6_sainv_w("MGBench cuda | q2 | sort 2 | L6 | sainv | W", 2 , 2, 6, "sainv", 0.7);
MGBench<tags::GPU::CUDA, methods::CYCLE::W::STATIC, SparseMatrixELL<double> > cuda_q2_sort2_l7_sainv_w("MGBench cuda | q2 | sort 2 | L7 | sainv | W", 2 , 2, 7, "sainv", 0.7);
MGBench<tags::GPU::CUDA, methods::CYCLE::W::STATIC, SparseMatrixELL<double> > cuda_q2_sort2_l8_sainv_w("MGBench cuda | q2 | sort 2 | L8 | sainv | W", 2 , 2, 8, "sainv", 0.7);

MGBench<tags::GPU::CUDA, methods::CYCLE::W::STATIC, SparseMatrixELL<double> > cuda_q2_sort3_l6_sainv_w("MGBench cuda | q2 | sort 3 | L6 | sainv | W", 2 , 3, 6, "sainv", 0.7);
MGBench<tags::GPU::CUDA, methods::CYCLE::W::STATIC, SparseMatrixELL<double> > cuda_q2_sort3_l7_sainv_w("MGBench cuda | q2 | sort 3 | L7 | sainv | W", 2 , 3, 7, "sainv", 0.7);
MGBench<tags::GPU::CUDA, methods::CYCLE::W::STATIC, SparseMatrixELL<double> > cuda_q2_sort3_l8_sainv_w("MGBench cuda | q2 | sort 3 | L8 | sainv | W", 2 , 3, 8, "sainv", 0.7);


MGBench<tags::GPU::CUDA, methods::CYCLE::W::STATIC, SparseMatrixELL<double> > cuda_q2_sort4_l6_sainv_w("MGBench cuda | q2 | sort 4 | L6 | sainv | W", 2 , 4, 6, "sainv", 0.7);
MGBench<tags::GPU::CUDA, methods::CYCLE::W::STATIC, SparseMatrixELL<double> > cuda_q2_sort4_l7_sainv_w("MGBench cuda | q2 | sort 4 | L7 | sainv | W", 2 , 4, 7, "sainv", 0.7);
MGBench<tags::GPU::CUDA, methods::CYCLE::W::STATIC, SparseMatrixELL<double> > cuda_q2_sort4_l8_sainv_w("MGBench cuda | q2 | sort 4 | L8 | sainv | W", 2 , 4, 8, "sainv", 0.7);

//q1t
MGBench<tags::GPU::CUDA, methods::CYCLE::W::STATIC, SparseMatrixELL<double> > cuda_q1t_sort0_l6_sainv_w("MGBench cuda | q1t | sort 0 | L6 | sainv | W", 3 , 0, 6, "sainv", 0.7);
MGBench<tags::GPU::CUDA, methods::CYCLE::W::STATIC, SparseMatrixELL<double> > cuda_q1t_sort0_l7_sainv_w("MGBench cuda | q1t | sort 0 | L7 | sainv | W", 3 , 0, 7, "sainv", 0.7);
MGBench<tags::GPU::CUDA, methods::CYCLE::W::STATIC, SparseMatrixELL<double> > cuda_q1t_sort0_l8_sainv_w("MGBench cuda | q1t | sort 0 | L8 | sainv | W", 3 , 0, 8, "sainv", 0.7);


MGBench<tags::GPU::CUDA, methods::CYCLE::W::STATIC, SparseMatrixELL<double> > cuda_q1t_sort1_l6_sainv_w("MGBench cuda | q1t | sort 1 | L6 | sainv | W", 3 , 1, 6, "sainv", 0.7);
MGBench<tags::GPU::CUDA, methods::CYCLE::W::STATIC, SparseMatrixELL<double> > cuda_q1t_sort1_l7_sainv_w("MGBench cuda | q1t | sort 1 | L7 | sainv | W", 3 , 1, 7, "sainv", 0.7);
MGBench<tags::GPU::CUDA, methods::CYCLE::W::STATIC, SparseMatrixELL<double> > cuda_q1t_sort1_l8_sainv_w("MGBench cuda | q1t | sort 1 | L8 | sainv | W", 3 , 1, 8, "sainv", 0.7);


MGBench<tags::GPU::CUDA, methods::CYCLE::W::STATIC, SparseMatrixELL<double> > cuda_q1t_sort2_l6_sainv_w("MGBench cuda | q1t | sort 2 | L6 | sainv | W", 3 , 2, 6, "sainv", 0.7);
MGBench<tags::GPU::CUDA, methods::CYCLE::W::STATIC, SparseMatrixELL<double> > cuda_q1t_sort2_l7_sainv_w("MGBench cuda | q1t | sort 2 | L7 | sainv | W", 3 , 2, 7, "sainv", 0.7);
MGBench<tags::GPU::CUDA, methods::CYCLE::W::STATIC, SparseMatrixELL<double> > cuda_q1t_sort2_l8_sainv_w("MGBench cuda | q1t | sort 2 | L8 | sainv | W", 3 , 2, 8, "sainv", 0.7);

MGBench<tags::GPU::CUDA, methods::CYCLE::W::STATIC, SparseMatrixELL<double> > cuda_q1t_sort3_l6_sainv_w("MGBench cuda | q1t | sort 3 | L6 | sainv | W", 3 , 3, 6, "sainv", 0.7);
MGBench<tags::GPU::CUDA, methods::CYCLE::W::STATIC, SparseMatrixELL<double> > cuda_q1t_sort3_l7_sainv_w("MGBench cuda | q1t | sort 3 | L7 | sainv | W", 3 , 3, 7, "sainv", 0.7);
MGBench<tags::GPU::CUDA, methods::CYCLE::W::STATIC, SparseMatrixELL<double> > cuda_q1t_sort3_l8_sainv_w("MGBench cuda | q1t | sort 3 | L8 | sainv | W", 3 , 3, 8, "sainv", 0.7);


MGBench<tags::GPU::CUDA, methods::CYCLE::W::STATIC, SparseMatrixELL<double> > cuda_q1t_sort4_l6_sainv_w("MGBench cuda | q1t | sort 4 | L6 | sainv | W", 3 , 4, 6, "sainv", 0.7);
MGBench<tags::GPU::CUDA, methods::CYCLE::W::STATIC, SparseMatrixELL<double> > cuda_q1t_sort4_l7_sainv_w("MGBench cuda | q1t | sort 4 | L7 | sainv | W", 3 , 4, 7, "sainv", 0.7);
MGBench<tags::GPU::CUDA, methods::CYCLE::W::STATIC, SparseMatrixELL<double> > cuda_q1t_sort4_l8_sainv_w("MGBench cuda | q1t | sort 4 | L8 | sainv | W", 3 , 4, 8, "sainv", 0.7);
#endif


//F-cycle
#ifdef HONEI_SSE
//JAC
//q1
MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::F::V::STATIC, DenseVector<double> > mcsse_q1_sort0_l7_jac_f("MGBench mcsse | q1 | sort 0 | L7 | jac | F", 1 , 0, 7, "jac", 0.7);
MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::F::V::STATIC, DenseVector<double> > mcsse_q1_sort0_l8_jac_f("MGBench mcsse | q1 | sort 0 | L8 | jac | F", 1 , 0, 8, "jac", 0.7);
MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::F::V::STATIC, DenseVector<double> > mcsse_q1_sort0_l9_jac_f("MGBench mcsse | q1 | sort 0 | L9 | jac | F", 1 , 0, 9, "jac", 0.7);


MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::F::V::STATIC, DenseVector<double> > mcsse_q1_sort1_l7_jac_f("MGBench mcsse | q1 | sort 1 | L7 | jac | F", 1 , 1, 7, "jac", 0.7);
MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::F::V::STATIC, DenseVector<double> > mcsse_q1_sort1_l8_jac_f("MGBench mcsse | q1 | sort 1 | L8 | jac | F", 1 , 1, 8, "jac", 0.7);
MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::F::V::STATIC, DenseVector<double> > mcsse_q1_sort1_l9_jac_f("MGBench mcsse | q1 | sort 1 | L9 | jac | F", 1 , 1, 9, "jac", 0.7);


MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::F::V::STATIC, DenseVector<double> > mcsse_q1_sort2_l7_jac_f("MGBench mcsse | q1 | sort 2 | L7 | jac | F", 1 , 2, 7, "jac", 0.7);
MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::F::V::STATIC, DenseVector<double> > mcsse_q1_sort2_l8_jac_f("MGBench mcsse | q1 | sort 2 | L8 | jac | F", 1 , 2, 8, "jac", 0.7);
MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::F::V::STATIC, DenseVector<double> > mcsse_q1_sort2_l9_jac_f("MGBench mcsse | q1 | sort 2 | L9 | jac | F", 1 , 2, 9, "jac", 0.7);

MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::F::V::STATIC, DenseVector<double> > mcsse_q1_sort3_l7_jac_f("MGBench mcsse | q1 | sort 3 | L7 | jac | F", 1 , 3, 7, "jac", 0.7);
MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::F::V::STATIC, DenseVector<double> > mcsse_q1_sort3_l8_jac_f("MGBench mcsse | q1 | sort 3 | L8 | jac | F", 1 , 3, 8, "jac", 0.7);
MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::F::V::STATIC, DenseVector<double> > mcsse_q1_sort3_l9_jac_f("MGBench mcsse | q1 | sort 3 | L9 | jac | F", 1 , 3, 9, "jac", 0.7);


MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::F::V::STATIC, DenseVector<double> > mcsse_q1_sort4_l7_jac_f("MGBench mcsse | q1 | sort 4 | L7 | jac | F", 1 , 4, 7, "jac", 0.7);
MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::F::V::STATIC, DenseVector<double> > mcsse_q1_sort4_l8_jac_f("MGBench mcsse | q1 | sort 4 | L8 | jac | F", 1 , 4, 8, "jac", 0.7);
MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::F::V::STATIC, DenseVector<double> > mcsse_q1_sort4_l9_jac_f("MGBench mcsse | q1 | sort 4 | L9 | jac | F", 1 , 4, 9, "jac", 0.7);

//q2
MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::F::V::STATIC, DenseVector<double> > mcsse_q2_sort0_l6_jac_f("MGBench mcsse | q2 | sort 0 | L6 | jac | F", 2 , 0, 6, "jac", 0.7);
MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::F::V::STATIC, DenseVector<double> > mcsse_q2_sort0_l7_jac_f("MGBench mcsse | q2 | sort 0 | L7 | jac | F", 2 , 0, 7, "jac", 0.7);
MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::F::V::STATIC, DenseVector<double> > mcsse_q2_sort0_l8_jac_f("MGBench mcsse | q2 | sort 0 | L8 | jac | F", 2 , 0, 8, "jac", 0.7);


MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::F::V::STATIC, DenseVector<double> > mcsse_q2_sort1_l6_jac_f("MGBench mcsse | q2 | sort 1 | L6 | jac | F", 2 , 1, 6, "jac", 0.7);
MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::F::V::STATIC, DenseVector<double> > mcsse_q2_sort1_l7_jac_f("MGBench mcsse | q2 | sort 1 | L7 | jac | F", 2 , 1, 7, "jac", 0.7);
MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::F::V::STATIC, DenseVector<double> > mcsse_q2_sort1_l8_jac_f("MGBench mcsse | q2 | sort 1 | L8 | jac | F", 2 , 1, 8, "jac", 0.7);


MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::F::V::STATIC, DenseVector<double> > mcsse_q2_sort2_l6_jac_f("MGBench mcsse | q2 | sort 2 | L6 | jac | F", 2 , 2, 6, "jac", 0.7);
MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::F::V::STATIC, DenseVector<double> > mcsse_q2_sort2_l7_jac_f("MGBench mcsse | q2 | sort 2 | L7 | jac | F", 2 , 2, 7, "jac", 0.7);
MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::F::V::STATIC, DenseVector<double> > mcsse_q2_sort2_l8_jac_f("MGBench mcsse | q2 | sort 2 | L8 | jac | F", 2 , 2, 8, "jac", 0.7);

MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::F::V::STATIC, DenseVector<double> > mcsse_q2_sort3_l6_jac_f("MGBench mcsse | q2 | sort 3 | L6 | jac | F", 2 , 3, 6, "jac", 0.7);
MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::F::V::STATIC, DenseVector<double> > mcsse_q2_sort3_l7_jac_f("MGBench mcsse | q2 | sort 3 | L7 | jac | F", 2 , 3, 7, "jac", 0.7);
MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::F::V::STATIC, DenseVector<double> > mcsse_q2_sort3_l8_jac_f("MGBench mcsse | q2 | sort 3 | L8 | jac | F", 2 , 3, 8, "jac", 0.7);

MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::F::V::STATIC, DenseVector<double> > mcsse_q2_sort4_l6_jac_f("MGBench mcsse | q2 | sort 4 | L6 | jac | F", 2 , 4, 6, "jac", 0.7);
MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::F::V::STATIC, DenseVector<double> > mcsse_q2_sort4_l7_jac_f("MGBench mcsse | q2 | sort 4 | L7 | jac | F", 2 , 4, 7, "jac", 0.7);
MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::F::V::STATIC, DenseVector<double> > mcsse_q2_sort4_l8_jac_f("MGBench mcsse | q2 | sort 4 | L8 | jac | F", 2 , 4, 8, "jac", 0.7);

//q1t
MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::F::V::STATIC, DenseVector<double> > mcsse_q1t_sort0_l6_jac_f("MGBench mcsse | q1t | sort 0 | L6 | jac | F", 3, 0, 6, "jac", 0.7);
MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::F::V::STATIC, DenseVector<double> > mcsse_q1t_sort0_l7_jac_f("MGBench mcsse | q1t | sort 0 | L7 | jac | F", 3 , 0, 7, "jac", 0.7);
MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::F::V::STATIC, DenseVector<double> > mcsse_q1t_sort0_l8_jac_f("MGBench mcsse | q1t | sort 0 | L8 | jac | F", 3 , 0, 8, "jac", 0.7);


MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::F::V::STATIC, DenseVector<double> > mcsse_q1t_sort1_l6_jac_f("MGBench mcsse | q1t | sort 1 | L6 | jac | F", 3 , 1, 6, "jac", 0.7);
MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::F::V::STATIC, DenseVector<double> > mcsse_q1t_sort1_l7_jac_f("MGBench mcsse | q1t | sort 1 | L7 | jac | F", 3 , 1, 7, "jac", 0.7);
MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::F::V::STATIC, DenseVector<double> > mcsse_q1t_sort1_l8_jac_f("MGBench mcsse | q1t | sort 1 | L8 | jac | F", 3 , 1, 8, "jac", 0.7);


MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::F::V::STATIC, DenseVector<double> > mcsse_q1t_sort2_l6_jac_f("MGBench mcsse | q1t | sort 2 | L6 | jac | F", 3 , 2, 6, "jac", 0.7);
MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::F::V::STATIC, DenseVector<double> > mcsse_q1t_sort2_l7_jac_f("MGBench mcsse | q1t | sort 2 | L7 | jac | F", 3 , 2, 7, "jac", 0.7);
MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::F::V::STATIC, DenseVector<double> > mcsse_q1t_sort2_l8_jac_f("MGBench mcsse | q1t | sort 2 | L8 | jac | F", 3 , 2, 8, "jac", 0.7);

MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::F::V::STATIC, DenseVector<double> > mcsse_q1t_sort3_l6_jac_f("MGBench mcsse | q1t | sort 3 | L6 | jac | F", 3 , 3, 6, "jac", 0.7);
MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::F::V::STATIC, DenseVector<double> > mcsse_q1t_sort3_l7_jac_f("MGBench mcsse | q1t | sort 3 | L7 | jac | F", 3 , 3, 7, "jac", 0.7);
MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::F::V::STATIC, DenseVector<double> > mcsse_q1t_sort3_l8_jac_f("MGBench mcsse | q1t | sort 3 | L8 | jac | F", 3 , 3, 8, "jac", 0.7);

MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::F::V::STATIC, DenseVector<double> > mcsse_q1t_sort4_l6_jac_f("MGBench mcsse | q1t | sort 4 | L6 | jac | F", 3 , 4, 6, "jac", 0.7);
MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::F::V::STATIC, DenseVector<double> > mcsse_q1t_sort4_l7_jac_f("MGBench mcsse | q1t | sort 4 | L7 | jac | F", 3 , 4, 7, "jac", 0.7);
MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::F::V::STATIC, DenseVector<double> > mcsse_q1t_sort4_l8_jac_f("MGBench mcsse | q1t | sort 4 | L8 | jac | F", 3 , 4, 8, "jac", 0.7);

//SPAI
//q1
MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::F::V::STATIC, SparseMatrixELL<double> > mcsse_q1_sort0_l7_spai_grote_f("MGBench mcsse | q1 | sort 0 | L7 | spai_grote | F", 1 , 0, 7, "spai_grote", 1.);
MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::F::V::STATIC, SparseMatrixELL<double> > mcsse_q1_sort0_l8_spai_grote_f("MGBench mcsse | q1 | sort 0 | L8 | spai_grote | F", 1 , 0, 8, "spai_grote", 1.);
MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::F::V::STATIC, SparseMatrixELL<double> > mcsse_q1_sort0_l9_spai_grote_f("MGBench mcsse | q1 | sort 0 | L9 | spai_grote | F", 1 , 0, 9, "spai_grote", 1.);


MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::F::V::STATIC, SparseMatrixELL<double> > mcsse_q1_sort1_l7_spai_grote_f("MGBench mcsse | q1 | sort 1 | L7 | spai_grote | F", 1 , 1, 7, "spai_grote", 1.);
MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::F::V::STATIC, SparseMatrixELL<double> > mcsse_q1_sort1_l8_spai_grote_f("MGBench mcsse | q1 | sort 1 | L8 | spai_grote | F", 1 , 1, 8, "spai_grote", 1.);
MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::F::V::STATIC, SparseMatrixELL<double> > mcsse_q1_sort1_l9_spai_grote_f("MGBench mcsse | q1 | sort 1 | L9 | spai_grote | F", 1 , 1, 9, "spai_grote", 1.);


MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::F::V::STATIC, SparseMatrixELL<double> > mcsse_q1_sort2_l7_spai_grote_f("MGBench mcsse | q1 | sort 2 | L7 | spai_grote | F", 1 , 2, 7, "spai_grote", 1.);
MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::F::V::STATIC, SparseMatrixELL<double> > mcsse_q1_sort2_l8_spai_grote_f("MGBench mcsse | q1 | sort 2 | L8 | spai_grote | F", 1 , 2, 8, "spai_grote", 1.);
MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::F::V::STATIC, SparseMatrixELL<double> > mcsse_q1_sort2_l9_spai_grote_f("MGBench mcsse | q1 | sort 2 | L9 | spai_grote | F", 1 , 2, 9, "spai_grote", 1.);

MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::F::V::STATIC, SparseMatrixELL<double> > mcsse_q1_sort3_l7_spai_grote_f("MGBench mcsse | q1 | sort 3 | L7 | spai_grote | F", 1 , 3, 7, "spai_grote", 1.);
MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::F::V::STATIC, SparseMatrixELL<double> > mcsse_q1_sort3_l8_spai_grote_f("MGBench mcsse | q1 | sort 3 | L8 | spai_grote | F", 1 , 3, 8, "spai_grote", 1.);
MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::F::V::STATIC, SparseMatrixELL<double> > mcsse_q1_sort3_l9_spai_grote_f("MGBench mcsse | q1 | sort 3 | L9 | spai_grote | F", 1 , 3, 9, "spai_grote", 1.);

MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::F::V::STATIC, SparseMatrixELL<double> > mcsse_q1_sort4_l7_spai_grote_f("MGBench mcsse | q1 | sort 4 | L7 | spai_grote | F", 1 , 4, 7, "spai_grote", 1.);
MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::F::V::STATIC, SparseMatrixELL<double> > mcsse_q1_sort4_l8_spai_grote_f("MGBench mcsse | q1 | sort 4 | L8 | spai_grote | F", 1 , 4, 8, "spai_grote", 1.);
MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::F::V::STATIC, SparseMatrixELL<double> > mcsse_q1_sort4_l9_spai_grote_f("MGBench mcsse | q1 | sort 4 | L9 | spai_grote | F", 1 , 4, 9, "spai_grote", 1.);

//q2
MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::F::V::STATIC, SparseMatrixELL<double> > mcsse_q2_sort0_l6_spai_grote_f("MGBench mcsse | q2 | sort 0 | L6 | spai_grote | F", 2 , 0, 6, "spai_grote", 1.);
MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::F::V::STATIC, SparseMatrixELL<double> > mcsse_q2_sort0_l7_spai_grote_f("MGBench mcsse | q2 | sort 0 | L7 | spai_grote | F", 2 , 0, 7, "spai_grote", 1.);
MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::F::V::STATIC, SparseMatrixELL<double> > mcsse_q2_sort0_l8_spai_grote_f("MGBench mcsse | q2 | sort 0 | L8 | spai_grote | F", 2 , 0, 8, "spai_grote", 1.);


MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::F::V::STATIC, SparseMatrixELL<double> > mcsse_q2_sort1_l6_spai_grote_f("MGBench mcsse | q2 | sort 1 | L6 | spai_grote | F", 2 , 1, 6, "spai_grote", 1.);
MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::F::V::STATIC, SparseMatrixELL<double> > mcsse_q2_sort1_l7_spai_grote_f("MGBench mcsse | q2 | sort 1 | L7 | spai_grote | F", 2 , 1, 7, "spai_grote", 1.);
MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::F::V::STATIC, SparseMatrixELL<double> > mcsse_q2_sort1_l8_spai_grote_f("MGBench mcsse | q2 | sort 1 | L8 | spai_grote | F", 2 , 1, 8, "spai_grote", 1.);


MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::F::V::STATIC, SparseMatrixELL<double> > mcsse_q2_sort2_l6_spai_grote_f("MGBench mcsse | q2 | sort 2 | L6 | spai_grote | F", 2 , 2, 6, "spai_grote", 1.);
MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::F::V::STATIC, SparseMatrixELL<double> > mcsse_q2_sort2_l7_spai_grote_f("MGBench mcsse | q2 | sort 2 | L7 | spai_grote | F", 2 , 2, 7, "spai_grote", 1.);
MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::F::V::STATIC, SparseMatrixELL<double> > mcsse_q2_sort2_l8_spai_grote_f("MGBench mcsse | q2 | sort 2 | L8 | spai_grote | F", 2 , 2, 8, "spai_grote", 1.);

MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::F::V::STATIC, SparseMatrixELL<double> > mcsse_q2_sort3_l6_spai_grote_f("MGBench mcsse | q2 | sort 3 | L6 | spai_grote | F", 2 , 3, 6, "spai_grote", 1.);
MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::F::V::STATIC, SparseMatrixELL<double> > mcsse_q2_sort3_l7_spai_grote_f("MGBench mcsse | q2 | sort 3 | L7 | spai_grote | F", 2 , 3, 7, "spai_grote", 1.);
MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::F::V::STATIC, SparseMatrixELL<double> > mcsse_q2_sort3_l8_spai_grote_f("MGBench mcsse | q2 | sort 3 | L8 | spai_grote | F", 2 , 3, 8, "spai_grote", 1.);

MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::F::V::STATIC, SparseMatrixELL<double> > mcsse_q2_sort4_l6_spai_grote_f("MGBench mcsse | q2 | sort 4 | L6 | spai_grote | F", 2 , 4, 6, "spai_grote", 1.);
MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::F::V::STATIC, SparseMatrixELL<double> > mcsse_q2_sort4_l7_spai_grote_f("MGBench mcsse | q2 | sort 4 | L7 | spai_grote | F", 2 , 4, 7, "spai_grote", 1.);
MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::F::V::STATIC, SparseMatrixELL<double> > mcsse_q2_sort4_l8_spai_grote_f("MGBench mcsse | q2 | sort 4 | L8 | spai_grote | F", 2 , 4, 8, "spai_grote", 1.);

//q1t
MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::F::V::STATIC, SparseMatrixELL<double> > mcsse_q1t_sort0_l6_spai_grote_f("MGBench mcsse | q1t | sort 0 | L6 | spai_grote | F", 3, 0, 6, "spai_grote", 1.);
MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::F::V::STATIC, SparseMatrixELL<double> > mcsse_q1t_sort0_l7_spai_grote_f("MGBench mcsse | q1t | sort 0 | L7 | spai_grote | F", 3 , 0, 7, "spai_grote", 1.);
MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::F::V::STATIC, SparseMatrixELL<double> > mcsse_q1t_sort0_l8_spai_grote_f("MGBench mcsse | q1t | sort 0 | L8 | spai_grote | F", 3 , 0, 8, "spai_grote", 1.);


MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::F::V::STATIC, SparseMatrixELL<double> > mcsse_q1t_sort1_l6_spai_grote_f("MGBench mcsse | q1t | sort 1 | L6 | spai_grote | F", 3 , 1, 6, "spai_grote", 1.);
MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::F::V::STATIC, SparseMatrixELL<double> > mcsse_q1t_sort1_l7_spai_grote_f("MGBench mcsse | q1t | sort 1 | L7 | spai_grote | F", 3 , 1, 7, "spai_grote", 1.);
MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::F::V::STATIC, SparseMatrixELL<double> > mcsse_q1t_sort1_l8_spai_grote_f("MGBench mcsse | q1t | sort 1 | L8 | spai_grote | F", 3 , 1, 8, "spai_grote", 1.);


MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::F::V::STATIC, SparseMatrixELL<double> > mcsse_q1t_sort2_l6_spai_grote_f("MGBench mcsse | q1t | sort 2 | L6 | spai_grote | F", 3 , 2, 6, "spai_grote", 1.);
MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::F::V::STATIC, SparseMatrixELL<double> > mcsse_q1t_sort2_l7_spai_grote_f("MGBench mcsse | q1t | sort 2 | L7 | spai_grote | F", 3 , 2, 7, "spai_grote", 1.);
MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::F::V::STATIC, SparseMatrixELL<double> > mcsse_q1t_sort2_l8_spai_grote_f("MGBench mcsse | q1t | sort 2 | L8 | spai_grote | F", 3 , 2, 8, "spai_grote", 1.);

MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::F::V::STATIC, SparseMatrixELL<double> > mcsse_q1t_sort3_l6_spai_grote_f("MGBench mcsse | q1t | sort 3 | L6 | spai_grote | F", 3 , 3, 6, "spai_grote", 1.);
MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::F::V::STATIC, SparseMatrixELL<double> > mcsse_q1t_sort3_l7_spai_grote_f("MGBench mcsse | q1t | sort 3 | L7 | spai_grote | F", 3 , 3, 7, "spai_grote", 1.);
MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::F::V::STATIC, SparseMatrixELL<double> > mcsse_q1t_sort3_l8_spai_grote_f("MGBench mcsse | q1t | sort 3 | L8 | spai_grote | F", 3 , 3, 8, "spai_grote", 1.);

MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::F::V::STATIC, SparseMatrixELL<double> > mcsse_q1t_sort4_l6_spai_grote_f("MGBench mcsse | q1t | sort 4 | L6 | spai_grote | F", 3 , 4, 6, "spai_grote", 1.);
MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::F::V::STATIC, SparseMatrixELL<double> > mcsse_q1t_sort4_l7_spai_grote_f("MGBench mcsse | q1t | sort 4 | L7 | spai_grote | F", 3 , 4, 7, "spai_grote", 1.);
MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::F::V::STATIC, SparseMatrixELL<double> > mcsse_q1t_sort4_l8_spai_grote_f("MGBench mcsse | q1t | sort 4 | L8 | spai_grote | F", 3 , 4, 8, "spai_grote", 1.);

//SAINV
//q1
MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::F::V::STATIC, SparseMatrixELL<double> > mcsse_q1_sort0_l7_sainv_f("MGBench mcsse | q1 | sort 0 | L7 | sainv | F", 1 , 0, 7, "sainv", 0.7);
MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::F::V::STATIC, SparseMatrixELL<double> > mcsse_q1_sort0_l8_sainv_f("MGBench mcsse | q1 | sort 0 | L8 | sainv | F", 1 , 0, 8, "sainv", 0.7);
MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::F::V::STATIC, SparseMatrixELL<double> > mcsse_q1_sort0_l9_sainv_f("MGBench mcsse | q1 | sort 0 | L9 | sainv | F", 1 , 0, 9, "sainv", 0.7);


MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::F::V::STATIC, SparseMatrixELL<double> > mcsse_q1_sort1_l7_sainv_f("MGBench mcsse | q1 | sort 1 | L7 | sainv | F", 1 , 1, 7, "sainv", 0.7);
MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::F::V::STATIC, SparseMatrixELL<double> > mcsse_q1_sort1_l8_sainv_f("MGBench mcsse | q1 | sort 1 | L8 | sainv | F", 1 , 1, 8, "sainv", 0.7);
MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::F::V::STATIC, SparseMatrixELL<double> > mcsse_q1_sort1_l9_sainv_f("MGBench mcsse | q1 | sort 1 | L9 | sainv | F", 1 , 1, 9, "sainv", 0.7);


MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::F::V::STATIC, SparseMatrixELL<double> > mcsse_q1_sort2_l7_sainv_f("MGBench mcsse | q1 | sort 2 | L7 | sainv | F", 1 , 2, 7, "sainv", 0.7);
MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::F::V::STATIC, SparseMatrixELL<double> > mcsse_q1_sort2_l8_sainv_f("MGBench mcsse | q1 | sort 2 | L8 | sainv | F", 1 , 2, 8, "sainv", 0.7);
MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::F::V::STATIC, SparseMatrixELL<double> > mcsse_q1_sort2_l9_sainv_f("MGBench mcsse | q1 | sort 2 | L9 | sainv | F", 1 , 2, 9, "sainv", 0.7);

MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::F::V::STATIC, SparseMatrixELL<double> > mcsse_q1_sort3_l7_sainv_f("MGBench mcsse | q1 | sort 3 | L7 | sainv | F", 1 , 3, 7, "sainv", 0.7);
MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::F::V::STATIC, SparseMatrixELL<double> > mcsse_q1_sort3_l8_sainv_f("MGBench mcsse | q1 | sort 3 | L8 | sainv | F", 1 , 3, 8, "sainv", 0.7);
MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::F::V::STATIC, SparseMatrixELL<double> > mcsse_q1_sort3_l9_sainv_f("MGBench mcsse | q1 | sort 3 | L9 | sainv | F", 1 , 3, 9, "sainv", 0.7);

MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::F::V::STATIC, SparseMatrixELL<double> > mcsse_q1_sort4_l7_sainv_f("MGBench mcsse | q1 | sort 4 | L7 | sainv | F", 1 , 4, 7, "sainv", 0.7);
MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::F::V::STATIC, SparseMatrixELL<double> > mcsse_q1_sort4_l8_sainv_f("MGBench mcsse | q1 | sort 4 | L8 | sainv | F", 1 , 4, 8, "sainv", 0.7);
MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::F::V::STATIC, SparseMatrixELL<double> > mcsse_q1_sort4_l9_sainv_f("MGBench mcsse | q1 | sort 4 | L9 | sainv | F", 1 , 4, 9, "sainv", 0.7);

//q2
MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::F::V::STATIC, SparseMatrixELL<double> > mcsse_q2_sort0_l6_sainv_f("MGBench mcsse | q2 | sort 0 | L6 | sainv | F", 2 , 0, 6, "sainv", 0.7);
MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::F::V::STATIC, SparseMatrixELL<double> > mcsse_q2_sort0_l7_sainv_f("MGBench mcsse | q2 | sort 0 | L7 | sainv | F", 2 , 0, 7, "sainv", 0.7);
MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::F::V::STATIC, SparseMatrixELL<double> > mcsse_q2_sort0_l8_sainv_f("MGBench mcsse | q2 | sort 0 | L8 | sainv | F", 2 , 0, 8, "sainv", 0.7);


MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::F::V::STATIC, SparseMatrixELL<double> > mcsse_q2_sort1_l6_sainv_f("MGBench mcsse | q2 | sort 1 | L6 | sainv | F", 2 , 1, 6, "sainv", 0.7);
MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::F::V::STATIC, SparseMatrixELL<double> > mcsse_q2_sort1_l7_sainv_f("MGBench mcsse | q2 | sort 1 | L7 | sainv | F", 2 , 1, 7, "sainv", 0.7);
MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::F::V::STATIC, SparseMatrixELL<double> > mcsse_q2_sort1_l8_sainv_f("MGBench mcsse | q2 | sort 1 | L8 | sainv | F", 2 , 1, 8, "sainv", 0.7);


MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::F::V::STATIC, SparseMatrixELL<double> > mcsse_q2_sort2_l6_sainv_f("MGBench mcsse | q2 | sort 2 | L6 | sainv | F", 2 , 2, 6, "sainv", 0.7);
MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::F::V::STATIC, SparseMatrixELL<double> > mcsse_q2_sort2_l7_sainv_f("MGBench mcsse | q2 | sort 2 | L7 | sainv | F", 2 , 2, 7, "sainv", 0.7);
MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::F::V::STATIC, SparseMatrixELL<double> > mcsse_q2_sort2_l8_sainv_f("MGBench mcsse | q2 | sort 2 | L8 | sainv | F", 2 , 2, 8, "sainv", 0.7);

MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::F::V::STATIC, SparseMatrixELL<double> > mcsse_q2_sort3_l6_sainv_f("MGBench mcsse | q2 | sort 3 | L6 | sainv | F", 2 , 3, 6, "sainv", 0.7);
MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::F::V::STATIC, SparseMatrixELL<double> > mcsse_q2_sort3_l7_sainv_f("MGBench mcsse | q2 | sort 3 | L7 | sainv | F", 2 , 3, 7, "sainv", 0.7);
MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::F::V::STATIC, SparseMatrixELL<double> > mcsse_q2_sort3_l8_sainv_f("MGBench mcsse | q2 | sort 3 | L8 | sainv | F", 2 , 3, 8, "sainv", 0.7);

MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::F::V::STATIC, SparseMatrixELL<double> > mcsse_q2_sort4_l6_sainv_f("MGBench mcsse | q2 | sort 4 | L6 | sainv | F", 2 , 4, 6, "sainv", 0.7);
MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::F::V::STATIC, SparseMatrixELL<double> > mcsse_q2_sort4_l7_sainv_f("MGBench mcsse | q2 | sort 4 | L7 | sainv | F", 2 , 4, 7, "sainv", 0.7);
MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::F::V::STATIC, SparseMatrixELL<double> > mcsse_q2_sort4_l8_sainv_f("MGBench mcsse | q2 | sort 4 | L8 | sainv | F", 2 , 4, 8, "sainv", 0.7);


//q1t
MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::F::V::STATIC, SparseMatrixELL<double> > mcsse_q1t_sort0_l6_sainv_f("MGBench mcsse | q1t | sort 0 | L6 | sainv | F", 3 , 0, 6, "sainv", 0.7);
MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::F::V::STATIC, SparseMatrixELL<double> > mcsse_q1t_sort0_l7_sainv_f("MGBench mcsse | q1t | sort 0 | L7 | sainv | F", 3 , 0, 7, "sainv", 0.7);
MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::F::V::STATIC, SparseMatrixELL<double> > mcsse_q1t_sort0_l8_sainv_f("MGBench mcsse | q1t | sort 0 | L8 | sainv | F", 3 , 0, 8, "sainv", 0.7);


MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::F::V::STATIC, SparseMatrixELL<double> > mcsse_q1t_sort1_l6_sainv_f("MGBench mcsse | q1t | sort 1 | L6 | sainv | F", 3 , 1, 6, "sainv", 0.7);
MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::F::V::STATIC, SparseMatrixELL<double> > mcsse_q1t_sort1_l7_sainv_f("MGBench mcsse | q1t | sort 1 | L7 | sainv | F", 3 , 1, 7, "sainv", 0.7);
MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::F::V::STATIC, SparseMatrixELL<double> > mcsse_q1t_sort1_l8_sainv_f("MGBench mcsse | q1t | sort 1 | L8 | sainv | F", 3 , 1, 8, "sainv", 0.7);


MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::F::V::STATIC, SparseMatrixELL<double> > mcsse_q1t_sort2_l6_sainv_f("MGBench mcsse | q1t | sort 2 | L6 | sainv | F", 3 , 2, 6, "sainv", 0.7);
MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::F::V::STATIC, SparseMatrixELL<double> > mcsse_q1t_sort2_l7_sainv_f("MGBench mcsse | q1t | sort 2 | L7 | sainv | F", 3 , 2, 7, "sainv", 0.7);
MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::F::V::STATIC, SparseMatrixELL<double> > mcsse_q1t_sort2_l8_sainv_f("MGBench mcsse | q1t | sort 2 | L8 | sainv | F", 3 , 2, 8, "sainv", 0.7);

MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::F::V::STATIC, SparseMatrixELL<double> > mcsse_q1t_sort3_l6_sainv_f("MGBench mcsse | q1t | sort 3 | L6 | sainv | F", 3 , 3, 6, "sainv", 0.7);
MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::F::V::STATIC, SparseMatrixELL<double> > mcsse_q1t_sort3_l7_sainv_f("MGBench mcsse | q1t | sort 3 | L7 | sainv | F", 3 , 3, 7, "sainv", 0.7);
MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::F::V::STATIC, SparseMatrixELL<double> > mcsse_q1t_sort3_l8_sainv_f("MGBench mcsse | q1t | sort 3 | L8 | sainv | F", 3 , 3, 8, "sainv", 0.7);

MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::F::V::STATIC, SparseMatrixELL<double> > mcsse_q1t_sort4_l6_sainv_f("MGBench mcsse | q1t | sort 4 | L6 | sainv | F", 3 , 4, 6, "sainv", 0.7);
MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::F::V::STATIC, SparseMatrixELL<double> > mcsse_q1t_sort4_l7_sainv_f("MGBench mcsse | q1t | sort 4 | L7 | sainv | F", 3 , 4, 7, "sainv", 0.7);
MGBench<tags::CPU::MultiCore::SSE, methods::CYCLE::F::V::STATIC, SparseMatrixELL<double> > mcsse_q1t_sort4_l8_sainv_f("MGBench mcsse | q1t | sort 4 | L8 | sainv | F", 3 , 4, 8, "sainv", 0.7);
#endif

#if defined HONEI_CUDA && defined HONEI_CUDA_DOUBLE
//JAC
//q1
MGBench<tags::GPU::CUDA, methods::CYCLE::F::V::STATIC, DenseVector<double> > cuda_q1_sort0_l7_jac_f("MGBench cuda | q1 | sort 0 | L7 | jac | F", 1 , 0, 7, "jac", 0.7);
MGBench<tags::GPU::CUDA, methods::CYCLE::F::V::STATIC, DenseVector<double> > cuda_q1_sort0_l8_jac_f("MGBench cuda | q1 | sort 0 | L8 | jac | F", 1 , 0, 8, "jac", 0.7);
MGBench<tags::GPU::CUDA, methods::CYCLE::F::V::STATIC, DenseVector<double> > cuda_q1_sort0_l9_jac_f("MGBench cuda | q1 | sort 0 | L9 | jac | F", 1 , 0, 9, "jac", 0.7);


MGBench<tags::GPU::CUDA, methods::CYCLE::F::V::STATIC, DenseVector<double> > cuda_q1_sort1_l7_jac_f("MGBench cuda | q1 | sort 1 | L7 | jac | F", 1 , 1, 7, "jac", 0.7);
MGBench<tags::GPU::CUDA, methods::CYCLE::F::V::STATIC, DenseVector<double> > cuda_q1_sort1_l8_jac_f("MGBench cuda | q1 | sort 1 | L8 | jac | F", 1 , 1, 8, "jac", 0.7);
MGBench<tags::GPU::CUDA, methods::CYCLE::F::V::STATIC, DenseVector<double> > cuda_q1_sort1_l9_jac_f("MGBench cuda | q1 | sort 1 | L9 | jac | F", 1 , 1, 9, "jac", 0.7);


MGBench<tags::GPU::CUDA, methods::CYCLE::F::V::STATIC, DenseVector<double> > cuda_q1_sort2_l7_jac_f("MGBench cuda | q1 | sort 2 | L7 | jac | F", 1 , 2, 7, "jac", 0.7);
MGBench<tags::GPU::CUDA, methods::CYCLE::F::V::STATIC, DenseVector<double> > cuda_q1_sort2_l8_jac_f("MGBench cuda | q1 | sort 2 | L8 | jac | F", 1 , 2, 8, "jac", 0.7);
MGBench<tags::GPU::CUDA, methods::CYCLE::F::V::STATIC, DenseVector<double> > cuda_q1_sort2_l9_jac_f("MGBench cuda | q1 | sort 2 | L9 | jac | F", 1 , 2, 9, "jac", 0.7);

MGBench<tags::GPU::CUDA, methods::CYCLE::F::V::STATIC, DenseVector<double> > cuda_q1_sort3_l7_jac_f("MGBench cuda | q1 | sort 3 | L7 | jac | F", 1 , 3, 7, "jac", 0.7);
MGBench<tags::GPU::CUDA, methods::CYCLE::F::V::STATIC, DenseVector<double> > cuda_q1_sort3_l8_jac_f("MGBench cuda | q1 | sort 3 | L8 | jac | F", 1 , 3, 8, "jac", 0.7);
MGBench<tags::GPU::CUDA, methods::CYCLE::F::V::STATIC, DenseVector<double> > cuda_q1_sort3_l9_jac_f("MGBench cuda | q1 | sort 3 | L9 | jac | F", 1 , 3, 9, "jac", 0.7);

MGBench<tags::GPU::CUDA, methods::CYCLE::F::V::STATIC, DenseVector<double> > cuda_q1_sort4_l7_jac_f("MGBench cuda | q1 | sort 4 | L7 | jac | F", 1 , 4, 7, "jac", 0.7);
MGBench<tags::GPU::CUDA, methods::CYCLE::F::V::STATIC, DenseVector<double> > cuda_q1_sort4_l8_jac_f("MGBench cuda | q1 | sort 4 | L8 | jac | F", 1 , 4, 8, "jac", 0.7);
MGBench<tags::GPU::CUDA, methods::CYCLE::F::V::STATIC, DenseVector<double> > cuda_q1_sort4_l9_jac_f("MGBench cuda | q1 | sort 4 | L9 | jac | F", 1 , 4, 9, "jac", 0.7);

//q2
MGBench<tags::GPU::CUDA, methods::CYCLE::F::V::STATIC, DenseVector<double> > cuda_q2_sort0_l6_jac_f("MGBench cuda | q2 | sort 0 | L6 | jac | F", 2 , 0, 6, "jac", 0.7);
MGBench<tags::GPU::CUDA, methods::CYCLE::F::V::STATIC, DenseVector<double> > cuda_q2_sort0_l7_jac_f("MGBench cuda | q2 | sort 0 | L7 | jac | F", 2 , 0, 7, "jac", 0.7);
MGBench<tags::GPU::CUDA, methods::CYCLE::F::V::STATIC, DenseVector<double> > cuda_q2_sort0_l8_jac_f("MGBench cuda | q2 | sort 0 | L8 | jac | F", 2 , 0, 8, "jac", 0.7);


MGBench<tags::GPU::CUDA, methods::CYCLE::F::V::STATIC, DenseVector<double> > cuda_q2_sort1_l6_jac_f("MGBench cuda | q2 | sort 1 | L6 | jac | F", 2 , 1, 6, "jac", 0.7);
MGBench<tags::GPU::CUDA, methods::CYCLE::F::V::STATIC, DenseVector<double> > cuda_q2_sort1_l7_jac_f("MGBench cuda | q2 | sort 1 | L7 | jac | F", 2 , 1, 7, "jac", 0.7);
MGBench<tags::GPU::CUDA, methods::CYCLE::F::V::STATIC, DenseVector<double> > cuda_q2_sort1_l8_jac_f("MGBench cuda | q2 | sort 1 | L8 | jac | F", 2 , 1, 8, "jac", 0.7);


MGBench<tags::GPU::CUDA, methods::CYCLE::F::V::STATIC, DenseVector<double> > cuda_q2_sort2_l6_jac_f("MGBench cuda | q2 | sort 2 | L6 | jac | F", 2 , 2, 6, "jac", 0.7);
MGBench<tags::GPU::CUDA, methods::CYCLE::F::V::STATIC, DenseVector<double> > cuda_q2_sort2_l7_jac_f("MGBench cuda | q2 | sort 2 | L7 | jac | F", 2 , 2, 7, "jac", 0.7);
MGBench<tags::GPU::CUDA, methods::CYCLE::F::V::STATIC, DenseVector<double> > cuda_q2_sort2_l8_jac_f("MGBench cuda | q2 | sort 2 | L8 | jac | F", 2 , 2, 8, "jac", 0.7);

MGBench<tags::GPU::CUDA, methods::CYCLE::F::V::STATIC, DenseVector<double> > cuda_q2_sort3_l6_jac_f("MGBench cuda | q2 | sort 3 | L6 | jac | F", 2 , 3, 6, "jac", 0.7);
MGBench<tags::GPU::CUDA, methods::CYCLE::F::V::STATIC, DenseVector<double> > cuda_q2_sort3_l7_jac_f("MGBench cuda | q2 | sort 3 | L7 | jac | F", 2 , 3, 7, "jac", 0.7);
MGBench<tags::GPU::CUDA, methods::CYCLE::F::V::STATIC, DenseVector<double> > cuda_q2_sort3_l8_jac_f("MGBench cuda | q2 | sort 3 | L8 | jac | F", 2 , 3, 8, "jac", 0.7);

MGBench<tags::GPU::CUDA, methods::CYCLE::F::V::STATIC, DenseVector<double> > cuda_q2_sort4_l6_jac_f("MGBench cuda | q2 | sort 4 | L6 | jac | F", 2 , 4, 6, "jac", 0.7);
MGBench<tags::GPU::CUDA, methods::CYCLE::F::V::STATIC, DenseVector<double> > cuda_q2_sort4_l7_jac_f("MGBench cuda | q2 | sort 4 | L7 | jac | F", 2 , 4, 7, "jac", 0.7);
MGBench<tags::GPU::CUDA, methods::CYCLE::F::V::STATIC, DenseVector<double> > cuda_q2_sort4_l8_jac_f("MGBench cuda | q2 | sort 4 | L8 | jac | F", 2 , 4, 8, "jac", 0.7);

//q1t
MGBench<tags::GPU::CUDA, methods::CYCLE::F::V::STATIC, DenseVector<double> > cuda_q1t_sort0_l6_jac_f("MGBench cuda | q1t | sort 0 | L6 | jac | F", 3 , 0, 6, "jac", 0.7);
MGBench<tags::GPU::CUDA, methods::CYCLE::F::V::STATIC, DenseVector<double> > cuda_q1t_sort0_l7_jac_f("MGBench cuda | q1t | sort 0 | L7 | jac | F", 3 , 0, 7, "jac", 0.7);
MGBench<tags::GPU::CUDA, methods::CYCLE::F::V::STATIC, DenseVector<double> > cuda_q1t_sort0_l8_jac_f("MGBench cuda | q1t | sort 0 | L8 | jac | F", 3 , 0, 8, "jac", 0.7);


MGBench<tags::GPU::CUDA, methods::CYCLE::F::V::STATIC, DenseVector<double> > cuda_q1t_sort1_l6_jac_f("MGBench cuda | q1t | sort 1 | L6 | jac | F", 3 , 1, 6, "jac", 0.7);
MGBench<tags::GPU::CUDA, methods::CYCLE::F::V::STATIC, DenseVector<double> > cuda_q1t_sort1_l7_jac_f("MGBench cuda | q1t | sort 1 | L7 | jac | F", 3 , 1, 7, "jac", 0.7);
MGBench<tags::GPU::CUDA, methods::CYCLE::F::V::STATIC, DenseVector<double> > cuda_q1t_sort1_l8_jac_f("MGBench cuda | q1t | sort 1 | L8 | jac | F", 3 , 1, 8, "jac", 0.7);


MGBench<tags::GPU::CUDA, methods::CYCLE::F::V::STATIC, DenseVector<double> > cuda_q1t_sort2_l6_jac_f("MGBench cuda | q1t | sort 2 | L6 | jac | F", 3 , 2, 6, "jac", 0.7);
MGBench<tags::GPU::CUDA, methods::CYCLE::F::V::STATIC, DenseVector<double> > cuda_q1t_sort2_l7_jac_f("MGBench cuda | q1t | sort 2 | L7 | jac | F", 3 , 2, 7, "jac", 0.7);
MGBench<tags::GPU::CUDA, methods::CYCLE::F::V::STATIC, DenseVector<double> > cuda_q1t_sort2_l8_jac_f("MGBench cuda | q1t | sort 2 | L8 | jac | F", 3 , 2, 8, "jac", 0.7);

MGBench<tags::GPU::CUDA, methods::CYCLE::F::V::STATIC, DenseVector<double> > cuda_q1t_sort3_l6_jac_f("MGBench cuda | q1t | sort 3 | L6 | jac | F", 3 , 3, 6, "jac", 0.7);
MGBench<tags::GPU::CUDA, methods::CYCLE::F::V::STATIC, DenseVector<double> > cuda_q1t_sort3_l7_jac_f("MGBench cuda | q1t | sort 3 | L7 | jac | F", 3 , 3, 7, "jac", 0.7);
MGBench<tags::GPU::CUDA, methods::CYCLE::F::V::STATIC, DenseVector<double> > cuda_q1t_sort3_l8_jac_f("MGBench cuda | q1t | sort 3 | L8 | jac | F", 3 , 3, 8, "jac", 0.7);

MGBench<tags::GPU::CUDA, methods::CYCLE::F::V::STATIC, DenseVector<double> > cuda_q1t_sort4_l6_jac_f("MGBench cuda | q1t | sort 4 | L6 | jac | F", 3 , 4, 6, "jac", 0.7);
MGBench<tags::GPU::CUDA, methods::CYCLE::F::V::STATIC, DenseVector<double> > cuda_q1t_sort4_l7_jac_f("MGBench cuda | q1t | sort 4 | L7 | jac | F", 3 , 4, 7, "jac", 0.7);
MGBench<tags::GPU::CUDA, methods::CYCLE::F::V::STATIC, DenseVector<double> > cuda_q1t_sort4_l8_jac_f("MGBench cuda | q1t | sort 4 | L8 | jac | F", 3 , 4, 8, "jac", 0.7);

//SPAI
//q1
MGBench<tags::GPU::CUDA, methods::CYCLE::F::V::STATIC, SparseMatrixELL<double> > cuda_q1_sort0_l7_spai_grote_f("MGBench cuda | q1 | sort 0 | L7 | spai_grote | F", 1 , 0, 7, "spai_grote", 1.);
MGBench<tags::GPU::CUDA, methods::CYCLE::F::V::STATIC, SparseMatrixELL<double> > cuda_q1_sort0_l8_spai_grote_f("MGBench cuda | q1 | sort 0 | L8 | spai_grote | F", 1 , 0, 8, "spai_grote", 1.);
MGBench<tags::GPU::CUDA, methods::CYCLE::F::V::STATIC, SparseMatrixELL<double> > cuda_q1_sort0_l9_spai_grote_f("MGBench cuda | q1 | sort 0 | L9 | spai_grote | F", 1 , 0, 9, "spai_grote", 1.);


MGBench<tags::GPU::CUDA, methods::CYCLE::F::V::STATIC, SparseMatrixELL<double> > cuda_q1_sort1_l7_spai_grote_f("MGBench cuda | q1 | sort 1 | L7 | spai_grote | F", 1 , 1, 7, "spai_grote", 1.);
MGBench<tags::GPU::CUDA, methods::CYCLE::F::V::STATIC, SparseMatrixELL<double> > cuda_q1_sort1_l8_spai_grote_f("MGBench cuda | q1 | sort 1 | L8 | spai_grote | F", 1 , 1, 8, "spai_grote", 1.);
MGBench<tags::GPU::CUDA, methods::CYCLE::F::V::STATIC, SparseMatrixELL<double> > cuda_q1_sort1_l9_spai_grote_f("MGBench cuda | q1 | sort 1 | L9 | spai_grote | F", 1 , 1, 9, "spai_grote", 1.);


MGBench<tags::GPU::CUDA, methods::CYCLE::F::V::STATIC, SparseMatrixELL<double> > cuda_q1_sort2_l7_spai_grote_f("MGBench cuda | q1 | sort 2 | L7 | spai_grote | F", 1 , 2, 7, "spai_grote", 1.);
MGBench<tags::GPU::CUDA, methods::CYCLE::F::V::STATIC, SparseMatrixELL<double> > cuda_q1_sort2_l8_spai_grote_f("MGBench cuda | q1 | sort 2 | L8 | spai_grote | F", 1 , 2, 8, "spai_grote", 1.);
MGBench<tags::GPU::CUDA, methods::CYCLE::F::V::STATIC, SparseMatrixELL<double> > cuda_q1_sort2_l9_spai_grote_f("MGBench cuda | q1 | sort 2 | L9 | spai_grote | F", 1 , 2, 9, "spai_grote", 1.);

MGBench<tags::GPU::CUDA, methods::CYCLE::F::V::STATIC, SparseMatrixELL<double> > cuda_q1_sort3_l7_spai_grote_f("MGBench cuda | q1 | sort 3 | L7 | spai_grote | F", 1 , 3, 7, "spai_grote", 1.);
MGBench<tags::GPU::CUDA, methods::CYCLE::F::V::STATIC, SparseMatrixELL<double> > cuda_q1_sort3_l8_spai_grote_f("MGBench cuda | q1 | sort 3 | L8 | spai_grote | F", 1 , 3, 8, "spai_grote", 1.);
MGBench<tags::GPU::CUDA, methods::CYCLE::F::V::STATIC, SparseMatrixELL<double> > cuda_q1_sort3_l9_spai_grote_f("MGBench cuda | q1 | sort 3 | L9 | spai_grote | F", 1 , 3, 9, "spai_grote", 1.);

MGBench<tags::GPU::CUDA, methods::CYCLE::F::V::STATIC, SparseMatrixELL<double> > cuda_q1_sort4_l7_spai_grote_f("MGBench cuda | q1 | sort 4 | L7 | spai_grote | F", 1 , 4, 7, "spai_grote", 1.);
MGBench<tags::GPU::CUDA, methods::CYCLE::F::V::STATIC, SparseMatrixELL<double> > cuda_q1_sort4_l8_spai_grote_f("MGBench cuda | q1 | sort 4 | L8 | spai_grote | F", 1 , 4, 8, "spai_grote", 1.);
MGBench<tags::GPU::CUDA, methods::CYCLE::F::V::STATIC, SparseMatrixELL<double> > cuda_q1_sort4_l9_spai_grote_f("MGBench cuda | q1 | sort 4 | L9 | spai_grote | F", 1 , 4, 9, "spai_grote", 1.);

//q2
MGBench<tags::GPU::CUDA, methods::CYCLE::F::V::STATIC, SparseMatrixELL<double> > cuda_q2_sort0_l6_spai_grote_f("MGBench cuda | q2 | sort 0 | L6 | spai_grote | F", 2 , 0, 6, "spai_grote", 1.);
MGBench<tags::GPU::CUDA, methods::CYCLE::F::V::STATIC, SparseMatrixELL<double> > cuda_q2_sort0_l7_spai_grote_f("MGBench cuda | q2 | sort 0 | L7 | spai_grote | F", 2 , 0, 7, "spai_grote", 1.);
MGBench<tags::GPU::CUDA, methods::CYCLE::F::V::STATIC, SparseMatrixELL<double> > cuda_q2_sort0_l8_spai_grote_f("MGBench cuda | q2 | sort 0 | L8 | spai_grote | F", 2 , 0, 8, "spai_grote", 1.);


MGBench<tags::GPU::CUDA, methods::CYCLE::F::V::STATIC, SparseMatrixELL<double> > cuda_q2_sort1_l6_spai_grote_f("MGBench cuda | q2 | sort 1 | L6 | spai_grote | F", 2 , 1, 6, "spai_grote", 1.);
MGBench<tags::GPU::CUDA, methods::CYCLE::F::V::STATIC, SparseMatrixELL<double> > cuda_q2_sort1_l7_spai_grote_f("MGBench cuda | q2 | sort 1 | L7 | spai_grote | F", 2 , 1, 7, "spai_grote", 1.);
MGBench<tags::GPU::CUDA, methods::CYCLE::F::V::STATIC, SparseMatrixELL<double> > cuda_q2_sort1_l8_spai_grote_f("MGBench cuda | q2 | sort 1 | L8 | spai_grote | F", 2 , 1, 8, "spai_grote", 1.);

MGBench<tags::GPU::CUDA, methods::CYCLE::F::V::STATIC, SparseMatrixELL<double> > cuda_q2_sort2_l6_spai_grote_f("MGBench cuda | q2 | sort 2 | L6 | spai_grote | F", 2 , 1, 6, "spai_grote", 1.);
MGBench<tags::GPU::CUDA, methods::CYCLE::F::V::STATIC, SparseMatrixELL<double> > cuda_q2_sort2_l7_spai_grote_f("MGBench cuda | q2 | sort 2 | L7 | spai_grote | F", 2 , 1, 7, "spai_grote", 1.);
MGBench<tags::GPU::CUDA, methods::CYCLE::F::V::STATIC, SparseMatrixELL<double> > cuda_q2_sort2_l8_spai_grote_f("MGBench cuda | q2 | sort 2 | L8 | spai_grote | F", 2 , 1, 8, "spai_grote", 1.);

MGBench<tags::GPU::CUDA, methods::CYCLE::F::V::STATIC, SparseMatrixELL<double> > cuda_q2_sort3_l6_spai_grote_f("MGBench cuda | q2 | sort 3 | L6 | spai_grote | F", 2 , 3, 6, "spai_grote", 1.);
MGBench<tags::GPU::CUDA, methods::CYCLE::F::V::STATIC, SparseMatrixELL<double> > cuda_q2_sort3_l7_spai_grote_f("MGBench cuda | q2 | sort 3 | L7 | spai_grote | F", 2 , 3, 7, "spai_grote", 1.);
MGBench<tags::GPU::CUDA, methods::CYCLE::F::V::STATIC, SparseMatrixELL<double> > cuda_q2_sort3_l8_spai_grote_f("MGBench cuda | q2 | sort 3 | L8 | spai_grote | F", 2 , 3, 8, "spai_grote", 1.);


MGBench<tags::GPU::CUDA, methods::CYCLE::F::V::STATIC, SparseMatrixELL<double> > cuda_q2_sort4_l6_spai_grote_f("MGBench cuda | q2 | sort 4 | L6 | spai_grote | F", 2 , 4, 6, "spai_grote", 1.);
MGBench<tags::GPU::CUDA, methods::CYCLE::F::V::STATIC, SparseMatrixELL<double> > cuda_q2_sort4_l7_spai_grote_f("MGBench cuda | q2 | sort 4 | L7 | spai_grote | F", 2 , 4, 7, "spai_grote", 1.);
MGBench<tags::GPU::CUDA, methods::CYCLE::F::V::STATIC, SparseMatrixELL<double> > cuda_q2_sort4_l8_spai_grote_f("MGBench cuda | q2 | sort 4 | L8 | spai_grote | F", 2 , 4, 8, "spai_grote", 1.);

//q1t
MGBench<tags::GPU::CUDA, methods::CYCLE::F::V::STATIC, SparseMatrixELL<double> > cuda_q1t_sort0_l6_spai_grote_f("MGBench cuda | q1t | sort 0 | L6 | spai_grote | F", 3 , 0, 6, "spai_grote", 1.);
MGBench<tags::GPU::CUDA, methods::CYCLE::F::V::STATIC, SparseMatrixELL<double> > cuda_q1t_sort0_l7_spai_grote_f("MGBench cuda | q1t | sort 0 | L7 | spai_grote | F", 3 , 0, 7, "spai_grote", 1.);
MGBench<tags::GPU::CUDA, methods::CYCLE::F::V::STATIC, SparseMatrixELL<double> > cuda_q1t_sort0_l8_spai_grote_f("MGBench cuda | q1t | sort 0 | L8 | spai_grote | F", 3 , 0, 8, "spai_grote", 1.);


MGBench<tags::GPU::CUDA, methods::CYCLE::F::V::STATIC, SparseMatrixELL<double> > cuda_q1t_sort1_l6_spai_grote_f("MGBench cuda | q1t | sort 1 | L6 | spai_grote | F", 3 , 1, 6, "spai_grote", 1.);
MGBench<tags::GPU::CUDA, methods::CYCLE::F::V::STATIC, SparseMatrixELL<double> > cuda_q1t_sort1_l7_spai_grote_f("MGBench cuda | q1t | sort 1 | L7 | spai_grote | F", 3 , 1, 7, "spai_grote", 1.);
MGBench<tags::GPU::CUDA, methods::CYCLE::F::V::STATIC, SparseMatrixELL<double> > cuda_q1t_sort1_l8_spai_grote_f("MGBench cuda | q1t | sort 1 | L8 | spai_grote | F", 3 , 1, 8, "spai_grote", 1.);

MGBench<tags::GPU::CUDA, methods::CYCLE::F::V::STATIC, SparseMatrixELL<double> > cuda_q1t_sort2_l6_spai_grote_f("MGBench cuda | q1t | sort 2 | L6 | spai_grote | F", 3 , 1, 6, "spai_grote", 1.);
MGBench<tags::GPU::CUDA, methods::CYCLE::F::V::STATIC, SparseMatrixELL<double> > cuda_q1t_sort2_l7_spai_grote_f("MGBench cuda | q1t | sort 2 | L7 | spai_grote | F", 3 , 1, 7, "spai_grote", 1.);
MGBench<tags::GPU::CUDA, methods::CYCLE::F::V::STATIC, SparseMatrixELL<double> > cuda_q1t_sort2_l8_spai_grote_f("MGBench cuda | q1t | sort 2 | L8 | spai_grote | F", 3 , 1, 8, "spai_grote", 1.);

MGBench<tags::GPU::CUDA, methods::CYCLE::F::V::STATIC, SparseMatrixELL<double> > cuda_q1t_sort3_l6_spai_grote_f("MGBench cuda | q1t | sort 3 | L6 | spai_grote | F", 3 , 3, 6, "spai_grote", 1.);
MGBench<tags::GPU::CUDA, methods::CYCLE::F::V::STATIC, SparseMatrixELL<double> > cuda_q1t_sort3_l7_spai_grote_f("MGBench cuda | q1t | sort 3 | L7 | spai_grote | F", 3 , 3, 7, "spai_grote", 1.);
MGBench<tags::GPU::CUDA, methods::CYCLE::F::V::STATIC, SparseMatrixELL<double> > cuda_q1t_sort3_l8_spai_grote_f("MGBench cuda | q1t | sort 3 | L8 | spai_grote | F", 3 , 3, 8, "spai_grote", 1.);


MGBench<tags::GPU::CUDA, methods::CYCLE::F::V::STATIC, SparseMatrixELL<double> > cuda_q1t_sort4_l6_spai_grote_f("MGBench cuda | q1t | sort 4 | L6 | spai_grote | F", 3 , 4, 6, "spai_grote", 1.);
MGBench<tags::GPU::CUDA, methods::CYCLE::F::V::STATIC, SparseMatrixELL<double> > cuda_q1t_sort4_l7_spai_grote_f("MGBench cuda | q1t | sort 4 | L7 | spai_grote | F", 3 , 4, 7, "spai_grote", 1.);
MGBench<tags::GPU::CUDA, methods::CYCLE::F::V::STATIC, SparseMatrixELL<double> > cuda_q1t_sort4_l8_spai_grote_f("MGBench cuda | q1t | sort 4 | L8 | spai_grote | F", 3 , 4, 8, "spai_grote", 1.);
//SAINV
//q1
MGBench<tags::GPU::CUDA, methods::CYCLE::F::V::STATIC, SparseMatrixELL<double> > cuda_q1_sort0_l7_sainv_f("MGBench cuda | q1 | sort 0 | L7 | sainv | F", 1 , 0, 7, "sainv", 0.7);
MGBench<tags::GPU::CUDA, methods::CYCLE::F::V::STATIC, SparseMatrixELL<double> > cuda_q1_sort0_l8_sainv_f("MGBench cuda | q1 | sort 0 | L8 | sainv | F", 1 , 0, 8, "sainv", 0.7);
MGBench<tags::GPU::CUDA, methods::CYCLE::F::V::STATIC, SparseMatrixELL<double> > cuda_q1_sort0_l9_sainv_f("MGBench cuda | q1 | sort 0 | L9 | sainv | F", 1 , 0, 9, "sainv", 0.7);


MGBench<tags::GPU::CUDA, methods::CYCLE::F::V::STATIC, SparseMatrixELL<double> > cuda_q1_sort1_l7_sainv_f("MGBench cuda | q1 | sort 1 | L7 | sainv | F", 1 , 1, 7, "sainv", 0.7);
MGBench<tags::GPU::CUDA, methods::CYCLE::F::V::STATIC, SparseMatrixELL<double> > cuda_q1_sort1_l8_sainv_f("MGBench cuda | q1 | sort 1 | L8 | sainv | F", 1 , 1, 8, "sainv", 0.7);
MGBench<tags::GPU::CUDA, methods::CYCLE::F::V::STATIC, SparseMatrixELL<double> > cuda_q1_sort1_l9_sainv_f("MGBench cuda | q1 | sort 1 | L9 | sainv | F", 1 , 1, 9, "sainv", 0.7);


MGBench<tags::GPU::CUDA, methods::CYCLE::F::V::STATIC, SparseMatrixELL<double> > cuda_q1_sort2_l7_sainv_f("MGBench cuda | q1 | sort 2 | L7 | sainv | F", 1 , 2, 7, "sainv", 0.7);
MGBench<tags::GPU::CUDA, methods::CYCLE::F::V::STATIC, SparseMatrixELL<double> > cuda_q1_sort2_l8_sainv_f("MGBench cuda | q1 | sort 2 | L8 | sainv | F", 1 , 2, 8, "sainv", 0.7);
MGBench<tags::GPU::CUDA, methods::CYCLE::F::V::STATIC, SparseMatrixELL<double> > cuda_q1_sort2_l9_sainv_f("MGBench cuda | q1 | sort 2 | L9 | sainv | F", 1 , 2, 9, "sainv", 0.7);

MGBench<tags::GPU::CUDA, methods::CYCLE::F::V::STATIC, SparseMatrixELL<double> > cuda_q1_sort3_l7_sainv_f("MGBench cuda | q1 | sort 3 | L7 | sainv | F", 1 , 3, 7, "sainv", 0.7);
MGBench<tags::GPU::CUDA, methods::CYCLE::F::V::STATIC, SparseMatrixELL<double> > cuda_q1_sort3_l8_sainv_f("MGBench cuda | q1 | sort 3 | L8 | sainv | F", 1 , 3, 8, "sainv", 0.7);
MGBench<tags::GPU::CUDA, methods::CYCLE::F::V::STATIC, SparseMatrixELL<double> > cuda_q1_sort3_l9_sainv_f("MGBench cuda | q1 | sort 3 | L9 | sainv | F", 1 , 3, 9, "sainv", 0.7);

MGBench<tags::GPU::CUDA, methods::CYCLE::F::V::STATIC, SparseMatrixELL<double> > cuda_q1_sort4_l7_sainv_f("MGBench cuda | q1 | sort 4 | L7 | sainv | F", 1 , 4, 7, "sainv", 0.7);
MGBench<tags::GPU::CUDA, methods::CYCLE::F::V::STATIC, SparseMatrixELL<double> > cuda_q1_sort4_l8_sainv_f("MGBench cuda | q1 | sort 4 | L8 | sainv | F", 1 , 4, 8, "sainv", 0.7);
MGBench<tags::GPU::CUDA, methods::CYCLE::F::V::STATIC, SparseMatrixELL<double> > cuda_q1_sort4_l9_sainv_f("MGBench cuda | q1 | sort 4 | L9 | sainv | F", 1 , 4, 9, "sainv", 0.7);

//q2
MGBench<tags::GPU::CUDA, methods::CYCLE::F::V::STATIC, SparseMatrixELL<double> > cuda_q2_sort0_l6_sainv_f("MGBench cuda | q2 | sort 0 | L6 | sainv | F", 2 , 0, 6, "sainv", 0.7);
MGBench<tags::GPU::CUDA, methods::CYCLE::F::V::STATIC, SparseMatrixELL<double> > cuda_q2_sort0_l7_sainv_f("MGBench cuda | q2 | sort 0 | L7 | sainv | F", 2 , 0, 7, "sainv", 0.7);
MGBench<tags::GPU::CUDA, methods::CYCLE::F::V::STATIC, SparseMatrixELL<double> > cuda_q2_sort0_l8_sainv_f("MGBench cuda | q2 | sort 0 | L8 | sainv | F", 2 , 0, 8, "sainv", 0.7);


MGBench<tags::GPU::CUDA, methods::CYCLE::F::V::STATIC, SparseMatrixELL<double> > cuda_q2_sort1_l6_sainv_f("MGBench cuda | q2 | sort 1 | L6 | sainv | F", 2 , 1, 6, "sainv", 0.7);
MGBench<tags::GPU::CUDA, methods::CYCLE::F::V::STATIC, SparseMatrixELL<double> > cuda_q2_sort1_l7_sainv_f("MGBench cuda | q2 | sort 1 | L7 | sainv | F", 2 , 1, 7, "sainv", 0.7);
MGBench<tags::GPU::CUDA, methods::CYCLE::F::V::STATIC, SparseMatrixELL<double> > cuda_q2_sort1_l8_sainv_f("MGBench cuda | q2 | sort 1 | L8 | sainv | F", 2 , 1, 8, "sainv", 0.7);


MGBench<tags::GPU::CUDA, methods::CYCLE::F::V::STATIC, SparseMatrixELL<double> > cuda_q2_sort2_l6_sainv_f("MGBench cuda | q2 | sort 2 | L6 | sainv | F", 2 , 2, 6, "sainv", 0.7);
MGBench<tags::GPU::CUDA, methods::CYCLE::F::V::STATIC, SparseMatrixELL<double> > cuda_q2_sort2_l7_sainv_f("MGBench cuda | q2 | sort 2 | L7 | sainv | F", 2 , 2, 7, "sainv", 0.7);
MGBench<tags::GPU::CUDA, methods::CYCLE::F::V::STATIC, SparseMatrixELL<double> > cuda_q2_sort2_l8_sainv_f("MGBench cuda | q2 | sort 2 | L8 | sainv | F", 2 , 2, 8, "sainv", 0.7);

MGBench<tags::GPU::CUDA, methods::CYCLE::F::V::STATIC, SparseMatrixELL<double> > cuda_q2_sort3_l6_sainv_f("MGBench cuda | q2 | sort 3 | L6 | sainv | F", 2 , 3, 6, "sainv", 0.7);
MGBench<tags::GPU::CUDA, methods::CYCLE::F::V::STATIC, SparseMatrixELL<double> > cuda_q2_sort3_l7_sainv_f("MGBench cuda | q2 | sort 3 | L7 | sainv | F", 2 , 3, 7, "sainv", 0.7);
MGBench<tags::GPU::CUDA, methods::CYCLE::F::V::STATIC, SparseMatrixELL<double> > cuda_q2_sort3_l8_sainv_f("MGBench cuda | q2 | sort 3 | L8 | sainv | F", 2 , 3, 8, "sainv", 0.7);


MGBench<tags::GPU::CUDA, methods::CYCLE::F::V::STATIC, SparseMatrixELL<double> > cuda_q2_sort4_l6_sainv_f("MGBench cuda | q2 | sort 4 | L6 | sainv | F", 2 , 4, 6, "sainv", 0.7);
MGBench<tags::GPU::CUDA, methods::CYCLE::F::V::STATIC, SparseMatrixELL<double> > cuda_q2_sort4_l7_sainv_f("MGBench cuda | q2 | sort 4 | L7 | sainv | F", 2 , 4, 7, "sainv", 0.7);
MGBench<tags::GPU::CUDA, methods::CYCLE::F::V::STATIC, SparseMatrixELL<double> > cuda_q2_sort4_l8_sainv_f("MGBench cuda | q2 | sort 4 | L8 | sainv | F", 2 , 4, 8, "sainv", 0.7);

//q1t
MGBench<tags::GPU::CUDA, methods::CYCLE::F::V::STATIC, SparseMatrixELL<double> > cuda_q1t_sort0_l6_sainv_f("MGBench cuda | q1t | sort 0 | L6 | sainv | F", 3 , 0, 6, "sainv", 0.7);
MGBench<tags::GPU::CUDA, methods::CYCLE::F::V::STATIC, SparseMatrixELL<double> > cuda_q1t_sort0_l7_sainv_f("MGBench cuda | q1t | sort 0 | L7 | sainv | F", 3 , 0, 7, "sainv", 0.7);
MGBench<tags::GPU::CUDA, methods::CYCLE::F::V::STATIC, SparseMatrixELL<double> > cuda_q1t_sort0_l8_sainv_f("MGBench cuda | q1t | sort 0 | L8 | sainv | F", 3 , 0, 8, "sainv", 0.7);


MGBench<tags::GPU::CUDA, methods::CYCLE::F::V::STATIC, SparseMatrixELL<double> > cuda_q1t_sort1_l6_sainv_f("MGBench cuda | q1t | sort 1 | L6 | sainv | F", 3 , 1, 6, "sainv", 0.7);
MGBench<tags::GPU::CUDA, methods::CYCLE::F::V::STATIC, SparseMatrixELL<double> > cuda_q1t_sort1_l7_sainv_f("MGBench cuda | q1t | sort 1 | L7 | sainv | F", 3 , 1, 7, "sainv", 0.7);
MGBench<tags::GPU::CUDA, methods::CYCLE::F::V::STATIC, SparseMatrixELL<double> > cuda_q1t_sort1_l8_sainv_f("MGBench cuda | q1t | sort 1 | L8 | sainv | F", 3 , 1, 8, "sainv", 0.7);


MGBench<tags::GPU::CUDA, methods::CYCLE::F::V::STATIC, SparseMatrixELL<double> > cuda_q1t_sort2_l6_sainv_f("MGBench cuda | q1t | sort 2 | L6 | sainv | F", 3 , 2, 6, "sainv", 0.7);
MGBench<tags::GPU::CUDA, methods::CYCLE::F::V::STATIC, SparseMatrixELL<double> > cuda_q1t_sort2_l7_sainv_f("MGBench cuda | q1t | sort 2 | L7 | sainv | F", 3 , 2, 7, "sainv", 0.7);
MGBench<tags::GPU::CUDA, methods::CYCLE::F::V::STATIC, SparseMatrixELL<double> > cuda_q1t_sort2_l8_sainv_f("MGBench cuda | q1t | sort 2 | L8 | sainv | F", 3 , 2, 8, "sainv", 0.7);

MGBench<tags::GPU::CUDA, methods::CYCLE::F::V::STATIC, SparseMatrixELL<double> > cuda_q1t_sort3_l6_sainv_f("MGBench cuda | q1t | sort 3 | L6 | sainv | F", 3 , 3, 6, "sainv", 0.7);
MGBench<tags::GPU::CUDA, methods::CYCLE::F::V::STATIC, SparseMatrixELL<double> > cuda_q1t_sort3_l7_sainv_f("MGBench cuda | q1t | sort 3 | L7 | sainv | F", 3 , 3, 7, "sainv", 0.7);
MGBench<tags::GPU::CUDA, methods::CYCLE::F::V::STATIC, SparseMatrixELL<double> > cuda_q1t_sort3_l8_sainv_f("MGBench cuda | q1t | sort 3 | L8 | sainv | F", 3 , 3, 8, "sainv", 0.7);


MGBench<tags::GPU::CUDA, methods::CYCLE::F::V::STATIC, SparseMatrixELL<double> > cuda_q1t_sort4_l6_sainv_f("MGBench cuda | q1t | sort 4 | L6 | sainv | F", 3 , 4, 6, "sainv", 0.7);
MGBench<tags::GPU::CUDA, methods::CYCLE::F::V::STATIC, SparseMatrixELL<double> > cuda_q1t_sort4_l7_sainv_f("MGBench cuda | q1t | sort 4 | L7 | sainv | F", 3 , 4, 7, "sainv", 0.7);
MGBench<tags::GPU::CUDA, methods::CYCLE::F::V::STATIC, SparseMatrixELL<double> > cuda_q1t_sort4_l8_sainv_f("MGBench cuda | q1t | sort 4 | L8 | sainv | F", 3 , 4, 8, "sainv", 0.7);
#endif
