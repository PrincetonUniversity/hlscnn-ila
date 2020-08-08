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
  
  auto spad_addr = m.state(ACCEL_SPAD_WR_ADDR);
  auto axi_valid_flag = m.state(ACCEL_MASTER_AXI_CHILD_VALID_FLAG);
  auto axi_state = m.state(ACCEL_MASTER_AXI_CHILD_STATE);


  {// write data into SPAD0
    auto instr = m.NewInstr("SPAD0_DATA_WR");
    auto is_spad0_addr = (masked_addr >= SPAD0_BASE_ADDR) & (masked_addr < SPAD1_BASE_ADDR);
    
    instr.SetDecode(is_write & is_spad0_addr);

    // auto soc_mem_base_addr = m.state(CFG_REG_SOC_MEM_BASE_ADDR);
    // auto soc_mem_wr_length = m.state(CFG_REG_SOC_MEM_RD_WR_LENGTH);
    
    auto spad_addr_next = masked_addr - SPAD0_BASE_ADDR;
    
    instr.SetUpdate(spad_addr, spad_addr_next);
    instr.SetUpdate(axi_valid_flag, 
      BvConst(ACCEL_MASTER_AXI_CHILD_VALID, ACCEL_MASTER_AXI_CHILD_VALID_FLAG_BITWIDTH));
    instr.SetUpdate(axi_state,
      BvConst(MASTER_AXI_CHILD_STATE_SPAD0_RD, ACCEL_MASTER_AXI_CHILD_STATE_BITWIDTH));
  }

  {// write data into SPAD1
    auto instr = m.NewInstr("SPAD1_DATA_WR");
    auto is_spad1_addr = (masked_addr >= SPAD1_BASE_ADDR) & (masked_addr < MEM_ADDR_MAX);
    
    instr.SetDecode(is_write & is_spad1_addr);

    // auto soc_mem_base_addr = m.state(CFG_REG_SOC_MEM_BASE_ADDR);
    // auto soc_mem_wr_length = m.state(CFG_REG_SOC_MEM_RD_WR_LENGTH);
    auto spad_addr_next = masked_addr - SPAD1_BASE_ADDR;

    instr.SetUpdate(spad_addr, spad_addr_next);
    instr.SetUpdate(axi_valid_flag, 
      BvConst(ACCEL_MASTER_AXI_CHILD_VALID, ACCEL_MASTER_AXI_CHILD_VALID_FLAG_BITWIDTH));
    instr.SetUpdate(axi_state,
      BvConst(MASTER_AXI_CHILD_STATE_SPAD1_RD, ACCEL_MASTER_AXI_CHILD_STATE_BITWIDTH));
  }

}

}
}