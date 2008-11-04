/* vim: set sw=4 sts=4 et foldmethod=syntax : */

/*
 * Copyright (c) 2008 Markus Geveler <apryde@gmx.de>
 *
 * This file is part of the MATH C++ library. LibMath is free software;
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

#ifndef MATH_GUARD_FILL_MATRIX_HH
#define MATH_GUARD_FILL_MATRIX_HH 1

#include<honei/la/dense_vector.hh>
#include<honei/la/banded_matrix_q1.hh>
#include<honei/math/methods.hh>
#include<cmath>

using namespace methods;
namespace honei
{
    namespace applications
    {
        class POISSON;
    }
    namespace boundary_types
    {
        class DIRICHLET
        {
            class DIRICHLET_0;
        };
        class NEUMANN;
        class DIRICHLET_NEUMANN
        {
            class NEUMANN_EAST_DIRICHLET_1;
        };
    }
    template <typename Tag_, typename Application_, typename BoundaryType_>
        struct FillMatrix
        {
        };

    template <typename Tag_>
        struct FillMatrix<Tag_, applications::POISSON, boundary_types::DIRICHLET::DIRICHLET_0>
        {
            template <typename DT_>
                static inline void value(BandedMatrixQ1<DT_> & target)
                {
                    unsigned long size(target.band(DD).size());
                    DT_ * ll = target.band(LL).elements();
                    DT_ * ld = target.band(LD).elements();
                    DT_ * lu = target.band(LU).elements();

                    DT_ * dl = target.band(DL).elements();
                    DT_ * dd = target.band(DD).elements();
                    DT_ * du = target.band(DU).elements();

                    DT_ * ul = target.band(UL).elements();
                    DT_ * ud = target.band(UD).elements();
                    DT_ * uu = target.band(UU).elements();

                    unsigned long N(size);
                    unsigned long M((unsigned long)sqrt(N));
                    for (unsigned long i(1) ; i <= N ; ++i)
                    {
                        // first, Dirichlet unit rows
                        if (i <= M || i > N-M || (i%M) == 1 || (i%M) == 0)
                        {
                            dd[i-1] = 1.0;
                            ll[i-1] = ld[i-1] = lu[i-1] = 0.0;
                            dl[i-1] = du[i-1] = 0.0;
                            ul[i-1] = ud[i-1] = uu[i-1] = 0.0;
                        }
                        // then, inner points
                        else
                        {
                            dd[i-1] = 8.0/3.0;
                            ll[i-1] = ld[i-1] = lu[i-1] = -1.0/3.0;
                            dl[i-1] = du[i-1] = -1.0/3.0;
                            ul[i-1] = ud[i-1] = uu[i-1] = -1.0/3.0;
                        }
                    }
                }
        };
}
#endif
