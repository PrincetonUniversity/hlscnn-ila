// =============================================================================
// MIT License
//
// Copyright (c) 2019 Princeton University
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
// =============================================================================

// File: vir_mem_instr.cc

// This file contains writes to virtual memory

#include <ilang/ilang++.h>
#include <hlscnn/hlscnn_top.h>

namespace ilang {
namespace hlscnn {

void DefineVirMemInstr(Ila& m) {

  { // write data into virtual memory
    auto instr = m.NewInstr("VIR_MEM_WR");
    
    auto is_write = (m.input(TOP_SLAVE_IF_WR) & ~m.input(TOP_SLAVE_IF_RD));
    auto addr_valid = ((m.input(TOP_SLAVE_ADDR_IN) >= VIRTUAL_SOC_MEMORY_ADDR_MIN) &
                        (m.input(TOP_SLAVE_ADDR_IN) < VIRTUAL_SOC_MEMORY_ADDR_MAX));
    auto is_vir_access = (m.input(VIRTUAL_SOC_ACCESS) == 1);
    instr.SetDecode(is_write & addr_valid & is_vir_access);

    auto vir_mem = m.state(VIRTUAL_SOC_MEMORY);
    auto vir_mem_next = vir_mem;

    auto addr = m.input(TOP_SLAVE_ADDR_IN) - VIRTUAL_SOC_MEMORY_ADDR_MIN;

    vir_mem_next = Store(vir_mem_next, addr + 0, m.input(TOP_SLAVE_DATA_IN_0));
    vir_mem_next = Store(vir_mem_next, addr + 1, m.input(TOP_SLAVE_DATA_IN_1));
    vir_mem_next = Store(vir_mem_next, addr + 2, m.input(TOP_SLAVE_DATA_IN_2));
    vir_mem_next = Store(vir_mem_next, addr + 3, m.input(TOP_SLAVE_DATA_IN_3));
    vir_mem_next = Store(vir_mem_next, addr + 4, m.input(TOP_SLAVE_DATA_IN_4));
    vir_mem_next = Store(vir_mem_next, addr + 5, m.input(TOP_SLAVE_DATA_IN_5));
    vir_mem_next = Store(vir_mem_next, addr + 6, m.input(TOP_SLAVE_DATA_IN_6));
    vir_mem_next = Store(vir_mem_next, addr + 7, m.input(TOP_SLAVE_DATA_IN_7));
    vir_mem_next = Store(vir_mem_next, addr + 8, m.input(TOP_SLAVE_DATA_IN_8));
    vir_mem_next = Store(vir_mem_next, addr + 9, m.input(TOP_SLAVE_DATA_IN_9));
    vir_mem_next = Store(vir_mem_next, addr + 10, m.input(TOP_SLAVE_DATA_IN_10));
    vir_mem_next = Store(vir_mem_next, addr + 11, m.input(TOP_SLAVE_DATA_IN_11));
    vir_mem_next = Store(vir_mem_next, addr + 12, m.input(TOP_SLAVE_DATA_IN_12));
    vir_mem_next = Store(vir_mem_next, addr + 13, m.input(TOP_SLAVE_DATA_IN_13));
    vir_mem_next = Store(vir_mem_next, addr + 14, m.input(TOP_SLAVE_DATA_IN_14));
    vir_mem_next = Store(vir_mem_next, addr + 15, m.input(TOP_SLAVE_DATA_IN_15));

    instr.SetUpdate(vir_mem, vir_mem_next);
  }
}

}
}