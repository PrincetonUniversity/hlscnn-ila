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

// File: spad_instr.cc

#include <ilang/ilang++.h>
#include <hlscnn/hlscnn_top.h>

namespace ilang {
namespace hlscnn {

void DefineSPADInstr(Ila& m) {
  // define config write instructions
  auto is_write = (m.input(TOP_IF_WR) & ~m.input(TOP_IF_RD));
  // masked address.
  // "Mask off the top 8 bits, which represent the device memory map
  // offset from the CPU.""
  auto masked_addr = Concat(BvConst(0, 8), 
                            Extract(m.input(TOP_ADDR_IN), 23, 0));


  {// write data into SPAD0
    auto instr = m.NewInstr("SPAD0_DATA_WR");
    auto is_spad0_addr = (masked_addr >= SPAD0_BASE_ADDR) & (masked_addr < SPAD1_BASE_ADDR);
    
    instr.SetDecode(is_write & is_spad0_addr);

    // auto soc_mem_base_addr = m.state(CFG_REG_SOC_MEM_BASE_ADDR);
    // auto soc_mem_wr_length = m.state(CFG_REG_SOC_MEM_RD_WR_LENGTH);
    auto addr = masked_addr - SPAD0_BASE_ADDR;
    auto spad0 = m.state(SCRATCH_PAD_0);

    auto spad0_next = spad0;

    spad0_next = Store(spad0_next, addr+0, m.input(TOP_DATA_IN_0));
    spad0_next = Store(spad0_next, addr+1, m.input(TOP_DATA_IN_1));
    spad0_next = Store(spad0_next, addr+2, m.input(TOP_DATA_IN_2));
    spad0_next = Store(spad0_next, addr+3, m.input(TOP_DATA_IN_3));
    spad0_next = Store(spad0_next, addr+4, m.input(TOP_DATA_IN_4));
    spad0_next = Store(spad0_next, addr+5, m.input(TOP_DATA_IN_5));
    spad0_next = Store(spad0_next, addr+6, m.input(TOP_DATA_IN_6));
    spad0_next = Store(spad0_next, addr+7, m.input(TOP_DATA_IN_7));
    spad0_next = Store(spad0_next, addr+8, m.input(TOP_DATA_IN_8));
    spad0_next = Store(spad0_next, addr+9, m.input(TOP_DATA_IN_9));
    spad0_next = Store(spad0_next, addr+10, m.input(TOP_DATA_IN_10));
    spad0_next = Store(spad0_next, addr+11, m.input(TOP_DATA_IN_11));
    spad0_next = Store(spad0_next, addr+12, m.input(TOP_DATA_IN_12));
    spad0_next = Store(spad0_next, addr+13, m.input(TOP_DATA_IN_13));
    spad0_next = Store(spad0_next, addr+14, m.input(TOP_DATA_IN_14));
    spad0_next = Store(spad0_next, addr+15, m.input(TOP_DATA_IN_15));

    instr.SetUpdate(spad0, spad0_next);
  }

    {// write data into SPAD1
    auto instr = m.NewInstr("SPAD1_DATA_WR");
    auto is_spad1_addr = (masked_addr >= SPAD1_BASE_ADDR) & (masked_addr < MEM_ADDR_MAX);
    
    instr.SetDecode(is_write & is_spad1_addr);

    // auto soc_mem_base_addr = m.state(CFG_REG_SOC_MEM_BASE_ADDR);
    // auto soc_mem_wr_length = m.state(CFG_REG_SOC_MEM_RD_WR_LENGTH);
    auto addr = masked_addr - SPAD1_BASE_ADDR;
    auto spad1 = m.state(SCRATCH_PAD_1);

    auto spad1_next = spad1;

    spad1_next = Store(spad1_next, addr+0, m.input(TOP_DATA_IN_0));
    spad1_next = Store(spad1_next, addr+1, m.input(TOP_DATA_IN_1));
    spad1_next = Store(spad1_next, addr+2, m.input(TOP_DATA_IN_2));
    spad1_next = Store(spad1_next, addr+3, m.input(TOP_DATA_IN_3));
    spad1_next = Store(spad1_next, addr+4, m.input(TOP_DATA_IN_4));
    spad1_next = Store(spad1_next, addr+5, m.input(TOP_DATA_IN_5));
    spad1_next = Store(spad1_next, addr+6, m.input(TOP_DATA_IN_6));
    spad1_next = Store(spad1_next, addr+7, m.input(TOP_DATA_IN_7));
    spad1_next = Store(spad1_next, addr+8, m.input(TOP_DATA_IN_8));
    spad1_next = Store(spad1_next, addr+9, m.input(TOP_DATA_IN_9));
    spad1_next = Store(spad1_next, addr+10, m.input(TOP_DATA_IN_10));
    spad1_next = Store(spad1_next, addr+11, m.input(TOP_DATA_IN_11));
    spad1_next = Store(spad1_next, addr+12, m.input(TOP_DATA_IN_12));
    spad1_next = Store(spad1_next, addr+13, m.input(TOP_DATA_IN_13));
    spad1_next = Store(spad1_next, addr+14, m.input(TOP_DATA_IN_14));
    spad1_next = Store(spad1_next, addr+15, m.input(TOP_DATA_IN_15));

    instr.SetUpdate(spad1, spad1_next);
  }

}

}
}