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

#include <mpi.h>
#include <honei/backends/mpi/operations.hh>

namespace honei
{
    namespace mpi
    {
        void mpi_init(int * argc, char*** argv)
        {
            MPI_Init(argc, argv);
        }

        void mpi_finalize()
        {
            MPI_Finalize();
        }

        void mpi_comm_size(int * size)
        {
            MPI_Comm_size(MPI_COMM_WORLD, size);
        }

        void mpi_comm_rank(int * id)
        {
            MPI_Comm_rank(MPI_COMM_WORLD, id);
        }

        template <typename DT_>
        void mpi_bcast(DT_ * data, unsigned long size, int sender)
        {
            MPI_Bcast(data, size * sizeof(DT_), MPI_BYTE, sender, MPI_COMM_WORLD);
        }

        template <typename DT_>
        void mpi_send(DT_ * data, unsigned long size, int target, int tag)
        {
            MPI_Send(data, size * sizeof(DT_), MPI_BYTE, target, tag, MPI_COMM_WORLD);
        }

        template <typename DT_>
        void mpi_recv(DT_ * data, unsigned long size, int sender, int tag)
        {
            MPI_Status _stat;
            MPI_Recv(data, size * sizeof(DT_), MPI_BYTE, sender, tag, MPI_COMM_WORLD, &_stat);
        }

        template void mpi_bcast<float>(float * data, unsigned long size, int sender);
        template void mpi_bcast<double>(double * data, unsigned long size, int sender);
        template void mpi_bcast<unsigned long>(unsigned long * data, unsigned long size, int sender);

        template void mpi_send<float>(float * data, unsigned long size, int target, int tag);
        template void mpi_send<double>(double * data, unsigned long size, int target, int tag);
        template void mpi_send<unsigned long>(unsigned long * data, unsigned long size, int target, int tag);

        template void mpi_recv<float>(float * data, unsigned long size, int sender, int tag);
        template void mpi_recv<double>(double * data, unsigned long size, int sender, int tag);
        template void mpi_recv<unsigned long>(unsigned long * data, unsigned long size, int sender, int tag);
    }
}