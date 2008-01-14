/* vim: set sw=4 sts=4 et foldmethod=syntax : */

/*
 * Copyright (c) 2008 Sven Mallach <sven.mallach@honei.org>
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

#include <cell/cell.hh>
#include <cell/libla/operations.hh>
#include <cell/libutil/allocator.hh>
#include <cell/libutil/transfer.hh>

#include <spu_intrinsics.h>

namespace honei
{
    namespace cell
    {
        namespace implementation
        {
            void sum_dense_dense_float(vector float * a, const vector float * b,
                    const unsigned size, vector float & b_carry, const unsigned b_offset)
            {
                for (unsigned i(0) ; i < size ; ++i)
                {
                    extract(b_carry, b[i], b_offset);
                    a[i] = spu_add(a[i], b_carry);
                    b_carry = b[i];
                }
            }
        }

        namespace operations
        {
            Operation<2, float, rtm_dma> sum_dense_dense_float = {
                &implementation::sum_dense_dense_float
            };
        }
    }
}
