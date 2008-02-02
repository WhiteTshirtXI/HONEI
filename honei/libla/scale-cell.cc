/* vim: set sw=4 sts=4 et foldmethod=syntax : */

/*
 * Copyright (c) 2007, 2008 Sven Mallach <sven.mallach@honei.org>
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

#include <honei/cell/cell.hh>
#include <honei/libla/scale.hh>
#include <honei/libutil/memory_backend_cell.hh>
#include <honei/libutil/spe_instruction.hh>
#include <honei/libutil/spe_manager.hh>
#include <honei/libutil/spe_transfer_list.hh>

namespace honei
{
    using namespace cell;

    DenseMatrix<float> &
    Scale<tags::Cell>::value(DenseMatrix<float> & b, const float a)
    {
        CONTEXT("When scaling DenseMatrix<float> (Cell):");

        SPEFrameworkInstruction<1, float, cell::rtm_dma> instruction(oc_scale_dense_float, b.elements(), b.rows() * b.columns(), a);

        if (instruction.use_spe())
        {
            SPEManager::instance()->dispatch(instruction);
        }

        for (MutableMatrix<float>::ElementIterator i(b.begin_elements()), i_end(b.element_at(instruction.transfer_begin())) ; i != i_end ; ++i)
        {
            *i *= a;
        }

        for (MutableMatrix<float>::ElementIterator i(b.element_at(instruction.transfer_end())), i_end(b.end_elements()) ; i != i_end ; ++i)
        {
            *i *= a;
        }

        if (instruction.use_spe())
            instruction.wait();

        return b;
    }

    DenseVectorContinuousBase<float> &
    Scale<tags::Cell>::value(DenseVectorContinuousBase<float> & b, const float a)
    {
        CONTEXT("When scaling DenseVectorContinuousBase<float> (Cell):");

        SPEFrameworkInstruction<1, float, cell::rtm_dma> instruction(oc_scale_dense_float, b.elements(), b.size(), a);

        if (instruction.use_spe())
        {
            SPEManager::instance()->dispatch(instruction);
        }

        for (Vector<float>::ElementIterator i(b.begin_elements()), i_end(b.element_at(instruction.transfer_begin())) ; i != i_end ; ++i)
        {
            *i *= a;
        }

        for (Vector<float>::ElementIterator i(b.element_at(instruction.transfer_end())), i_end(b.end_elements()) ; i != i_end ; ++i)
        {
            *i *= a;
        }

        if (instruction.use_spe())
            instruction.wait();

        return b;
    }

    SparseVector<float> &
    Scale<tags::Cell>::value(SparseVector<float> & b, const float a)
    {
        CONTEXT("When scaling SparseVector<float> (Cell):");

        Operand oa = { b.elements() };
        Operand ob;
        ob.u = b.used_elements() / (1024 * 4);
        Operand oc;
        oc.u = b.used_elements() % (1024 * 4);
        oc.u &= ~0xF;
        Operand od;
        od.f = a;

        unsigned rest_index(ob.u * 4096 + oc.u);

        oc.u *= 4;

        bool use_spe(true);

        if (0 == oc.u)
        {
            if (ob.u > 0)
            {
                oc.u = 16 * 1024;
            }
            else
            {
                use_spe = false;
            }
        }
        else
        {
            ++ob.u;
        }

        SPEInstruction instruction(oc_scale_dense_float, 16 * 1024, oa, ob, oc, od);

        if (use_spe)
        {
            SPEManager::instance()->dispatch(instruction);
        }

        for (unsigned i(rest_index); i < b.used_elements() ; i++)
        {
            *(b.elements() + i) *= a;
        }

        if (use_spe)
            instruction.wait();

        return b;

    }

    SparseMatrix<float> &
    Scale<tags::Cell>::value(SparseMatrix<float> & b, const float a)
    {
        CONTEXT("When scaling SparseMatrix<float> (Cell):");

        std::list<SPEInstruction *> instructions;
        std::vector<SPETransferList> lists = std::vector<SPETransferList>();

        lists.push_back(SPETransferList(2048, 16384));
        unsigned nr_lists(0), dispatched(0), size(0);

        for (SparseMatrix<float>::ConstRowIterator i(b.begin_non_zero_rows()), i_end(b.end_non_zero_rows()) ; i != i_end ; ++i)
        {
            float * address(i->elements());
            unsigned rowsize(i->used_elements() * sizeof(float));
            rowsize = rowsize % 16 == 0 ? rowsize : rowsize + 16 - (rowsize % 16);

            unsigned parts((rowsize / 16384) + 1);
            unsigned partrowsize(0);
            float * addresses[parts];

            for (unsigned x(0) ; x < parts ; x++)
            {
                addresses[x] = address + (x * 4096);
                partrowsize = rowsize < 16384 ? rowsize : 16384;
                ListElement * retval = lists.at(nr_lists).add(addresses[x], partrowsize);

                if (retval == 0)
                {
                    // Dispatch old list
                    Operand oa = { lists.at(nr_lists).elements() };
                    Operand ob = { lists.at(nr_lists).effective_address() };
                    Operand oc;
                    oc.f = a;
                    Operand od = { size };
                    Operand oe = { lists.at(nr_lists).size() % 2 == 0 ? lists.at(nr_lists).size() : lists.at(nr_lists).size() + 1 };

                    SPEInstruction * instruction = new SPEInstruction(oc_scale_sparse_float, lists.at(nr_lists).size(), oa, ob, oc, od, oe);
                    SPEManager::instance()->dispatch(*instruction);
                    instructions.push_back(instruction);
                    dispatched++;

                    // Add Element to new list
                    size = 0;
                    lists.push_back(SPETransferList(2048, 16384));
                    nr_lists++;
                    lists.at(nr_lists).add(addresses[x], partrowsize);
                    size += partrowsize;
                }
                else
                {
                    size += partrowsize;
                }

                rowsize -=partrowsize;
            }

        }

        if (dispatched == nr_lists && size != 0) // Maybe the last list didn't get full and therefore hasn't been dispatched
        {
            Operand oa = { lists.at(nr_lists).elements() };
            Operand ob = { lists.at(nr_lists).effective_address() };
            Operand oc; 
            oc.f = a;
            Operand od = { size };
            Operand oe = { lists.at(nr_lists).size() % 2 == 0 ? lists.at(nr_lists).size() : lists.at(nr_lists).size() + 1 };

            SPEInstruction * instruction = new SPEInstruction(oc_scale_sparse_float, lists.at(nr_lists).size(), oa, ob, oc, od, oe);
            SPEManager::instance()->dispatch(*instruction);
            instructions.push_back(instruction);
            dispatched++;
        }

        for(std::list<SPEInstruction *>::iterator i(instructions.begin()), i_end(instructions.end()) ; i != i_end ; ++i)
        {
                (*i)->wait();

                delete *i;
        }

        return b;
    }

}

