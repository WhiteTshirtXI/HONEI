/* vim: set sw=4 sts=4 et foldmethod=syntax : */

/*
 * Copyright (c) 2007 Sven Mallach <sven.mallach@uni-dortmund.de>
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
#include <cell/libutil/allocator.hh>
#include <cell/libutil/transfer.hh>

#include <spu_intrinsics.h>
#include <spu_mfcio.h>
#include <stdio.h>

using namespace honei::cell;

int dense_sparse_float_sum(const Instruction & inst)
{
    Allocation * block_a(acquire_block()); // a's elements
    Allocation * block_b(acquire_block()); // b's elements
    Allocation * block_c(acquire_block()); // b's indices

    Pointer<float> a = { block_a->address };
    Pointer<float> b = { block_b->address };
    Pointer<unsigned long long> c = { block_c->address };

    mfc_get(a.untyped, inst.a.ea, multiple_of_sixteen(inst.size * sizeof(float)), 1, 0, 0);
    mfc_get(b.untyped, inst.b.ea, multiple_of_sixteen(inst.size * sizeof(float)), 2, 0, 0);
    mfc_get(c.untyped, inst.c.ea, multiple_of_sixteen(inst.size * sizeof(unsigned long long)), 3, 0, 0);

    mfc_write_tag_mask(1 << 3);
    mfc_read_tag_status_all();
    mfc_write_tag_mask(1 << 2);
    mfc_read_tag_status_all();
    mfc_write_tag_mask(1 << 1);
    mfc_read_tag_status_all();

    for (unsigned long long i(0); i < inst.d.u ; i++)
    {
        a.typed[c.typed[i]] += b.typed[i];
    }

    mfc_put(a.untyped, inst.a.ea, multiple_of_sixteen(inst.size * sizeof(float)), 4, 0, 0);
    mfc_write_tag_mask(1 << 4);
    mfc_read_tag_status_all();

    release_block(*block_a);
    release_block(*block_b);
    release_block(*block_c);

    return 0;
}
