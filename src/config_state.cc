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

// File: config_reg.cc
#include <ilang/ilang++.h>

#include <hlscnn/hlscnn_top.h>

namespace ilang {
namespace hlscnn {

void DefineConfigReg(Ila& m) {
  
  DefineCfgReg_helper(m, CFG_REG_SOC_MEM_BASE_ADDR);
  DefineCfgReg_helper(m, CFG_REG_SOC_MEM_RD_WR_LENGTH);
  
  DefineCfgReg_helper(m, CFG_REG_ACCEL_SPAD_CFG);

  // FC config regs
  DefineCfgReg_helper(m, CFG_REG_ACCEL_FC_START_FLAG_REG);
  DefineCfgReg_helper(m, CFG_REG_ACCEL_FC_WEIGHT_BASE);
  DefineCfgReg_helper(m, CFG_REG_ACCEL_FC_ACT_BASE);
  DefineCfgReg_helper(m, CFG_REG_ACCEL_FC_SIZE_CONFIG);
  DefineCfgReg_helper(m, CFG_REG_ACCEL_BIAS_ACT_CONFIG);
  
  // conv config regs
  DefineCfgReg_helper(m, CFG_REG_ACCEL_CONV_TRIGGER);
  
  DefineCfgReg_helper(m, CFG_REG_ACCEL_CONV_ACT_BASE_ADDR);
  DefineCfgReg_helper(m, CFG_REG_ACCEL_CONV_WEIGHT_BASE_ADDR);
  DefineCfgReg_helper(m, CFG_REG_ACCEL_CONV_OUTPUT_BASE_ADDR);
  
  DefineCfgReg_helper(m, CFG_REG_ACCEL_CONV_INPUT_SIZE_CFG);
  DefineCfgReg_helper(m, CFG_REG_ACCEL_CONV_OUTPUT_SIZE_CFG);
  DefineCfgReg_helper(m, CFG_REG_ACCEL_KERNEL_SIZE_CFG);

  DefineCfgReg_helper(m, CFG_REG_ACCEL_CONV_CHANNEL_CFG);

  // reduction config regs
  DefineCfgReg_helper(m, CFG_REG_ACCEL_REDUCTION_TRIGGER);
  
  DefineCfgReg_helper(m, CFG_REG_ACCEL_REDUCTION_INPUT_BASE_ADDR);
  DefineCfgReg_helper(m, CFG_REG_ACCEL_REDUCTION_OUTPUT_BASE_ADDR);

  DefineCfgReg_helper(m, CFG_REG_ACCEL_REDUCTION_INPUT_SIZE_CFG);
  DefineCfgReg_helper(m, CFG_REG_ACCEL_REDUCTION_BIAS_CONFIG);
}

void DefineFCParam(Ila& m) {

  m.NewBvState(FC_WEIGHT_BASE, FC_WEIGHT_BASE_BITWIDTH);
  m.NewBvState(FC_ACT_BASE, FC_ACT_BASE_BITWIDTH);
  m.NewBvState(FC_NUM_ROWS, FC_NUM_ROWS_BITWIDTH);
  m.NewBvState(FC_NUM_COLS, FC_NUM_COLS_BITWIDTH);
  m.NewBvState(FC_RUN_BIAS, FC_RUN_BIAS_BITWIDTH);
  m.NewBvState(FC_BATCH_SIZE, FC_BATCH_SIZE_BITWIDTH);
  m.NewBvState(FC_ACT_FUNC, FC_ACT_FUNC_BITWIDTH);
  m.NewBvState(FC_RELU_THRESHOLD, FC_RELU_THRESHOLD_BITWIDTH);
}

void DefineConvParam(Ila& m) {

  m.NewBvState(CONV_ACT_BASE, CONV_ACT_BASE_BITWIDTH);
  m.NewBvState(CONV_WEIGHT_BASE, CONV_WEIGHT_BASE_BITWIDTH);
  m.NewBvState(CONV_SPAD_OUTPUT_BASE, CONV_SPAD_OUTPUT_BASE_BITWIDTH);

  m.NewBvState(CONV_INPUT_ROW_NUM, CONV_INPUT_ROW_NUM_BITWIDTH);
  m.NewBvState(CONV_INPUT_COL_NUM, CONV_INPUT_COL_NUM_BITWIDTH);

  m.NewBvState(CONV_INPUT_CHAN_NUM, CONV_INPUT_CHAN_NUM_BITWIDTH);
  m.NewBvState(CONV_OUTPUT_CHAN_NUM, CONV_OUTPUT_CHAN_NUM_BITWIDTH);

  m.NewBvState(CONV_KERNEL_ROW_NUM, CONV_KERNEL_ROW_NUM_BITWIDTH);
  m.NewBvState(CONV_KERNEL_COL_NUM, CONV_KERNEL_COL_NUM_BITWIDTH);

  m.NewBvState(CONV_KERNEL_C_STRIDE, CONV_KERNEL_C_STRIDE_BITWIDTH);
  m.NewBvState(CONV_KERNEL_R_STRIDE, CONV_KERNEL_R_STRIDE_BITWIDTH);

  m.NewBvState(CONV_ENABLE_BIAS, CONV_ENABLE_BIAS_BITWIDTH);
  m.NewBvState(CONV_ENABLE_RELU, CONV_ENABLE_RELU_BITWIDTH);
  m.NewBvState(CONV_ENABLE_ACCUM, CONV_ENABLE_ACCUM_BITWIDTH);
  m.NewBvState(CONV_ENABLE_WB, CONV_ENABLE_WB_BITWIDTH);

  m.NewBvState(CONV_OFILTER_IDX, CONV_OFILTER_IDX_BITWIDTH);

  m.NewBvState(CONV_OUTPUT_ROW_NUM, CONV_OUTPUT_ROW_NUM_BITWIDTH);
  m.NewBvState(CONV_OUTPUT_COL_NUM, CONV_OUTPUT_COL_NUM_BITWIDTH);

  m.NewBvState(CONV_CHAN_BIAS, CONV_CHAN_BIAS_BITWIDTH);

}

void DefineReduceParam(Ila& m) {

  m.NewBvState(REDUCTION_INPUT_BASE_ADDR, REDUCTION_INPUT_BASE_ADDR_BITWIDTH);
  m.NewBvState(REDUCTION_OUTPUT_BASE_ADDR, REDUCTION_OUTPUT_BASE_ADDR_BITWIDTH);

  m.NewBvState(REDUCTION_INPUT_ROW_NUM, REDUCTION_INPUT_ROW_NUM_BITWIDTH);
  m.NewBvState(REDUCTION_INPUT_COL_NUM, REDUCITON_INPUT_COL_NUM_BITWIDTH);
  m.NewBvState(REDUCTION_INPUT_CHAN_NUM, REDUCTION_INPUT_CHAN_NUM_BITWIDTH);

  m.NewBvState(REDUCTION_ACT_FUNC, REDUCTION_ACT_FUNC_BITWIDTH);
  
  m.NewBvState(REDUCTION_BIAS, REDUCTION_BIAS_BITWIDTH);
  m.NewBvState(REDUCTION_RELU_THRESHOLD, REDUCTION_RELU_THRESHOLD_BITWIDTH);
}


}

}