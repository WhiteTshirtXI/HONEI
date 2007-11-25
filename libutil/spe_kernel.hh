/* vim: set sw=4 sts=4 et foldmethod=syntax : */

/*
 * Copyright (c) 2007 Danny van Dyk <danny.dyk@uni-dortmund.de>
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

#ifndef LIBUTIL_GUARD_SPE_KERNEL_HH
#define LIBUTIL_GUARD_SPE_KERNEL_HH 1

#include <cell/interface.hh>
#include <libutil/spe_manager.hh>
#include <libutil/time_stamp.hh>

namespace honei
{
    // Forward declarations.
    class SPEInstruction;
    class SPEKernelRegistrator;

    /**
     * \ingroup grpcell
     */
    class SPEKernel
    {
        public:
            typedef cell::KernelInfo Info;
        private:
            struct Implementation;

            /// Our implementation.
            std::tr1::shared_ptr<Implementation> _imp;

            /// Return a pointer to our kernel's argument.
            void * argument() const;

            /// Return a pointer to our environment.
            void * environment() const;

            /// Signal that the SPE part has been loaded.
            void signal() const;

        public:
            friend class SPE::Implementation;

            /// Constructor.
            SPEKernel(const SPEKernel::Info & info);

            /// \name Iteration over supported opcodes.
            /// \{

            typedef libwrapiter::ForwardIterator<SPEKernel, const cell::OpCode> OpCodeIterator;

            OpCodeIterator begin_supported_opcodes() const;

            OpCodeIterator end_supported_opcodes() const;

            /// \}

            /// Enqueue an instruction.
            unsigned int enqueue(const SPEInstruction & instruction);

            /// Execute the kernel's PPE part.
            void operator() () throw ();

            /**
             * Wait until specified instruction has finished.
             *
             * \param instruction_index Index of the instruction that we are waiting for.
             */
            void wait(unsigned instruction_index) const;

            /**
             * Load the kernel into a given SPE.
             *
             * \param spe The SPE into which the kernel shall be loaded.
             */
            void load(const SPE & spe) const;

            /**
             * Return the number of unfinished enqueued instructions.
             */
            unsigned instruction_load() const;

            /**
             * Return the timepoint of the last finished instruction.
             */
            TimeStamp last_finished() const;
    };
}

#endif
