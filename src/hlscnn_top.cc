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

  // Define Instructions
  DefineConfigInstr(m);
  DefineSPADInstr(m);
  DefineAccelConvTrigger(m);

  // Define child instructions
  // DefineAXIMasterChild(m);
  DefineAccelConvChild(m);

  ILA_INFO << "spad0 base addr: " << std::hex << SPAD0_BASE_ADDR;
  ILA_INFO << "spad1 base addr: " << std::hex << SPAD1_BASE_ADDR;  


  return m;
}

}; // namespace hlscnn

}; // namespace ilang
