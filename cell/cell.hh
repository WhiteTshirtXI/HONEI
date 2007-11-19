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

#ifndef CELL_GUARD_CELL_HH
#define CELL_GUARD_CELL_HH 1

#include <cell/opcodes.hh>
#include <cell/traits.hh>

namespace honei
{
    namespace cell
    {

#if defined(__PPU__)

        typedef void * EffectiveAddress;

        typedef unsigned int LocalStoreAddress;

        enum OpCode;

#elif defined(__SPU__)

        typedef unsigned long long EffectiveAddress;

        typedef void * LocalStoreAddress;

        template <typename T_> union Pointer;

        template <> union Pointer<float>
        {
            void * untyped;
            unsigned adjustable;
            float * typed;
            vector float * vectorised;
        };

        template <> union Pointer<unsigned long long>
        {
            void * untyped;
            unsigned adjustable;
            unsigned long long * typed;
            vector unsigned long long * vectorised;
        };

        template <typename T_> union Subscriptable;

        template <> union Subscriptable<float>
        {
            vector float value;
            float array[4];
        };

        template <> union Subscriptable<unsigned long>
        {
            vector unsigned long value;
            unsigned long array[4];
        };

#else
#  error "You should not include this header in anything but Cell source code!"
#endif

        template <typename T_> union MailableResult;

        template <> union MailableResult<float>
        {
            float value;
            unsigned int mail;
        };

        struct __attribute__((packed)) Environment
        {
            LocalStoreAddress begin;
            LocalStoreAddress end;
        };

        union Operand
        {
            EffectiveAddress ea;
            unsigned long long u;
            double d;
            float f;
            long long s;
        };

        struct __attribute__((packed)) Instruction
        {
            OpCode opcode;
            unsigned size;
            Operand a;
            Operand b;
            Operand c;
            Operand d;
            Operand e;
            Operand f;
            Operand g;
            Operand h;
            Operand i;
            Operand j;
            Operand k;
            Operand l;
            Operand m;
            Operand n;
            Operand o;
        };

        enum KernelMessages
        {
            km_instruction_finished = 1 << 0,
            km_result_dword,
            km_result_qword,
            km_unknown_opcode,
            km_debug_get,
            km_debug_put,
            km_debug_enter,
            km_debug_leave,
            km_debug_acquire_block,
            km_debug_release_block,
            km_debug_dump
        };
    }
}

#endif
