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

// File: hlscnn_top.cc

#include <ilang/ilang++.h>

#include <hlscnn/hlscnn_top.h>
#include <ilang/util/log.h>

namespace ilang {
namespace hlscnn {

Ila GetHlscnnIla(const std::string& model_name) {
  auto m = Ila(model_name);

  SetUnsignedComparison(true);
  // model valid function

  // Define top input
  DefineTopIO(m);
  // Define configuration states
  DefineConfigReg(m);
  DefineFCParam(m);
  DefineConvParam(m);
  DefineReduceParam(m);

  // Define Arch states
  DefineArchState(m);
  DefineInternalState(m);

  // Define Init Conditions
  DefineInitCond(m);

  // Define valid condition
  auto valid_req = (m.input(TOP_SLAVE_IF_RD) ^ m.input(TOP_SLAVE_IF_WR));
  auto valid_addr = (m.input(TOP_SLAVE_ADDR_IN) <= ADDR_IN_MAX);
  m.SetValid(valid_req & valid_addr);


  // Define Instructions
  DefineConfigInstr(m);
  DefineSPADInstr(m);
  DefineAccelConvTrigger(m);

  DefineVirMemInstr(m);
  // Define child instructions
  // // DefineAXIMasterChild(m);
  DefineAccelConvChild(m);
  DefineSPADInstrChild(m);

  ILA_INFO << "spad0 base addr: 0x" << std::hex << SPAD0_BASE_ADDR;
  ILA_INFO << "spad1 base addr: 0x" << std::hex << SPAD1_BASE_ADDR;
  ILA_INFO << "spad1 max addr: 0x" << std::hex << MEM_ADDR_MAX;
  ILA_INFO << "vir_mem_base_addr: 0x" << std::hex << VIRTUAL_SOC_MEMORY_ADDR_MIN; 
  ILA_INFO << "vir_mem max addr: 0x" << std::hex << VIRTUAL_SOC_MEMORY_ADDR_MAX; 

  // // redundant uninterpreted functions for debug
  // m.NewBvState("debug_temp", ACT_TOTAL_BITWIDTH);
  // {
  //   auto instr = m.NewInstr("debug_uf");
  //   instr.SetDecode(BoolConst(false));
  //   auto wt_temp = BvConst(1, WEIGHT_TOTAL_BITWIDTH);
  //   auto psum_temp = BvConst(1, PSUM_TOTAL_BITWIDTH);
  //   auto act_temp = BvConst(1, ACT_TOTAL_BITWIDTH);

  //   auto tmp = ConvMacPsumMul(wt_temp, wt_temp);
  //   tmp = Psum2Act(tmp);
  //   tmp = ActAdd(tmp, tmp);
  //   tmp = ActRelu(tmp);
  //   instr.SetUpdate(m.state("debug_temp"), tmp);
  // }


  return m;
}

}; // namespace hlscnn

}; // namespace ilang
