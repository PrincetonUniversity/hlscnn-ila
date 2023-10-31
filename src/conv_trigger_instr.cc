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

// File: AccelConvTrigger.cc

#include <ilang/ilang++.h>
#include <hlscnn/hlscnn_top.h>

namespace ilang {
namespace hlscnn {

void DefineAccelConvTrigger(Ila& m) {
  // define config write instructions
  auto is_write = (m.input(TOP_SLAVE_IF_WR) & ~m.input(TOP_SLAVE_IF_RD));
  // masked address.
  // "Mask off the top 8 bits, which represent the device memory map
  // offset from the CPU.""
  auto masked_addr = Concat(BvConst(0, 8), 
                            Extract(m.input(TOP_SLAVE_ADDR_IN), 23, 0));
  auto not_vir_access = (m.input(VIRTUAL_SOC_ACCESS) == 0);
  auto is_config_addr = (masked_addr < SPAD0_BASE_ADDR) & not_vir_access;

  // get the aligned data. Reg data is only 32 bit

  // " For config reg writes only:
  //
  // Get the right 32-bit slice of the 128-bit data. This can be determined
  // by looking at the last four bits of the address - 128-bit data is 16
  // bytes, which takes 4 bits of addressing. Take that value and multiply
  // it by 8 (8 bits per byte) to get the start of the right 32-bit slice.
  // "

  // auto aligned_data = GetCfgRegAlignedData(m);

  auto reg_id = URem(masked_addr >> CFG_REG_SIZE_BITWIDTH,
                     BvConst(NumCfgRegisters, masked_addr.bit_width()));

  { // instr: AccelConvTrigger
    auto instr = m.NewInstr("ACCEL_CONV_TRIGGER");

    instr.SetDecode(is_write & is_config_addr & (reg_id == AccelConvTrigger));

    auto input_size_config = m.state(CFG_REG_ACCEL_CONV_INPUT_SIZE_CFG);
    auto output_size_config = m.state(CFG_REG_ACCEL_CONV_OUTPUT_SIZE_CFG);
    auto kernel_size_config = m.state(CFG_REG_ACCEL_KERNEL_SIZE_CFG);
    auto channel_config = m.state(CFG_REG_ACCEL_CONV_CHANNEL_CFG);
    
    instr.SetUpdate(m.state(CONV_ACT_BASE), m.state(CFG_REG_ACCEL_CONV_ACT_BASE_ADDR));
    instr.SetUpdate(m.state(CONV_WEIGHT_BASE), m.state(CFG_REG_ACCEL_CONV_WEIGHT_BASE_ADDR));
    instr.SetUpdate(m.state(CONV_SPAD_OUTPUT_BASE), m.state(CFG_REG_ACCEL_CONV_OUTPUT_BASE_ADDR));
    
    instr.SetUpdate(m.state(CONV_INPUT_COL_NUM), Extract(input_size_config, 9, 0));
    instr.SetUpdate(m.state(CONV_INPUT_ROW_NUM), Extract(input_size_config, 19, 10));
    instr.SetUpdate(m.state(CONV_INPUT_CHAN_NUM), Extract(input_size_config, 31, 20));

    instr.SetUpdate(m.state(CONV_OUTPUT_COL_NUM), Extract(output_size_config, 9, 0));
    instr.SetUpdate(m.state(CONV_OUTPUT_ROW_NUM), Extract(output_size_config, 19, 10));
    instr.SetUpdate(m.state(CONV_OUTPUT_CHAN_NUM), Extract(output_size_config, 31, 20));

    instr.SetUpdate(m.state(CONV_KERNEL_COL_NUM), Extract(kernel_size_config, 7, 0));
    instr.SetUpdate(m.state(CONV_KERNEL_ROW_NUM), Extract(kernel_size_config, 15, 8));
    instr.SetUpdate(m.state(CONV_KERNEL_C_STRIDE), Extract(kernel_size_config, 18, 16));
    instr.SetUpdate(m.state(CONV_KERNEL_R_STRIDE), Extract(kernel_size_config, 21, 19));

    instr.SetUpdate(m.state(CONV_CHAN_BIAS), Extract(channel_config, 15, 0));
    
    instr.SetUpdate(m.state(CONV_ENABLE_BIAS), SelectBit(channel_config, 16));
    instr.SetUpdate(m.state(CONV_ENABLE_RELU), SelectBit(channel_config, 17));
    instr.SetUpdate(m.state(CONV_ENABLE_ACCUM), SelectBit(channel_config, 18));
    // 10312023: extend the conv_ofilter_idx bitwidth to 12, the same as the conv output channel number
    instr.SetUpdate(m.state(CONV_OFILTER_IDX), Extract(channel_config, 30, 19));
    // 10312023: move the enable wb bit to the last bit
    instr.SetUpdate(m.state(CONV_ENABLE_WB), SelectBit(channel_config, 31));

    // set the child valid flag
    auto child_valid_flag = m.state(ACCEL_CONV_CHILD_VALID_FLAG);
    instr.SetUpdate(child_valid_flag, 
                    BvConst(ACCEL_CONV_CHILD_VALID, ACCEL_CONV_CHILD_VALID_FLAG_BITWIDTH));
    instr.SetUpdate(m.state(ACCEL_CONV_CHILD_STATE),
                    BvConst(CONV_CHILD_STATE_IDLE, ACCEL_CONV_CHILD_STATE_BITWIDTH));
    
    // the declaration of child is moved to top.
    // DefineAccelConvChild(m);
  }
}

} // namespace hlscnn
} // namespace ilang