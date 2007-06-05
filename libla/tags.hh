/* vim: set sw=4 sts=4 et foldmethod=syntax : */

/*
 * Copyright (c) 2007 Danny van Dyk <danny.dyk@uni-dortmund.de>
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

#ifndef LIBLA_GUARD_TAGS_HH
#define LIBLA_GUARD_TAGS_HH 1

namespace pg512 ///< \todo Namespace name?
{
    namespace tags
    {
        /**
         * Tag-type for generic/C++-based operations.
         *
         * \ingroup grptagscpu
         **/
        struct CPU
        {
            /**
             * Tag-type for multithreaded/C++-based operations.
             *
             * \ingroup grptagscpumulticore
             **/
            struct MultiCore;
        };

        /**
         * Tag-type for Cell-based operations.
         *
         * \ingroup grptagscell
         **/
        struct Cell
        {
        };
    }
}

#endif
