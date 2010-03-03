/* vim: set sw=4 sts=4 et foldmethod=syntax : */

/*
 * Copyright (c) 2008, 2009, 2010 Sven Mallach <mallach@honei.org>
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

#include <honei/backends/multicore/ticket.hh>
#include <honei/util/assertion.hh>
#include <honei/util/condition_variable.hh>
#include <honei/util/exception.hh>
#include <honei/util/lock.hh>
#include <honei/util/private_implementation_pattern-impl.hh>

namespace honei
{
        template <> struct Implementation<Ticket<tags::CPU::MultiCore> >
        {
            Mutex mutex;

            ConditionVariable completion;

            bool completed;

            /// Counter for unique global ticket IDs
            static unsigned counter;

            /// Unique ID
            unsigned id;

            /// Lowest sched_id of a core that may execute this task
            unsigned sched_min;

            /// Highest sched_id of a core that may execute this task
            unsigned sched_max;

            /// ID of the core that really executed this task
            unsigned sched_id;

            /// Process ID of the executing thread
            unsigned thread_id;

            Implementation(const unsigned sid_min, const unsigned sid_max) :
                completed(false),
                id(counter),
                sched_min(sid_min),
                sched_max(sid_max),
                sched_id(0xFFFF)
             {
                ++counter;
            }
        };

        Ticket<tags::CPU::MultiCore>::Ticket(const unsigned sid_min, const unsigned sid_max) :
            PrivateImplementationPattern<Ticket<tags::CPU::MultiCore>, Shared>(new Implementation<Ticket<tags::CPU::MultiCore> >(sid_min, sid_max))
        {
        }

        void Ticket<tags::CPU::MultiCore>::mark()
        {
            CONTEXT("When marking a ticket for completion:");
            Lock l(_imp->mutex);

            ASSERT(! _imp->completed, "ticket marked more than once!");

            _imp->completed = true;
            _imp->completion.signal();
        }

        void Ticket<tags::CPU::MultiCore>::wait() const
        {
            CONTEXT("When waiting for ticket completion:");
            Lock l(_imp->mutex);

            while (! _imp->completed)
            {
                _imp->completion.wait(_imp->mutex);
            }
        }

        unsigned Ticket<tags::CPU::MultiCore>::uid() const
        {
            return _imp->id;
        }

        unsigned Ticket<tags::CPU::MultiCore>::sid_min() const
        {
            return _imp->sched_min;
        }

        unsigned Ticket<tags::CPU::MultiCore>::sid_max() const
        {
            return _imp->sched_max;
        }

        unsigned & Ticket<tags::CPU::MultiCore>::sid()
        {
            return _imp->sched_id;
        }

        unsigned & Ticket<tags::CPU::MultiCore>::tid()
        {
            return _imp->thread_id;
        }

        unsigned Implementation<Ticket<tags::CPU::MultiCore> >::counter(0);
}
