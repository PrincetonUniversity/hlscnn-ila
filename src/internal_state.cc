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

// File: internal_state.cc

#include <ilang/ilang++.h>
#include <hlscnn/hlscnn_top.h>

namespace ilang {
namespace hlscnn {

void DefineInternalState(Ila& m) {

  ////////////////////////////////////
  // AXI master interface states
  ////////////////////////////////////
  m.NewBvState(ACCEL_MASTER_AXI_CHILD_VALID_FLAG, ACCEL_MASTER_AXI_CHILD_VALID_FLAG_BITWIDTH);
  m.NewBvState(ACCEL_MASTER_AXI_CHILD_STATE, ACCEL_MASTER_AXI_CHILD_STATE_BITWIDTH);
  m.NewBvState(ACCEL_SPAD_WR_ADDR, ACCEL_SPAD_WR_ADDR_BITWIDTH);
  
  ////////////////////////////////////
  // conv internal state
  ///////////////////////////////////
  m.NewBvState(ACCEL_CONV_CHILD_VALID_FLAG, ACCEL_CONV_CHILD_VALID_FLAG_BITWIDTH);
  m.NewBvState(ACCEL_CONV_CHILD_STATE, ACCEL_CONV_CHILD_STATE_BITWIDTH);

}

} // namespace hlscnn 
} // namespace ilang