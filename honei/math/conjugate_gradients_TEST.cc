/* vim: set sw=4 sts=4 et foldmethod=syntax : */

/*
 * Copyright (c) 2007 Markus Geveler <apryde@gmx.de>
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

#include <honei/math/conjugate_gradients.hh>
#include <honei/math/matrix_io.hh>
#include <unittest/unittest.hh>
#include <honei/util/stringify.hh>
#include <iostream>

using namespace honei;
using namespace tests;
using namespace std;

template <typename Tag_, typename DT1_>
class ConjugateGradientsTestDense:
    public BaseTest
{
    public:
        ConjugateGradientsTestDense(const std::string & tag) :
            BaseTest("Conjugate gradients solver test (Dense system)<" + tag + ">")
        {
            register_tag(Tag_::name);
        }

        virtual void run() const
        {
            DenseMatrix<DT1_> A(3, 3, DT1_(1));
            DenseVector<DT1_> b(3, DT1_(1));
            A[0][0] = DT1_(7);
            A[0][1] = DT1_(-2);
            A[0][2] = DT1_(0);
            A[1][0] = DT1_(-2);
            A[1][1] = DT1_(6);
            A[1][2] = DT1_(2);
            A[2][0] = DT1_(0);
            A[2][1] = DT1_(2);
            A[2][2] = DT1_(5);

            b[0] = DT1_(3);
            b[1] = DT1_(3);
            b[2] = DT1_(0);

            std::cout<<"A:"<<A<<endl;
            std::cout<<"b:"<<b<<endl;
            DenseVector<DT1_> result = ConjugateGradients<Tag_, methods::NONE>::value(A,b,2ul);
            DT1_ x_n = Norm< vnt_l_two, false, Tag_>::value(result);
            DenseVector<DT1_> x_analytical(3, DT1_(0));
            std::cout << "RESULT(v1):" << result << std::endl;

            x_analytical[0] = DT1_(2./3.);
            x_analytical[1] = DT1_(5./6.);
            x_analytical[2] = DT1_(-1./3.);
            DT1_ x_analytical_n = Norm< vnt_l_two, false, Tag_>::value(x_analytical);
            TEST_CHECK_EQUAL_WITHIN_EPS(x_analytical_n, x_n , double(0.1));

            DenseVector<DT1_> result_2 = ConjugateGradients<tags::CPU, methods::NONE>::value(A,b,double(std::numeric_limits<double>::epsilon()));
            std::cout << "RESULT(v2):" << result_2 << std::endl;

            DT1_ x_n_2 = Norm< vnt_l_two, false, Tag_>::value(result_2);
            TEST_CHECK_EQUAL_WITHIN_EPS(x_analytical_n, x_n_2 , double(0.1));

        }
};

template <typename Tag_, typename DT1_>
class ConjugateGradientsTestBanded:
    public BaseTest
{
    public:
        ConjugateGradientsTestBanded(const std::string & tag) :
            BaseTest("Conjugate gradients solver test (Banded system)<" + tag + ">")
        {
            register_tag(Tag_::name);
        }

        virtual void run() const
        {
            DenseVector<DT1_> diag(3, DT1_(1));
            DenseVector<DT1_> bp1(3, DT1_(1));
            DenseVector<DT1_> bp2(3, DT1_(1));
            DenseVector<DT1_> bm1(3, DT1_(1));
            DenseVector<DT1_> bm2(3, DT1_(1));

            diag[0] = DT1_(7);
            diag[1] = DT1_(6);
            diag[2] = DT1_(5);
            bp1[0] = DT1_(-2);
            bp1[1] = DT1_(2);
            bp1[2] = DT1_(0);
            bp2[0] = DT1_(0);
            bp2[1] = DT1_(0);
            bp2[2] = DT1_(0);
            bm1[0] = DT1_(0);
            bm1[1] = DT1_(-2);
            bm1[2] = DT1_(2);
            bm2[0] = DT1_(0);
            bm2[1] = DT1_(0);
            bm2[2] = DT1_(0);

            BandedMatrix<DT1_> A(3);
            A.insert_band(0, diag);
            A.insert_band(1, bp1);
            A.insert_band(2, bp2);
            A.insert_band(-1, bm1);
            A.insert_band(-2, bm2);

            DenseVector<DT1_> b(3, DT1_(1));
            b[0] = DT1_(3);
            b[1] = DT1_(3);
            b[2] = DT1_(0);

            std::cout << "A:" << A << std::endl;
            std::cout << "b:" << b << std::endl;

            DenseVector<DT1_> result = ConjugateGradients<tags::CPU, methods::NONE>::value(A,b,2ul);
            DT1_ x_n = Norm< vnt_l_two, false, Tag_>::value(result);
            DenseVector<DT1_> x_analytical(3, DT1_(1));
            x_analytical[0] = DT1_(2./3.);
            x_analytical[1] = DT1_(5./6.);
            x_analytical[2] = DT1_(-1./3.);
            DT1_ x_analytical_n = Norm< vnt_l_two, false, Tag_>::value(x_analytical);

            std::cout << "RESULT(v1):" << result << std::endl;
            TEST_CHECK_EQUAL_WITHIN_EPS(x_analytical_n, x_n , double(0.1));

            DenseVector<DT1_> result_2 = ConjugateGradients<tags::CPU, methods::NONE>::value(A,b,double(std::numeric_limits<double>::epsilon()));
            DT1_ x_n_2 = Norm< vnt_l_two, false, Tag_>::value(result_2);
            TEST_CHECK_EQUAL_WITHIN_EPS(x_analytical_n, x_n_2 , double(0.1));

            std::cout << "RESULT(v2):" << result_2 << std::endl;
        }
};

template <typename Tag_, typename DT1_>
class ConjugateGradientsTestDenseJAC:
    public BaseTest
{
    public:
        ConjugateGradientsTestDenseJAC(const std::string & tag) :
            BaseTest("Preconditioned (JAC) Conjugate gradients solver test (Dense system)<" + tag + ">")
        {
            register_tag(Tag_::name);
        }

        virtual void run() const
        {
            DenseMatrix<DT1_> A(3, 3, DT1_(1));
            DenseVector<DT1_> b(3, DT1_(1));
            A[0][0] = DT1_(7);
            A[0][1] = DT1_(-2);
            A[0][2] = DT1_(0);
            A[1][0] = DT1_(-2);
            A[1][1] = DT1_(6);
            A[1][2] = DT1_(2);
            A[2][0] = DT1_(0);
            A[2][1] = DT1_(2);
            A[2][2] = DT1_(5);

            b[0] = DT1_(3);
            b[1] = DT1_(3);
            b[2] = DT1_(0);

            std::cout << "A:" << A << std::endl;
            std::cout << "b:" << b << std::endl;
            //DenseVector<DT1_> result = ConjugateGradients<tags::CPU, methods::JAC>::value(A,b,long(2));
            //DT1_ x_n = Norm< vnt_l_two, false, Tag_>::value(result);
            DenseVector<DT1_> x_analytical(3, DT1_(0));
            //cout<<"RESULT(v1):"<<result<<endl;
            x_analytical[0] = DT1_(2./3.);
            x_analytical[1] = DT1_(5./6.);
            x_analytical[2] = DT1_(-1./3.);
            DT1_ x_analytical_n = Norm< vnt_l_two, false, Tag_>::value(x_analytical);
            //TEST_CHECK_EQUAL_WITHIN_EPS(x_analytical_n, x_n , double(0.1));

            DenseVector<DT1_> result_2 = ConjugateGradients<tags::CPU, methods::JAC>::value(A,b,double(std::numeric_limits<double>::epsilon()));
            std::cout << "RESULT(v2):" << result_2 << std::endl;

            DT1_ x_n_2 = Norm< vnt_l_two, false, Tag_>::value(result_2);
            TEST_CHECK_EQUAL_WITHIN_EPS(x_analytical_n, x_n_2 , double(0.1));

        }
};

template <typename Tag_, typename DT1_>
class ConjugateGradientsTestBandedJAC:
    public BaseTest
{
    public:
        ConjugateGradientsTestBandedJAC(const std::string & tag) :
            BaseTest("Preconditioned (Jacobi) Conjugate gradients solver test (Banded system)<" + tag + ">")
        {
            register_tag(Tag_::name);
        }

        virtual void run() const
        {
            DenseVector<DT1_> diag(3, DT1_(1));
            DenseVector<DT1_> bp1(3, DT1_(1));
            DenseVector<DT1_> bp2(3, DT1_(1));
            DenseVector<DT1_> bm1(3, DT1_(1));
            DenseVector<DT1_> bm2(3, DT1_(1));

            diag[0] = DT1_(7);
            diag[1] = DT1_(6);
            diag[2] = DT1_(5);
            bp1[0] = DT1_(-2);
            bp1[1] = DT1_(2);
            bp1[2] = DT1_(0);
            bp2[0] = DT1_(0);
            bp2[1] = DT1_(0);
            bp2[2] = DT1_(0);
            bm1[0] = DT1_(0);
            bm1[1] = DT1_(-2);
            bm1[2] = DT1_(2);
            bm2[0] = DT1_(0);
            bm2[1] = DT1_(0);
            bm2[2] = DT1_(0);

            BandedMatrix<DT1_> A(3);
            A.insert_band(0, diag);
            A.insert_band(1, bp1);
            A.insert_band(2, bp2);
            A.insert_band(-1, bm1);
            A.insert_band(-2, bm2);

            DenseVector<DT1_> b(3, DT1_(1));
            b[0] = DT1_(3);
            b[1] = DT1_(3);
            b[2] = DT1_(0);

            std::cout << "A:" << A << std::endl;
            std::cout << "b:" << b << std::endl;

            //DenseVector<DT1_> result = ConjugateGradients<tags::CPU, methods::JAC>::value(A,b,long(2));
            //DT1_ x_n = Norm< vnt_l_two, false, Tag_>::value(result);
            DenseVector<DT1_> x_analytical(3, DT1_(1));
            x_analytical[0] = DT1_(2./3.);
            x_analytical[1] = DT1_(5./6.);
            x_analytical[2] = DT1_(-1./3.);
            DT1_ x_analytical_n = Norm< vnt_l_two, false, Tag_>::value(x_analytical);

            //cout<<"RESULT(v1):"<<result<<endl;
            //TEST_CHECK_EQUAL_WITHIN_EPS(x_analytical_n, x_n , double(0.1));

            DenseVector<DT1_> result_2 = ConjugateGradients<tags::CPU, methods::JAC>::value(A,b,double(std::numeric_limits<double>::epsilon()));
            DT1_ x_n_2 = Norm< vnt_l_two, false, Tag_>::value(result_2);
            TEST_CHECK_EQUAL_WITHIN_EPS(x_analytical_n, x_n_2 , double(0.1));

            std::cout << "RESULT(v2):" << result_2 << std::endl;
        }
};
template <typename Tag_, typename DT1_>
class ConjugateGradientsTestDense_big:
    public BaseTest
{
    public:
        ConjugateGradientsTestDense_big(const std::string & tag) :
            BaseTest("CG  solver BIG!!!! test (dense system)<" + tag + ">")
        {
            register_tag(Tag_::name);
        }

        virtual void run() const
        {

            unsigned long size = 1000;
            DenseMatrix<DT1_> A(size,size, DT1_(1));
            DenseVector<DT1_> b(size, DT1_(1));

            //std::cout<<"A:"<<A<<endl;
            //std::cout<<"b:"<<b<<endl;
            DenseVector<DT1_> result(ConjugateGradients<tags::CPU, NONE>::value(A,b,double(std::numeric_limits<double>::epsilon())));
            DT1_ x_n = Norm< vnt_l_two, false, Tag_>::value(result);
            DenseVector<DT1_> x_analytical(size, DT1_(0.001));
            //cout<<"RESULT(v1):"<<result<<endl;

            DT1_ x_analytical_n = Norm< vnt_l_two, false, Tag_>::value(x_analytical);
            TEST_CHECK_EQUAL_WITHIN_EPS(x_analytical_n, x_n , double(0.001));

        }
};
template <typename Tag_, typename DT1_>
class ConjugateGradientsTestSparse:
    public BaseTest
{
    public:
        ConjugateGradientsTestSparse(const std::string & tag) :
            BaseTest("Conjugate gradients solver test (sparse system)<" + tag + ">")
        {
            register_tag(Tag_::name);
        }

        virtual void run() const
        {
            SparseMatrix<DT1_> A(3, 3);
            DenseVector<DT1_> b(3, DT1_(1));
            A[0][0] = DT1_(7);
            A[0][1] = DT1_(-2);
            A[0][2] = DT1_(0);
            A[1][0] = DT1_(-2);
            A[1][1] = DT1_(6);
            A[1][2] = DT1_(2);
            A[2][0] = DT1_(0);
            A[2][1] = DT1_(2);
            A[2][2] = DT1_(5);

            b[0] = DT1_(3);
            b[1] = DT1_(3);
            b[2] = DT1_(0);

            std::cout << "A:" << A << std::endl;
            std::cout << "b:" << b << std::endl;
            //DenseVector<DT1_> result = ConjugateGradients<tags::CPU, methods::NONE>::value(A,b,long(2));
            //DT1_ x_n = Norm< vnt_l_two, false, Tag_>::value(result);
            DenseVector<DT1_> x_analytical(3, DT1_(0));
            //cout<<"RESULT(v1):"<<result<<endl;

            x_analytical[0] = DT1_(2./3.);
            x_analytical[1] = DT1_(5./6.);
            x_analytical[2] = DT1_(-1./3.);
            DT1_ x_analytical_n = Norm< vnt_l_two, false, Tag_>::value(x_analytical);
            //TEST_CHECK_EQUAL_WITHIN_EPS(x_analytical_n, x_n , double(0.1));

            DenseVector<DT1_> result_2 = ConjugateGradients<tags::CPU, methods::NONE>::value(A,b,double(std::numeric_limits<double>::epsilon()));
            std::cout << "RESULT(v2):" << result_2 << std::endl;

            DT1_ x_n_2 = Norm< vnt_l_two, false, Tag_>::value(result_2);
            TEST_CHECK_EQUAL_WITHIN_EPS(x_analytical_n, x_n_2 , double(0.1));

        }
    };

template <typename Tag_, typename DT1_>
class ConjugateGradientsTestSparseJAC:
    public BaseTest
{
    public:
        ConjugateGradientsTestSparseJAC(const std::string & tag) :
            BaseTest("Preconditioned (JAC) Conjugate gradients solver test (sparse system)<" + tag + ">")
        {
            register_tag(Tag_::name);
        }

        virtual void run() const
        {
            DenseMatrix<DT1_> A(3, 3);
            DenseVector<DT1_> b(3, DT1_(1));
            A[0][0] = DT1_(7);
            A[0][1] = DT1_(-2);
            A[0][2] = DT1_(0);
            A[1][0] = DT1_(-2);
            A[1][1] = DT1_(6);
            A[1][2] = DT1_(2);
            A[2][0] = DT1_(0);
            A[2][1] = DT1_(2);
            A[2][2] = DT1_(5);

            b[0] = DT1_(3);
            b[1] = DT1_(3);
            b[2] = DT1_(0);

            std::cout << "A:" << A << std::endl;
            std::cout << "b:" << b << std::endl;
            //DenseVector<DT1_> result = ConjugateGradients<tags::CPU, methods::JAC>::value(A,b,long(2));
            //DT1_ x_n = Norm< vnt_l_two, false, Tag_>::value(result);
            DenseVector<DT1_> x_analytical(3, DT1_(0));
            //cout<<"RESULT(v1):"<<result<<endl;
            x_analytical[0] = DT1_(2./3.);
            x_analytical[1] = DT1_(5./6.);
            x_analytical[2] = DT1_(-1./3.);
            DT1_ x_analytical_n = Norm< vnt_l_two, false, Tag_>::value(x_analytical);
            //TEST_CHECK_EQUAL_WITHIN_EPS(x_analytical_n, x_n , double(0.1));

            DenseVector<DT1_> result_2 = ConjugateGradients<tags::CPU, methods::JAC>::value(A,b,double(std::numeric_limits<double>::epsilon()));
            std::cout<< "RESULT(v2):" << result_2 << std::endl;

            DT1_ x_n_2 = Norm< vnt_l_two, false, Tag_>::value(result_2);
            TEST_CHECK_EQUAL_WITHIN_EPS(x_analytical_n, x_n_2 , double(0.1));

        }
};
ConjugateGradientsTestSparseJAC<tags::CPU, float> cg_test_float_sparse_jac("float");
ConjugateGradientsTestSparseJAC<tags::CPU, double> cg_test_double_sparse_jac("double");
ConjugateGradientsTestSparse<tags::CPU, float> cg_test_float_sparse("float");
ConjugateGradientsTestSparse<tags::CPU, double> cg_test_double_sparse("double");


ConjugateGradientsTestDense<tags::CPU, float> cg_test_float_dense("float");
ConjugateGradientsTestDense<tags::CPU, double> cg_test_double_dense("double");
ConjugateGradientsTestBanded<tags::CPU, float> cg_test_float_banded("float");
ConjugateGradientsTestBanded<tags::CPU, double> cg_test_double_banded("double");
ConjugateGradientsTestDenseJAC<tags::CPU, float> cg_test_float_dense_jac("float");
ConjugateGradientsTestDenseJAC<tags::CPU, double> cg_test_double_dense_jac("double");
ConjugateGradientsTestBandedJAC<tags::CPU, float> cg_test_float_banded_jac("float");
ConjugateGradientsTestBandedJAC<tags::CPU, double> cg_test_double_banded_jac("double");
ConjugateGradientsTestDense_big<tags::CPU, float> cg_test_float_dense_big("float");
ConjugateGradientsTestDense_big<tags::CPU, double> cg_test_double_dense_big("double");


#ifdef HONEI_SSE
ConjugateGradientsTestSparse<tags::CPU::SSE, float> sse_cg_test_float_sparse("SSE float");
ConjugateGradientsTestSparse<tags::CPU::SSE, double> sse_cg_test_double_sparse("SSE double");

ConjugateGradientsTestSparseJAC<tags::CPU::SSE, float> sse_cg_test_float_sparse_jac("SSE float");
ConjugateGradientsTestSparseJAC<tags::CPU::SSE, double> sse_cg_test_double_sparse_jac("SSE double");

ConjugateGradientsTestDense<tags::CPU::SSE, float> sse_cg_test_float_dense("SSE float");
ConjugateGradientsTestDense<tags::CPU::SSE, double> sse_cg_test_double_dense("SSE double");
ConjugateGradientsTestBanded<tags::CPU::SSE, float> sse_cg_test_float_banded("SSE float");
ConjugateGradientsTestBanded<tags::CPU::SSE, double> sse_cg_test_double_banded("SSE double");
ConjugateGradientsTestDenseJAC<tags::CPU::SSE, float> sse_cg_test_float_dense_jac("SSE float");
ConjugateGradientsTestDenseJAC<tags::CPU::SSE, double> sse_cg_test_double_dense_jac("SSE double");
ConjugateGradientsTestBandedJAC<tags::CPU::SSE, float> sse_cg_test_float_banded_jac("SSE float");
ConjugateGradientsTestBandedJAC<tags::CPU::SSE, double> sse_cg_test_double_banded_jac("SSE double");
#endif

template <typename Tag_, typename DT1_>
class ConjugateGradientsMIXEDPRECTestBanded:
    public BaseTest
{
    public:
        ConjugateGradientsMIXEDPRECTestBanded(const std::string & tag) :
            BaseTest("Conjugate gradients solver MIXEDPREC test (Banded system)<" + tag + ">")
        {
            register_tag(Tag_::name);
        }

        virtual void run() const
        {
            DenseVector<DT1_> diag(3, DT1_(1));
            DenseVector<DT1_> bp1(3, DT1_(1));
            DenseVector<DT1_> bp2(3, DT1_(1));
            DenseVector<DT1_> bm1(3, DT1_(1));
            DenseVector<DT1_> bm2(3, DT1_(1));

            diag[0] = DT1_(7);
            diag[1] = DT1_(6);
            diag[2] = DT1_(5);
            bp1[0] = DT1_(-2);
            bp1[1] = DT1_(2);
            bp1[2] = DT1_(0);
            bp2[0] = DT1_(0);
            bp2[1] = DT1_(0);
            bp2[2] = DT1_(0);
            bm1[0] = DT1_(0);
            bm1[1] = DT1_(-2);
            bm1[2] = DT1_(2);
            bm2[0] = DT1_(0);
            bm2[1] = DT1_(0);
            bm2[2] = DT1_(0);

            BandedMatrix<DT1_> A(3);
            A.insert_band(0, diag);
            A.insert_band(1, bp1);
            A.insert_band(2, bp2);
            A.insert_band(-1, bm1);
            A.insert_band(-2, bm2);

            DenseVector<DT1_> b(3, DT1_(1));
            b[0] = DT1_(3);
            b[1] = DT1_(3);
            b[2] = DT1_(0);

            std::cout << "A:" << A << std::endl;
            std::cout << "b:" << b << std::endl;

            DenseVector<DT1_> result = ConjugateGradients<tags::CPU, methods::NONE>::value(A,b,2ul);
            DT1_ x_n = Norm< vnt_l_two, false, Tag_>::value(result);
            DenseVector<DT1_> x_analytical(3, DT1_(1));
            x_analytical[0] = DT1_(2./3.);
            x_analytical[1] = DT1_(5./6.);
            x_analytical[2] = DT1_(-1./3.);
            DT1_ x_analytical_n = Norm< vnt_l_two, false, Tag_>::value(x_analytical);

            std::cout << "RESULT(v1):" << result << std::endl;
            TEST_CHECK_EQUAL_WITHIN_EPS(x_analytical_n, x_n , double(0.1));

            DenseVector<DT1_> result_2 = ConjugateGradients<tags::CPU, methods::NONE>::value(A,b,double(std::numeric_limits<double>::epsilon()), 20);
            DT1_ x_n_2 = Norm< vnt_l_two, false, Tag_>::value(result_2);
            TEST_CHECK_EQUAL_WITHIN_EPS(x_analytical_n, x_n_2 , double(0.1));

            std::cout << "RESULT(v2):" << result_2 << std::endl;
        }
};
ConjugateGradientsMIXEDPRECTestBanded<tags::CPU, float> cg_test_mixed_banded1("float");


template <typename Tag_, typename DT1_>
class ConjugateGradientsTestSparseELL:
    public BaseTest
{
    public:
        ConjugateGradientsTestSparseELL(const std::string & tag) :
            BaseTest("ConjugateGradients solver test (sparse ELL system)<" + tag + ">")
        {
            register_tag(Tag_::name);
        }

        virtual void run() const
        {

            std::string filename(HONEI_SOURCEDIR);
            filename += "/honei/math/testdata/5pt_10x10.mtx";
            unsigned long non_zeros(MatrixIO::get_non_zeros(filename));
            unsigned long rows, columns, ax, bx;
            DenseVector<unsigned long> r(non_zeros);
            DenseVector<unsigned long> c(non_zeros);
            DenseVector<DT1_> data(non_zeros);

            MatrixIO::read_matrix(filename, r, c, data);
            MatrixIO::get_sizes(filename, columns, rows, ax, bx);
            SparseMatrixELL<DT1_> smatrix2(rows, columns, r, c, data);

            DenseVector<DT1_> x(rows, DT1_(1.2345));

            DenseVector<DT1_> rhs(rows, DT1_(0));

            Product<Tag_>::value(rhs, smatrix2, x);

            DenseVector<DT1_> initial_guess(rows, DT1_(2));
            DenseVector<DT1_> diag_inverted(rows, DT1_(0));
            for(unsigned long i(0) ; i < data.size() ; ++i)
            {
                if(r[i] == c[i])
                    diag_inverted[r[i]] = DT1_(1)/data[i];
            }

            DenseVector<DT1_> result(rhs.size(), DT1_(1));
            ConjugateGradients<Tag_, JAC>::value(smatrix2, rhs, result, diag_inverted, 1000ul);

            result.lock(lm_read_only);
            x.lock(lm_read_only);
            std::cout << result << std::endl;
            for(unsigned long i(0) ; i < rhs.size() ; ++i)
            {
                TEST_CHECK_EQUAL_WITHIN_EPS(result[i], x[i], std::numeric_limits<DT1_>::epsilon());
            }
            result.unlock(lm_read_only);
            x.unlock(lm_read_only);

        }
};
ConjugateGradientsTestSparseELL<tags::CPU, float> cg_test_float_sparse_ell("float");
ConjugateGradientsTestSparseELL<tags::CPU, double> cg_test_double_sparse_ell("double");
#ifdef HONEI_CUDA
ConjugateGradientsTestSparseELL<tags::GPU::CUDA, float> cuda_cg_test_float_sparse_ell("float");
//ConjugateGradientsTestSparseELL<tags::GPU::CUDA, double> cuda_cg_test_double_sparse_ell("double");
#endif
