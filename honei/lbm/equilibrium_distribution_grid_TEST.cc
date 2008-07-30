/* vim: set number sw=4 sts=4 et nofoldenable : */

/*
 * Copyright (c) 2008 Markus Geveler <apryde@gmx.de>
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
#include <honei/lbm/tags.hh>
#include <unittest/unittest.hh>
#include <honei/lbm/equilibrium_distribution_grid.hh>

using namespace honei;
using namespace tests;
using namespace std;

template <typename Tag_, typename DataType_>
class EqDisGridLABSWETest :
    public TaggedTest<Tag_>
{
    public:
        EqDisGridLABSWETest(const std::string & type) :
            TaggedTest<Tag_>("eq_dis_grid_labswe_test<" + type + ">")
        {
        }

        virtual void run() const
        {
            PackedGridData<lbm_lattice_types::D2Q9, DataType_> data;

            data.h = new DenseVector<DataType_>(1000ul, DataType_(1.23456));
            data.u = new DenseVector<DataType_>(1000ul, DataType_(1.23456));
            data.v = new DenseVector<DataType_>(1000ul, DataType_(1.23456));
            DataType_ g(9.81);
            DataType_ e(1.);
            DenseVector<DataType_> e_u(9ul, DataType_(2.));
            data.f_eq_0 = new DenseVector<DataType_>(1000);
            data.f_eq_1 = new DenseVector<DataType_>(1000);
            data.f_eq_2 = new DenseVector<DataType_>(1000);
            data.f_eq_3 = new DenseVector<DataType_>(1000);
            data.f_eq_4 = new DenseVector<DataType_>(1000);
            data.f_eq_5 = new DenseVector<DataType_>(1000);
            data.f_eq_6 = new DenseVector<DataType_>(1000);
            data.f_eq_7 = new DenseVector<DataType_>(1000);
            data.f_eq_8 = new DenseVector<DataType_>(1000);
            EquilibriumDistributionGrid<Tag_, lbm_applications::LABSWE>::
                value(g, e, e_u, e_u, data);

            for(unsigned long i(0); i < 1000; ++i)
            {
                TEST_CHECK_EQUAL_WITHIN_EPS((*data.f_eq_0)[i], (1.23456 - ((5. * 9.81 * 1.23456 * 1.23456) / (6.)) - ((2. * 1.23456) / (3.)) * ((1.23456 * 1.23456) + (1.23456 * 1.23456))), std::numeric_limits<DataType_>::epsilon() * 20.);
                TEST_CHECK_EQUAL_WITHIN_EPS((*data.f_eq_1)[i], ((9.81 * 1.23456 * 1.23456) / 6. + ((1.23456 / 3.) * (2. * 1.23456 + 2. * 1.23456)) + ((1.23456 / 2.) * (2. * 1.23456 * 2. * 1.23456 + 2. * 2. * 1.23456 * 2. * 1.23456 + 2. * 1.23456 * 2. * 1.23456)) - ((1.23456 / 6.) * (1.23456 * 1.23456 + 1.23456 * 1.23456))), std::numeric_limits<DataType_>::epsilon() * 20.);
                TEST_CHECK_EQUAL_WITHIN_EPS((*data.f_eq_2)[i], ((9.81 * 1.23456 * 1.23456) / 24. + ((1.23456 / 12.) * (2. * 1.23456 + 2. * 1.23456)) + ((1.23456 / 8.) * (2. * 1.23456 * 2. * 1.23456 + 2. * 2. * 1.23456 * 2. * 1.23456 + 2. * 1.23456 * 2. * 1.23456)) - ((1.23456 / 24.) * (1.23456 * 1.23456 + 1.23456 * 1.23456))), std::numeric_limits<DataType_>::epsilon() * 20.);
            }
        }
};
EqDisGridLABSWETest<tags::CPU, double> source_test_double("double");
EqDisGridLABSWETest<tags::CPU, float> source_test_float("float");
