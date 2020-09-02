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
  auto is_write = (m.input(TOP_SLAVE_IF_WR) & ~m.input(TOP_SLAVE_IF_RD));
  // masked address.
  // "Mask off the top 8 bits, which represent the device memory map
  // offset from the CPU.""
  auto masked_addr = Concat(BvConst(0, 8), 
                            Extract(m.input(TOP_SLAVE_ADDR_IN), 23, 0));
  auto soc_mem_addr = m.state(CFG_REG_SOC_MEM_BASE_ADDR);
  auto axi_addr_out = m.state(TOP_MASTER_RD_ADDR_OUT);


  {// write data into SPAD0
    auto instr = m.NewInstr("SPAD0_DATA_WR");
    auto is_spad0_addr = (masked_addr >= SPAD0_BASE_ADDR) & (masked_addr < SPAD1_BASE_ADDR);
    
    instr.SetDecode(is_write & is_spad0_addr);

    // this part model the AXI master interface addr port
    auto soc_mem_addr = m.state(CFG_REG_SOC_MEM_BASE_ADDR);
    instr.SetUpdate(axi_addr_out, soc_mem_addr);

    // this part takes the data from the virtual memory for simulation
    auto spad = m.state(SCRATCH_PAD_0);
    auto spad_addr = masked_addr - SPAD0_BASE_ADDR;
    auto vir_mem = m.state(VIRTUAL_SOC_MEMORY);

    auto spad_next = spad;

    spad_next = Store(spad_next, spad_addr + 0, Load(vir_mem, soc_mem_addr + 0));
    spad_next = Store(spad_next, spad_addr + 1, Load(vir_mem, soc_mem_addr + 1));
    spad_next = Store(spad_next, spad_addr + 2, Load(vir_mem, soc_mem_addr + 2));
    spad_next = Store(spad_next, spad_addr + 3, Load(vir_mem, soc_mem_addr + 3));
    spad_next = Store(spad_next, spad_addr + 4, Load(vir_mem, soc_mem_addr + 4));
    spad_next = Store(spad_next, spad_addr + 5, Load(vir_mem, soc_mem_addr + 5));
    spad_next = Store(spad_next, spad_addr + 6, Load(vir_mem, soc_mem_addr + 6));
    spad_next = Store(spad_next, spad_addr + 7, Load(vir_mem, soc_mem_addr + 7));
    spad_next = Store(spad_next, spad_addr + 8, Load(vir_mem, soc_mem_addr + 8));
    spad_next = Store(spad_next, spad_addr + 9, Load(vir_mem, soc_mem_addr + 9));
    spad_next = Store(spad_next, spad_addr + 10, Load(vir_mem, soc_mem_addr + 10));
    spad_next = Store(spad_next, spad_addr + 11, Load(vir_mem, soc_mem_addr + 11));
    spad_next = Store(spad_next, spad_addr + 12, Load(vir_mem, soc_mem_addr + 12));
    spad_next = Store(spad_next, spad_addr + 13, Load(vir_mem, soc_mem_addr + 13));
    spad_next = Store(spad_next, spad_addr + 14, Load(vir_mem, soc_mem_addr + 14));
    spad_next = Store(spad_next, spad_addr + 15, Load(vir_mem, soc_mem_addr + 15));

    instr.SetUpdate(spad, spad_next);
  }

  {// write data into SPAD1
    auto instr = m.NewInstr("SPAD1_DATA_WR");
    auto is_spad1_addr = (masked_addr >= SPAD1_BASE_ADDR) & (masked_addr < MEM_ADDR_MAX);
    
    instr.SetDecode(is_write & is_spad1_addr);

    // this part model the AXI master interface addr port
    auto soc_mem_addr = m.state(CFG_REG_SOC_MEM_BASE_ADDR);
    instr.SetUpdate(axi_addr_out, soc_mem_addr);

    // this part takes the data from the virtual memory for simulation
    auto spad = m.state(SCRATCH_PAD_1);
    auto spad_addr = masked_addr - SPAD0_BASE_ADDR;
    auto vir_mem = m.state(VIRTUAL_SOC_MEMORY);

    auto spad_next = spad;

    spad_next = Store(spad_next, spad_addr + 0, Load(vir_mem, soc_mem_addr + 0));
    spad_next = Store(spad_next, spad_addr + 1, Load(vir_mem, soc_mem_addr + 1));
    spad_next = Store(spad_next, spad_addr + 2, Load(vir_mem, soc_mem_addr + 2));
    spad_next = Store(spad_next, spad_addr + 3, Load(vir_mem, soc_mem_addr + 3));
    spad_next = Store(spad_next, spad_addr + 4, Load(vir_mem, soc_mem_addr + 4));
    spad_next = Store(spad_next, spad_addr + 5, Load(vir_mem, soc_mem_addr + 5));
    spad_next = Store(spad_next, spad_addr + 6, Load(vir_mem, soc_mem_addr + 6));
    spad_next = Store(spad_next, spad_addr + 7, Load(vir_mem, soc_mem_addr + 7));
    spad_next = Store(spad_next, spad_addr + 8, Load(vir_mem, soc_mem_addr + 8));
    spad_next = Store(spad_next, spad_addr + 9, Load(vir_mem, soc_mem_addr + 9));
    spad_next = Store(spad_next, spad_addr + 10, Load(vir_mem, soc_mem_addr + 10));
    spad_next = Store(spad_next, spad_addr + 11, Load(vir_mem, soc_mem_addr + 11));
    spad_next = Store(spad_next, spad_addr + 12, Load(vir_mem, soc_mem_addr + 12));
    spad_next = Store(spad_next, spad_addr + 13, Load(vir_mem, soc_mem_addr + 13));
    spad_next = Store(spad_next, spad_addr + 14, Load(vir_mem, soc_mem_addr + 14));
    spad_next = Store(spad_next, spad_addr + 15, Load(vir_mem, soc_mem_addr + 15));

    instr.SetUpdate(spad, spad_next);
  }

}

}
}