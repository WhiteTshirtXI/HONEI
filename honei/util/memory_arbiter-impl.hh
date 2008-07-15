/* vim: set sw=4 sts=4 et foldmethod=syntax : */

/*
 * Copyright (c) 2008 Dirk Ribbrock <dirk.ribbrock@uni-dortmund.de>
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

#ifndef MEMORY_GUARD_MEMORY_CONTROLLER_IMPL_HH
#define MEMORY_GUARD_MEMORY_CONTROLLER_IMPL_HH 1

#include <honei/util/instantiation_policy.hh>
#include <honei/util/private_implementation_pattern.hh>
#include <honei/util/private_implementation_pattern-impl.hh>
#include <honei/util/memory_arbiter.hh>
#include <honei/util/tags.hh>
#include <honei/util/exception.hh>
#include <honei/util/assertion.hh>
#include <honei/util/lock.hh>
#include <honei/util/log.hh>
#include <honei/util/stringify.hh>
#include <honei/util/mutex.hh>
#include <honei/util/condition_variable.hh>
#include <honei/util/memory_backend_base.hh>

#include <set>
#include <map>

namespace honei
{
    template <> struct Implementation<MemoryArbiter>
    {
        /// Our mutex.
        Mutex * const _mutex;

        /// Our one read/write lock has faded condition variable.
        ConditionVariable * _access_finished;

        /// Our map of all memory backends
        std::map<tags::TagValue, MemoryBackendBase *> _backends;

        /// Logical representation of a used chunk of memory.
        struct MemoryBlock
        {
            public:
                MemoryBlock(tags::TagValue w) :
                    writer(w)
            {
                read_count = 0;
                write_count = 0;
            }

                unsigned read_count;
                unsigned write_count;
                tags::TagValue writer;
                std::set<tags::TagValue> readers;
        };

        /// Our map of all memory blocks.
        std::map<void *, MemoryBlock> _blocks;

        friend class MemoryBackendRegistrator;

        /// Constructor
        Implementation() :
            _mutex(new Mutex),
            _access_finished(new ConditionVariable)
        {
        }

        /// Destructor
        ~Implementation()
        {
            delete _access_finished;
            delete _mutex;
        }

        void register_address(void * memid)
        {
            Lock l(*_mutex);
            std::map<void *, MemoryBlock>::iterator i(_blocks.find(memid));
            if (i != _blocks.end())
            {
                throw InternalError("MemoryArbiter: Duplicate Memory Block!");
            }
            else
            {
                MemoryBlock new_block(tags::tv_none);
                _blocks.insert(std::pair<void *, MemoryBlock>(memid, new_block));
            }
        }

        void remove_address(void * memid)
        {
            Lock l(*_mutex);
            std::map<void *, MemoryBlock>::iterator i(_blocks.find(memid));
            if (i == _blocks.end())
            {
                throw InternalError("MemoryArbiter: Memory Block not found!");
            }
            else
            {
                ASSERT(i->second.read_count == 0 , "Deleting MemoryBlock " + stringify(memid) + " that is still under " + stringify(i->second.read_count) + " read access!");
                ASSERT(i->second.write_count == 0, "Deleting MemoryBlock " + stringify(memid) + " that is still under " + stringify(i->second.write_count) + " write access!");
                // Delete the deprecated memory block in all relevant memory backends
                for (std::set<tags::TagValue>::iterator j(i->second.readers.begin()), j_end(i->second.readers.end()) ;
                        j != j_end ; ++j)
                {
                    _backends[*j]->free(memid);
                }
                _blocks.erase(i);
            }
        }

        void * read_only(tags::TagValue memory, void * memid, void * address, unsigned long bytes)
        {
            Lock l(*_mutex);
            std::map<void *, MemoryBlock>::iterator i(_blocks.find(memid));
            // Wait until no one writes on our memory block
            while (i != _blocks.end() && i->second.write_count != 0)
            {
                _access_finished->wait(*_mutex);
                i = _blocks.find(memid);
            }
            if (i == _blocks.end())
            {
                throw InternalError("MemoryArbiter: Memory Block not found!");
            }
            else
            {
                // If our memory block was changed on any other remote side, write it back to the main memory.
                if (i->second.writer != tags::tv_none && i->second.writer != memory)
                {
                    _backends[i->second.writer]->download(memid, address, bytes);
                    i->second.writer = tags::tv_none;
                }
                i->second.read_count++;
                i->second.readers.insert(memory);
            }
            return _backends[memory]->upload(memid, address, bytes);
        }

        void * read_and_write(tags::TagValue memory, void * memid, void * address, unsigned long bytes)
        {
            Lock l(*_mutex);
            std::map<void *, MemoryBlock>::iterator i(_blocks.find(memid));
            // Wait until no one reads or writes on our memory block
            while (i != _blocks.end() && (i->second.write_count != 0 || i->second.read_count != 0))
            {
                _access_finished->wait(*_mutex);
                i = _blocks.find(memid);
            }
            if (i == _blocks.end())
            {
                throw InternalError("MemoryArbiter: Memory Block not found!");
            }
            else
            {
                // If our memory block was changed on any other remote side, write it back to the main memory.
                if (i->second.writer != tags::tv_none && i->second.writer != memory)
                {
                    _backends[i->second.writer]->download(memid, address, bytes);
                }
                // Delete the deprecated memory block in all relevant memory backends
                for (std::set<tags::TagValue>::iterator j(i->second.readers.begin()), j_end(i->second.readers.end()) ;
                        j != j_end ; ++j)
                {
                    if (*j != memory)
                    {
                        _backends[*j]->free(memid);
                    }

                }
                i->second.readers.clear();
                i->second.writer= memory;
                i->second.write_count++;
                i->second.readers.insert(memory);
            }
            return _backends[memory]->upload(memid, address, bytes);
        }

        void * write_only(tags::TagValue memory, void * memid, void * address, unsigned long bytes)
        {
            Lock l(*_mutex);
            std::map<void *, MemoryBlock>::iterator i(_blocks.find(memid));
            // Wait until no one reads or writes on our memory block
            while (i != _blocks.end() && (i->second.write_count != 0 || i->second.read_count != 0))
            {
                _access_finished->wait(*_mutex);
                i = _blocks.find(memid);
            }
            if (i == _blocks.end())
            {
                throw InternalError("MemoryArbiter: Memory Block not found!");
            }
            else
            {
                // Delete the deprecated memory block in all relevant memory backends
                for (std::set<tags::TagValue>::iterator j(i->second.readers.begin()), j_end(i->second.readers.end()) ;
                        j != j_end ; ++j)
                {
                    if (*j != memory)
                    {
                        _backends[*j]->free(memid);
                    }

                }
                i->second.readers.clear();
                i->second.writer= memory;
                i->second.write_count++;
                i->second.readers.insert(memory);
            }
            return _backends[memory]->alloc(memid, address, bytes);
        }

        void release_read(void * memid)
        {
            Lock l(*_mutex);
            std::map<void *, MemoryBlock>::iterator i(_blocks.find(memid));
            if(i == _blocks.end())
            {
                throw InternalError("MemoryArbiter::release_read MemoryBlock not found!");
            }
            else
            {
                if (i->second.read_count == 0)
                {
                    throw InternalError("MemoryArbiter::release_read: read counter is already zero!");
                }
                else
                {
                    i->second.read_count--;
                }
                _access_finished->broadcast();
            }
        }

        void release_write(void * memid)
        {
            Lock l(*_mutex);
            std::map<void *, MemoryBlock>::iterator i(_blocks.find(memid));
            if(i == _blocks.end())
            {
                throw InternalError("MemoryArbiter::release_write MemoryBlock not found!");
            }
            else
            {
                if (i->second.write_count == 0)
                {
                    throw InternalError("MemoryArbiter::release_write: write counter is already zero!");
                }
                else
                {
                    i->second.write_count--;
                }
                _access_finished->broadcast();
            }
        }
    };

    MemoryArbiter::MemoryArbiter() :
        PrivateImplementationPattern<MemoryArbiter, Shared>(new Implementation<MemoryArbiter>())
    {
    }

    MemoryArbiter::~MemoryArbiter()
    {
    }

    void MemoryArbiter::register_address(void * memid)
    {
        CONTEXT("When registering memory address:");
        _imp->register_address(memid);
    }

    void MemoryArbiter::remove_address(void * memid)
    {
        CONTEXT("When removing memory address:");
        _imp->remove_address(memid);
    }

    void * MemoryArbiter::lock(LockMode mode, tags::TagValue memory, void * memid, void * address, unsigned long bytes)
    {
        switch (mode)
        {
            case lm_read_only:
                return read_only(memory, memid, address, bytes);
            case lm_write_only:
                return write_only(memory, memid, address, bytes);
            case lm_read_and_write:
                return read_and_write(memory, memid, address, bytes);
        }
    }

    void MemoryArbiter::unlock(LockMode mode, void * memid)
    {
        switch (mode)
        {
            case lm_read_only:
                release_read(memid);
                break;
            case lm_write_only:
                release_write(memid);
                break;
            case lm_read_and_write:
                release_write(memid);
                break;
        }
    }

    void * MemoryArbiter::read_only(tags::TagValue memory, void * memid, void * address, unsigned long bytes)
    {
        CONTEXT("When retrieving read_only lock:");
        return _imp->read_only(memory, memid, address, bytes);
    }

    void * MemoryArbiter::read_and_write(tags::TagValue memory, void * memid, void * address, unsigned long bytes)
    {
        CONTEXT("When retrieving read_and_write lock:");
        return _imp->read_and_write(memory, memid, address, bytes);
    }

    void * MemoryArbiter::write_only(tags::TagValue memory, void * memid, void * address, unsigned long bytes)
    {
        CONTEXT("When retrieving write_only lock:");
        return _imp->write_only(memory, memid, address, bytes);
    }

    void MemoryArbiter::release_read(void * memid)
    {
        CONTEXT("When releasing read lock:");
        _imp->release_read(memid);
    }

    void MemoryArbiter::release_write(void * memid)
    {
        CONTEXT("When releasing write lock:");
        _imp->release_write(memid);
    }

    void MemoryArbiter::insert_backend(std::pair<tags::TagValue, MemoryBackendBase *> backend)
    {
        CONTEXT("When adding a new memory backend:");
        Lock l(*this->_imp->_mutex);
        this->_imp->_backends.insert(backend);
    }
}
#endif