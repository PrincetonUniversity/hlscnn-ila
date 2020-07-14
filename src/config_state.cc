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

    DefineCfgReg_helper(m, CFG_REG_ACCEL_CHANNEL_CFG);

    // reduction config regs
    DefineCfgReg_helper(m, CFG_REG_ACCEL_REDUCTION_TRIGGER);
    
    DefineCfgReg_helper(m, CFG_REG_ACCEL_REDUCTION_INPUT_BASE_ADDR);
    DefineCfgReg_helper(m, CFG_REG_ACCEL_REDUCTION_OUTPUT_BASE_ADDR);

    DefineCfgReg_helper(m, CFG_REG_ACCEL_REDUCTION_INPUT_SIZE_CFG);
    DefineCfgReg_helper(m, CFG_REG_ACCEL_REDUCTION_BIAS_CONFIG);
  }

}

}