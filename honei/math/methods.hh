/* vim: set sw=4 sts=4 et foldmethod=syntax : */

#pragma once
#ifndef LIBMATH_GUARD_METHODS_HH
#define LIBMATH_GUARD_METHODS_HH 1
/*
 * Copyright (c) 2007 Markus Geveler <apryde@gmx.de>
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

namespace methods
{
    class CG
    {
    };

    class JAC
    {
    };

    class NONE
    {
    };

    class PCG
    {
        public:
            class JAC;
    };

    class CYCLE
    {
        public:
            class F;
            class W;
            class V;
    };
    class FIXED;
    class MIXED;

    class PROLMAT;

    class NATURAL;
    class TWO_LEVEL;
}


namespace applications
{
    class POISSON;
}
namespace boundary_types
{
    class DIRICHLET
    {
        public:
            class DIRICHLET_0;
    };
    class NEUMANN;
    class DIRICHLET_NEUMANN
    {
    };
}

#endif