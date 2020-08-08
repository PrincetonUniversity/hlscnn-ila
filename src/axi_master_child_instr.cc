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

// File: axi_master_child_instr.cc

#include <ilang/ilang++.h>
#include <hlscnn/hlscnn_top.h>

namespace ilang {
namespace hlscnn {

void DefineAXIMasterChild(Ila& m) {

  auto child = m.NewChild("ACCEL_AXI_MASTER_CHILD");
  auto is_child_valid = 
    (m.state(ACCEL_MASTER_AXI_CHILD_VALID_FLAG) == ACCEL_MASTER_AXI_CHILD_VALID);
  
  child.SetValid(is_child_valid);

  auto state = m.state(ACCEL_MASTER_AXI_CHILD_STATE);
  auto rd_resp_valid_flag = m.input(TOP_MASTER_RD_RESP_VALID_FLAG);

  child.AddInit(rd_resp_valid_flag == ACCEL_MASTER_AXI_CHILD_INVALID);

  { // instr 0 ---- AXI master read req for weights or activations to MEM ()
    auto instr = child.NewInstr("accel_axi_master_rd_req_spad0");
    auto is_instr_valid = ((rd_resp_valid_flag == ACCEL_MASTER_AXI_CHILD_INVALID) &
                            ((state == MASTER_AXI_CHILD_STATE_SPAD0_RD) |  
                              (state == MASTER_AXI_CHILD_STATE_SPAD1_RD)));

    instr.SetDecode(is_instr_valid);

    auto addr = m.state(CFG_REG_SOC_MEM_BASE_ADDR);
    // the number of beat / burst length is set to be 1
    auto num_beat = m.state(CFG_REG_SOC_MEM_RD_WR_LENGTH); 

    instr.SetUpdate(m.state(TOP_MASTER_IF_RD), 
                    BvConst(ACCEL_MASTER_AXI_CHILD_VALID, TOP_MASTER_IF_RD_BITWIDTH));
    instr.SetUpdate(m.state(TOP_MASTER_RD_ADDR_OUT), addr);
  }

  { // instr 1 ---- AXI master receive the data from mem and write it into SPAD
    auto instr = child.NewInstr("accel_axi_master_rd_recv_spad0");
    auto is_instr_valid = ((rd_resp_valid_flag == ACCEL_MASTER_AXI_CHILD_VALID) &
                            ((state == MASTER_AXI_CHILD_STATE_SPAD0_RD) |  
                              (state == MASTER_AXI_CHILD_STATE_SPAD1_RD)));

    instr.SetDecode(is_instr_valid);

    auto spad_addr = m.state(ACCEL_SPAD_WR_ADDR);
    auto spad = Ite(state == MASTER_AXI_CHILD_STATE_SPAD0_RD,
                    m.state(SCRATCH_PAD_0), m.state(SCRATCH_PAD_1));

    auto spad_next = spad;

    spad_next = Store(spad_next, spad_addr+0, m.input(TOP_MASTER_DATA_IN_0));
    spad_next = Store(spad_next, spad_addr+1, m.input(TOP_MASTER_DATA_IN_1));
    spad_next = Store(spad_next, spad_addr+2, m.input(TOP_MASTER_DATA_IN_2));
    spad_next = Store(spad_next, spad_addr+3, m.input(TOP_MASTER_DATA_IN_3));
    spad_next = Store(spad_next, spad_addr+4, m.input(TOP_MASTER_DATA_IN_4));
    spad_next = Store(spad_next, spad_addr+5, m.input(TOP_MASTER_DATA_IN_5));
    spad_next = Store(spad_next, spad_addr+6, m.input(TOP_MASTER_DATA_IN_6));
    spad_next = Store(spad_next, spad_addr+7, m.input(TOP_MASTER_DATA_IN_7));
    spad_next = Store(spad_next, spad_addr+8, m.input(TOP_MASTER_DATA_IN_8));
    spad_next = Store(spad_next, spad_addr+9, m.input(TOP_MASTER_DATA_IN_9));
    spad_next = Store(spad_next, spad_addr+10, m.input(TOP_MASTER_DATA_IN_10));
    spad_next = Store(spad_next, spad_addr+11, m.input(TOP_MASTER_DATA_IN_11));
    spad_next = Store(spad_next, spad_addr+12, m.input(TOP_MASTER_DATA_IN_12));
    spad_next = Store(spad_next, spad_addr+13, m.input(TOP_MASTER_DATA_IN_13));
    spad_next = Store(spad_next, spad_addr+14, m.input(TOP_MASTER_DATA_IN_14));
    spad_next = Store(spad_next, spad_addr+15, m.input(TOP_MASTER_DATA_IN_15));

    instr.SetUpdate(spad, spad_next);
    
    instr.SetUpdate(m.state(TOP_MASTER_IF_RD), 
                    BvConst(ACCEL_MASTER_AXI_CHILD_INVALID, TOP_MASTER_IF_RD_BITWIDTH));

    instr.SetUpdate(m.state(ACCEL_MASTER_AXI_CHILD_VALID_FLAG),
                    BvConst(ACCEL_MASTER_AXI_CHILD_INVALID, TOP_MASTER_IF_RD_BITWIDTH));
  }
  

}

} // namespace hlscnn
} // namespace ilang