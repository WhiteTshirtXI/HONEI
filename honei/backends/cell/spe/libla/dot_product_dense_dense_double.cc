/* vim: set sw=4 sts=4 et foldmethod=syntax : */

/*
 * Copyright (c) 2008 Sven Mallach <sven.mallach@honei.org>
 * Copyright (c) 2008 Dirk Ribbrock <dirk.ribbrock@uni-dortmund.de>
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
#include <honei/backends/cell/cell.hh>
#include <honei/backends/cell/spe/libla/operations.hh>
#include <honei/backends/cell/spe/libutil/allocator.hh>
#include <honei/backends/cell/spe/libutil/transfer.hh>

#include <spu_intrinsics.h>

namespace honei
{
    namespace cell
    {
        namespace implementation
        {
            vector double dot_product_dense_dense_double(const vector double & accumulator, vector double & b_carry, const vector double * a,
                    const vector double * b, unsigned size, unsigned b_offset, double, double)
            {
                vector double acc = accumulator;
                for (unsigned i(0) ; i < size ; ++i)
                {
                    extract(b_carry, b[i], b_offset);
                    acc = spu_madd(a[i], b_carry, acc);
                    b_carry = b[i];
                }

                return acc;
            }
        }

        namespace operations
        {
            Operation<2, double, rtm_mail> dot_product_dense_dense_double = {
                &zero_double,
                &implementation::dot_product_dense_dense_double,
                &sum_double
            };
        }
    }
}
