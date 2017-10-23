/* vim: set sw=4 sts=4 et foldmethod=syntax : */

/*
 * Copyright (c) 2007, 2008 Danny van Dyk <danny.dyk@uni-dortmund.de>
 * Copyright (c) 2010 Sven Mallach <mallach@honei.org>
 *
 * Based upon 'thread.hh' from Paludis, which is:
 *     Copyright (c) 2007 Ciaran McCreesh
 *
 * This file is part of the Utility C++ library. LibUtil is free software;
 * you can redistribute it and/or modify it under the terms of the GNU General
 * Public License version 2, as published by the Free Software Foundation.
 *
 * LibUtil is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc., 59 Temple
 * Place, Suite 330, Boston, MA  02111-1307  USA
 */

#pragma once
#ifndef LIBUTIL_GUARD_THREAD_HH
#define LIBUTIL_GUARD_THREAD_HH 1

#include <honei/util/instantiation_policy.hh>
#include <honei/util/private_implementation_pattern.hh>

#include <functional>

namespace honei
{
    /**
     * Thread uses TR1's function wrapper to execute a given function in a
     * separate thread.
     *
     * Thread uses POSIX threads internally and allows minimal synchronisation.
     * Thread guarantees that its function is finalised after destruction.
     */
    class Thread :
        public InstantiationPolicy<Thread, NonCopyable>,
        public PrivateImplementationPattern<Thread, Single>
    {
        public:
            /// Our function type.
            typedef std::function<void ()> Function;

            /// \name Constructor and destructor
            /// \{

            /**
             * Constructor.
             *
             * \param function A Function object that wraps the thread's main function.
             */
            Thread(const Function & function);

            /// Destructor.
            ~Thread();

            /// \}

            /// Return whether we have yet completed executing our function.
            bool completed() const;

            /// Return the current stack size of this thread.
            size_t get_stacksize() const;
    };
}

#endif
