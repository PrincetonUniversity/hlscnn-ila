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

// File: config_instr.cc

#include <ilang/ilang++.h>
#include <hlscnn/hlscnn_top.h>

namespace ilang {
namespace hlscnn {

void DefineConfigInstr(Ila& m) {
  
  // define config write instructions
  auto is_write = (m.input(TOP_SLAVE_IF_WR) & ~m.input(TOP_SLAVE_IF_RD));
  // masked address.
  // "Mask off the top 8 bits, which represent the device memory map
  // offset from the CPU.""
  auto masked_addr = Concat(BvConst(0, 8), 
                            Extract(m.input(TOP_SLAVE_ADDR_IN), 23, 0));
  auto is_config_addr = (masked_addr < SPAD0_BASE_ADDR);

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
  
  // there are 3 special config reg, FC AccelStartFlag, AccelConvTrigger, AccelReduceTrigger
  
  { // write FC AccelStartFlagReg
    auto instr = m.NewInstr("CFG_REG_WR_ACCEL_START_FLAG_REG");

    instr.SetDecode(is_write & is_config_addr & (reg_id == AccelStartFlagReg));

    auto remote_base_addr = m.state(CFG_REG_ACCEL_FC_WEIGHT_BASE);
    auto act_base_addr = m.state(CFG_REG_ACCEL_FC_ACT_BASE);
    auto size_config = m.state(CFG_REG_ACCEL_FC_SIZE_CONFIG);
    auto bias_activation = m.state(CFG_REG_ACCEL_BIAS_ACT_CONFIG);

    instr.SetUpdate(m.state(FC_WEIGHT_BASE), remote_base_addr);
    instr.SetUpdate(m.state(FC_ACT_BASE), act_base_addr);
    instr.SetUpdate(m.state(FC_BATCH_SIZE), Extract(size_config, 26, 24));
    instr.SetUpdate(m.state(FC_NUM_ROWS), Extract(size_config, 23, 12));
    instr.SetUpdate(m.state(FC_NUM_COLS), Extract(size_config, 11, 0));
    instr.SetUpdate(m.state(FC_ACT_FUNC), Extract(bias_activation, ACT_FUNC_WIDTH - 1, 0));
    instr.SetUpdate(m.state(FC_RELU_THRESHOLD), Extract(bias_activation, 24, 3));
    
    // TODO: Push FC trigger here

  }

  // update 08052020: moved to AccelConvTrigger.cc
  // { // write CONV AccelConvTrigger
  //   auto instr = m.NewInstr("CFG_REG_WR_ACCEL_CONV_TRIGGER");

  //   instr.SetDecode(is_write & is_config_addr & (reg_id == AccelConvTrigger));

  //   auto input_size_config = m.state(CFG_REG_ACCEL_CONV_INPUT_SIZE_CFG);
  //   auto output_size_config = m.state(CFG_REG_ACCEL_CONV_OUTPUT_SIZE_CFG);
  //   auto kernel_size_config = m.state(CFG_REG_ACCEL_KERNEL_SIZE_CFG);
  //   auto channel_config = m.state(CFG_REG_ACCEL_CONV_CHANNEL_CFG);
    
  //   instr.SetUpdate(m.state(CONV_ACT_BASE), m.state(CFG_REG_ACCEL_CONV_ACT_BASE_ADDR));
  //   instr.SetUpdate(m.state(CONV_WEIGHT_BASE), m.state(CFG_REG_ACCEL_CONV_WEIGHT_BASE_ADDR));
  //   instr.SetUpdate(m.state(CONV_SPAD_OUTPUT_BASE), m.state(CFG_REG_ACCEL_CONV_OUTPUT_BASE_ADDR));
    
  //   instr.SetUpdate(m.state(CONV_INPUT_COL_NUM), Extract(input_size_config, 9, 0));
  //   instr.SetUpdate(m.state(CONV_INPUT_ROW_NUM), Extract(input_size_config, 19, 10));
  //   instr.SetUpdate(m.state(CONV_INPUT_CHAN_NUM), Extract(input_size_config, 31, 20));

  //   instr.SetUpdate(m.state(CONV_OUTPUT_COL_NUM), Extract(output_size_config, 9, 0));
  //   instr.SetUpdate(m.state(CONV_OUTPUT_ROW_NUM), Extract(output_size_config, 19, 10));
  //   instr.SetUpdate(m.state(CONV_OUTPUT_CHAN_NUM), Extract(output_size_config, 31, 20));

  //   instr.SetUpdate(m.state(CONV_KERNEL_COL_NUM), Extract(kernel_size_config, 7, 0));
  //   instr.SetUpdate(m.state(CONV_KERNEL_ROW_NUM), Extract(kernel_size_config, 15, 8));
  //   instr.SetUpdate(m.state(CONV_KERNEL_C_STRIDE), Extract(kernel_size_config, 18, 16));
  //   instr.SetUpdate(m.state(CONV_KERNEL_R_STRIDE), Extract(kernel_size_config, 21, 19));

  //   instr.SetUpdate(m.state(CONV_CHAN_BIAS), Extract(channel_config, 15, 0));
    
  //   instr.SetUpdate(m.state(CONV_ENABLE_BIAS), SelectBit(channel_config, 16));
  //   instr.SetUpdate(m.state(CONV_ENABLE_RELU), SelectBit(channel_config, 17));
  //   instr.SetUpdate(m.state(CONV_ENABLE_ACCUM), SelectBit(channel_config, 18));
  //   instr.SetUpdate(m.state(CONV_OFILTER_IDX), Extract(channel_config, 26, 19));
  //   instr.SetUpdate(m.state(CONV_ENABLE_WB), SelectBit(channel_config, 27));

  //   // TODO: Push Conv Trigger here

  // }

  { // write ReductionTrigger
    auto instr = m.NewInstr("CFG_REG_WR_ACCEL_REDUCTION_TRIGGER");

    instr.SetDecode(is_write & is_config_addr & (reg_id == AccelReductionTrigger));

    auto input_base_addr = m.state(CFG_REG_ACCEL_REDUCTION_INPUT_BASE_ADDR);
    auto output_base_addr = m.state(CFG_REG_ACCEL_REDUCTION_OUTPUT_BASE_ADDR);
    auto input_size_config = m.state(CFG_REG_ACCEL_REDUCTION_INPUT_SIZE_CFG);
    auto reduction_bias_config = m.state(CFG_REG_ACCEL_REDUCTION_BIAS_CONFIG);
    auto general_bias_config = m.state(CFG_REG_ACCEL_BIAS_ACT_CONFIG);

    instr.SetUpdate(m.state(REDUCTION_INPUT_BASE_ADDR), input_base_addr);
    instr.SetUpdate(m.state(REDUCTION_OUTPUT_BASE_ADDR), output_base_addr);
    
    instr.SetUpdate(m.state(REDUCTION_INPUT_COL_NUM), Extract(input_size_config, 9, 0));
    instr.SetUpdate(m.state(REDUCTION_INPUT_ROW_NUM), Extract(input_size_config, 19, 10));
    instr.SetUpdate(m.state(REDUCTION_INPUT_CHAN_NUM), Extract(input_size_config, 29, 20));

    instr.SetUpdate(m.state(REDUCTION_BIAS), Extract(reduction_bias_config, 15, 0));

    instr.SetUpdate(m.state(REDUCTION_RELU_THRESHOLD), Extract(general_bias_config, 24, 3));
    instr.SetUpdate(m.state(REDUCTION_ACT_FUNC), Extract(general_bias_config, 1, 0));
    
    // TODO: Push Reduction trigger here

  }

  // other config register wr instrucitons
  SetConfigRegWrInstr(m, SocMemBaseAddr, CFG_REG_SOC_MEM_BASE_ADDR);
  SetConfigRegWrInstr(m, SocMemRdWrLength, CFG_REG_SOC_MEM_RD_WR_LENGTH);

  SetConfigRegWrInstr(m, AccelSpadCFG, CFG_REG_ACCEL_SPAD_CFG);

  // SetConfigRegWrInstr(m, AccelStartFlagReg, CFG_REG_ACCEL_FC_START_FLAG_REG);
  SetConfigRegWrInstr(m, AccelFCWeightsBase, CFG_REG_ACCEL_FC_WEIGHT_BASE);
  SetConfigRegWrInstr(m, AccelFCActivationBase, CFG_REG_ACCEL_FC_ACT_BASE);
  SetConfigRegWrInstr(m, AccelFCSizeConfig, CFG_REG_ACCEL_FC_SIZE_CONFIG);
  SetConfigRegWrInstr(m, AccelBiasActivationConfig, CFG_REG_ACCEL_BIAS_ACT_CONFIG);
  
  SetConfigRegWrInstr(m, AccelConvActivationBaseAddr, CFG_REG_ACCEL_CONV_ACT_BASE_ADDR);
  SetConfigRegWrInstr(m, AccelConvWeightsBaseAddr, CFG_REG_ACCEL_CONV_WEIGHT_BASE_ADDR);
  SetConfigRegWrInstr(m, AccelConvOutputsBaseAddr, CFG_REG_ACCEL_CONV_OUTPUT_BASE_ADDR);
  SetConfigRegWrInstr(m, AccelConvInputSizeConfig, CFG_REG_ACCEL_CONV_INPUT_SIZE_CFG);
  SetConfigRegWrInstr(m, AccelConvOutputSizeConfig, CFG_REG_ACCEL_CONV_OUTPUT_SIZE_CFG);
  SetConfigRegWrInstr(m, AccelConvKernelSizeConfig, CFG_REG_ACCEL_KERNEL_SIZE_CFG);
  SetConfigRegWrInstr(m, AccelConvChannelConfig, CFG_REG_ACCEL_CONV_CHANNEL_CFG);

  SetConfigRegWrInstr(m, AccelReductionInputBaseAddr, CFG_REG_ACCEL_REDUCTION_INPUT_BASE_ADDR);
  SetConfigRegWrInstr(m, AccelReductionOutputBaseAddr, CFG_REG_ACCEL_REDUCTION_OUTPUT_BASE_ADDR);
  SetConfigRegWrInstr(m, AccelReductionInputSizeConfig, CFG_REG_ACCEL_REDUCTION_INPUT_SIZE_CFG);
  SetConfigRegWrInstr(m, AccelReductionBiasConfig, CFG_REG_ACCEL_REDUCTION_BIAS_CONFIG);

}

} // namespace hlscnn
} // namespace ilang