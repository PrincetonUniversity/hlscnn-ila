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

// File: conv_child_instr.cc

#include <ilang/ilang++.h>
#include <hlscnn/hlscnn_top.h>
#include <ilang/util/log.h>

namespace ilang {
namespace hlscnn {

void DefineConvActFetch(Ila& child);
void DefineConvWeightFetch(Ila& child);
void DefineConvDatapath(Ila& child);

void DefineAccelConvChild(Ila& m) {
  auto child = m.NewChild("Accel_Conv_Child");
  auto child_valid_flag = m.state(ACCEL_CONV_CHILD_VALID_FLAG);
  
  child.SetValid(child_valid_flag == ACCEL_CONV_CHILD_VALID);

  // Declare child states
  auto state = child.NewBvState(ACCEL_CONV_CHILD_STATE, ACCEL_CONV_CHILD_STATE_BITWIDTH);

  child.NewBvState(CONV_CHILD_FILTER_ID, CONV_CHILD_FILTER_ID_BITWIDTH);
  child.NewBvState(CONV_CHILD_CHAN_BLOCK_ID, CONV_CHILD_CHAN_BLOCK_ID_BITWIDTH);
  child.NewBvState(CONV_CHILD_INPUT_ROW_ID, CONV_CHILD_INPUT_ROW_ID_BITWIDTH);
  child.NewBvState(CONV_CHILD_INPUT_COL_ID, CONV_CHILD_INPUT_COL_ID_BITWIDTH);

  child.AddInit(state == CONV_CHILD_STATE_IDLE);

  ILA_INFO << "child_state number: " << child.state_num();
  ILA_INFO << child.state(CONV_INPUT_CHAN_NUM);;

  // Declare child instructions, seperating activation fetching, weigth fetching 
  // and datapath
  DefineConvActFetch(child);
  DefineConvWeightFetch(child);
  // DefineConvDatapath(child);
  
}

void DefineConvActFetch(Ila& child) {
  
  auto state = child.state(ACCEL_CONV_CHILD_STATE);

  // child states
  auto filter_idx = child.state(CONV_CHILD_FILTER_ID);
  auto chan_block = child.state(CONV_CHILD_CHAN_BLOCK_ID);
  auto input_row = child.state(CONV_CHILD_INPUT_ROW_ID);
  auto input_col = child.state(CONV_CHILD_INPUT_COL_ID);

  { // instr ---- start Activation fetching
    auto instr = child.NewInstr("accel_conv_child_start");
    instr.SetDecode(state == CONV_CHILD_STATE_IDLE);

    instr.SetUpdate(filter_idx, BvConst(0, CONV_CHILD_FILTER_ID_BITWIDTH));
    instr.SetUpdate(chan_block, BvConst(0, CONV_CHILD_CHAN_BLOCK_ID_BITWIDTH));
    instr.SetUpdate(input_row, BvConst(0, CONV_CHILD_INPUT_ROW_ID_BITWIDTH));
    instr.SetUpdate(input_col, BvConst(0, CONV_CHILD_INPUT_COL_ID_BITWIDTH));

    // at the start, the next state should directly jump to the weight fetching!
    auto next_state = BvConst(CONV_CHILD_STATE_ACT_FILTER_ID,
                              ACCEL_CONV_CHILD_STATE_BITWIDTH);
    
    instr.SetUpdate(state, next_state);
  }

  { // instr ---- setting filter_idx
    auto instr = child.NewInstr("accel_conv_child_filter_idx");
    instr.SetDecode(state == CONV_CHILD_STATE_ACT_FILTER_ID);

    
    // instr.SetUpdate(filter_idx, 
    //                 Ite())



  }
}

void DefineConvWeightFetch(Ila& child) {
  
  auto state = child.state(ACCEL_CONV_CHILD_STATE);

  { 

  }

}



} // hlscnn
} // ilang