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

  child.NewBvState(CONV_CHILD_KERNEL_COL_ID, CONV_CHILD_KERNEL_COL_ID_BITWIDTH);
  child.NewBvState(CONV_CHILD_KERNEL_ROW_ID, CONV_CHILD_KERNEL_ROW_ID_BITWIDTH);
  
  for (int i = 0; i < CONV_VECTOR_SIZE; i++) {
    auto state_name = GetStateName(CONV_CHILD_ACT_ARRAY, i);
    child.NewBvState(state_name, CONV_CHILD_ACT_ARRAY_BITWIDTH);
  }
  
  child.AddInit(state == CONV_CHILD_STATE_IDLE);

  // ILA_INFO << "child_state number: " << child.state_num();
  // ILA_INFO << child.state(CONV_OFILTER_IDX);
  // ILA_INFO << "child_input count: " << child.input_num();
  // for (auto i = 0; i < child.input_num(); i++) {
  //   ILA_INFO << "child input: " << child.input(i);
  // }
  ILA_INFO << "conv_row_size: " << CONV_ROW_SIZE_T;
  ILA_INFO << "conv_channel_size: " << CONV_CHANNEL_SIZE_T;
  ILA_INFO << "conv_kernel_size: " << CONV_KERNEL_SIZE_T;
  ILA_INFO << "conv_filter_size: " << CONV_FILTER_SIZE_T;

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

    //TODO: at the start, the next state should directly jump to the weight fetching!
    auto next_state = BvConst(CONV_CHILD_STATE_ACT_FILTER_ID,
                              ACCEL_CONV_CHILD_STATE_BITWIDTH);
    
    instr.SetUpdate(state, next_state);
  }

  { // instr ---- setting filter_idx
    auto instr = child.NewInstr("accel_conv_child_act_filter_idx");
    instr.SetDecode(state == CONV_CHILD_STATE_ACT_FILTER_ID);

    auto num_filters = child.state(CONV_OFILTER_IDX);
    auto num_filters_ext = Concat(BvConst(0, filter_idx.bit_width() - num_filters.bit_width()),
                                  num_filters);

    auto next_filter_id = Ite(filter_idx >= num_filters_ext,
                              BvConst(0, filter_idx.bit_width()), filter_idx + 1);
    auto next_state = 
      Ite(filter_idx >= num_filters_ext,
            BvConst(CONV_CHILD_STATE_DONE, ACCEL_CONV_CHILD_STATE_BITWIDTH),
            BvConst(CONV_CHILD_STATE_ACT_INPUT_CHANNEL_BLOCK, ACCEL_CONV_CHILD_STATE_BITWIDTH));

    instr.SetUpdate(filter_idx, next_filter_id);
    instr.SetUpdate(state, next_state);
  }

  { // instr ---- incrementing input channel block id
    auto instr = child.NewInstr("accel_conv_child_act_input_chan_block_id");
    instr.SetDecode(state == CONV_CHILD_STATE_ACT_INPUT_CHANNEL_BLOCK);

    // last_channel_block = frac_ceil(input_channels, channel_block_size);
    auto input_channels = child.state(CONV_INPUT_CHAN_NUM);
    auto chan_block_size = BvConst(CHANNEL_BLOCK_SIZE, input_channels.bit_width());
    auto last_chan_block = 
      Ite(URem(input_channels, chan_block_size) == 0,
          input_channels / chan_block_size, input_channels / chan_block_size + 1);
    
    auto last_chan_blk_ext = Concat(BvConst(0, chan_block.bit_width()-last_chan_block.bit_width()),
                                    last_chan_block);
    
    auto next_chan_block = Ite(chan_block >= last_chan_blk_ext,
                               BvConst(0, chan_block.bit_width()), chan_block + 1);
    auto next_state = 
      Ite(chan_block >= last_chan_blk_ext,
          BvConst(CONV_CHILD_STATE_ACT_FILTER_ID, ACCEL_CONV_CHILD_STATE_BITWIDTH),
          BvConst(CONV_CHILD_STATE_ACT_INPUT_ROW, ACCEL_CONV_CHILD_STATE_BITWIDTH));
  
    instr.SetUpdate(chan_block, next_chan_block);
    instr.SetUpdate(state, next_state);
  }

  { // instr ---- incrementing input row id
    auto instr = child.NewInstr("accel_conv_child_act_input_row_id");
    instr.SetDecode(state == CONV_CHILD_STATE_ACT_INPUT_ROW);

    auto last_row = child.state(CONV_INPUT_ROW_NUM);
    auto row_stride = 1;

    auto last_row_ext = Concat(BvConst(0, input_row.bit_width() - last_row.bit_width()),
                                last_row);
    
    auto next_input_row = Ite(input_row >= last_row_ext,
                              BvConst(0, input_row.bit_width()), input_row + row_stride);
    auto next_state = 
      Ite(input_row >= last_row_ext,
          BvConst(CONV_CHILD_STATE_ACT_INPUT_CHANNEL_BLOCK, ACCEL_CONV_CHILD_STATE_BITWIDTH),
          BvConst(CONV_CHILD_STATE_ACT_INPUT_COL, ACCEL_CONV_CHILD_STATE_BITWIDTH));
    
    instr.SetUpdate(input_row, next_input_row);
    instr.SetUpdate(state, next_state);
  }

  { // instr ---- incrementing input col
    auto instr = child.NewInstr("accel_conv_child_act_input_col_id");
    instr.SetDecode(state == CONV_CHILD_STATE_ACT_INPUT_COL);

    auto last_col = child.state(CONV_INPUT_COL_NUM);
    auto last_col_ext = Concat(BvConst(0, input_col.bit_width() - last_col.bit_width()),
                                last_col);

    auto next_input_col = Ite(input_col >= last_col_ext,
                              BvConst(0, input_col.bit_width()), input_col + 1);
    
    auto next_state = 
      Ite(input_col >= last_col_ext,
          BvConst(CONV_CHILD_STATE_ACT_INPUT_ROW, ACCEL_CONV_CHILD_STATE_BITWIDTH),
          BvConst(CONV_CHILD_STATE_ACT_SEND_RD_REQ, ACCEL_CONV_CHILD_STATE_BITWIDTH));

    instr.SetUpdate(input_col, next_input_col);
    instr.SetUpdate(state, next_state);
  }

  { // instr ---- sending activation fetching request
    auto instr = child.NewInstr("accel_conv_child_act_send_rd_req");
    instr.SetDecode(state == CONV_CHILD_STATE_ACT_SEND_RD_REQ);
    
    //channel_block_address = base_addr + ((channel_block_idx*input_rows*input_cols*CHANNEL_BLOCK_SIZE) 
    // + in_row*(input_cols*CHANNEL_BLOCK_SIZE) + in_col*CHANNEL_BLOCK_SIZE)*(ACTIVATION_TOT_WIDTH/8);
    auto act_addr = act_gen_get_addr(child, input_row, input_col, chan_block);

    // push the act fetching request to the master AXI interface
    instr.SetUpdate(child.state(TOP_MASTER_RD_ADDR_OUT), act_addr);
    instr.SetUpdate(child.state(TOP_MASTER_IF_RD), 
                    BvConst(ACCEL_CONV_CHILD_VALID, TOP_MASTER_IF_RD_BITWIDTH));
    instr.SetUpdate(child.state(ACCEL_MASTER_AXI_CHILD_STATE),
            BvConst(MASTER_AXI_CHILD_STATE_ACT_RD, ACCEL_MASTER_AXI_CHILD_STATE_BITWIDTH));
    
    // setting the next state
    auto next_state = BvConst(CONV_CHILD_STATE_ACT_RECV_RD_RESP, ACCEL_CONV_CHILD_STATE_BITWIDTH);
    instr.SetUpdate(state, next_state);
  }

  { // instr ---- receiving activations
    auto instr = child.NewInstr("accel_conv_child_act_recv_rd_resp");
    // this instructions should wait for the resp of axi master rd
    auto decode_cond = ((state == CONV_CHILD_STATE_ACT_RECV_RD_RESP) &
          child.input(TOP_MASTER_RD_RESP_VALID_FLAG) == ACCEL_CONV_CHILD_VALID);

    instr.SetDecode(decode_cond);

    for (auto i = 0; i < CONV_VECTOR_SIZE; i++) {
      auto elem = child.state(GetStateName(CONV_CHILD_ACT_ARRAY, i));
      auto act_in_l = child.input(GetStateName(TOP_MASTER_DATA_IN, 2*i));
      auto act_in_h = child.input(GetStateName(TOP_MASTER_DATA_IN, 2*i+1));
      
      instr.SetUpdate(elem, Concat(act_in_h, act_in_l));
    }

    // set axi master rd command off
    instr.SetUpdate(child.state(TOP_MASTER_IF_RD),
                    BvConst(ACCEL_CONV_CHILD_INVALID, TOP_MASTER_IF_RD_BITWIDTH));
    instr.SetUpdate(child.state(ACCEL_MASTER_AXI_CHILD_STATE),
                    BvConst(MASTER_AXI_CHILD_STATE_IDLE, ACCEL_MASTER_AXI_CHILD_STATE_BITWIDTH));

    auto next_state = BvConst(CONV_CHILD_STATE_WEIGHT_INIT,
                              ACCEL_CONV_CHILD_STATE_BITWIDTH);

    instr.SetUpdate(state, next_state);
  }
}

void DefineConvWeightFetch(Ila& child) {
  
  auto state = child.state(ACCEL_CONV_CHILD_STATE);

  auto kern_row = child.state(CONV_CHILD_KERNEL_ROW_ID);
  auto kern_col = child.state(CONV_CHILD_KERNEL_COL_ID);
  auto row_stride = child.state(CONV_KERNEL_R_STRIDE);
  auto col_stride = child.state(CONV_KERNEL_C_STRIDE);

  auto filter_idx = child.state(CONV_CHILD_FILTER_ID);
  auto chan_block = child.state(CONV_CHILD_CHAN_BLOCK_ID);
  auto act_row = child.state(CONV_CHILD_INPUT_ROW_ID);
  auto act_col = child.state(CONV_CHILD_INPUT_COL_ID);

  auto row_stride_ext = Concat(BvConst(0, act_row.bit_width()-row_stride.bit_width()), row_stride);
  auto col_stride_ext = Concat(BvConst(0, act_col.bit_width()-col_stride.bit_width()), col_stride);

  auto kern_row_init = Extract(URem(act_row, row_stride_ext), kern_row.bit_width()-1, 0);
  auto kern_col_init = Extract(URem(act_col, col_stride_ext), kern_col.bit_width()-1, 0);

  { // instr ---- initializing the weight fetching parameters
    auto instr = child.NewInstr("accel_conv_child_weight_init");
    instr.SetDecode(state == CONV_CHILD_STATE_WEIGHT_INIT);

    instr.SetUpdate(kern_row, kern_row_init);
    instr.SetUpdate(kern_col, kern_col_init);

    auto next_state = BvConst(CONV_CHILD_STATE_WEIGHT_ROW_FETCH,
                              ACCEL_CONV_CHILD_STATE_BITWIDTH);
    
    instr.SetUpdate(state, next_state);
  }

  { // instr ---- incrementing kern_row
    auto instr = child.NewInstr("accel_conv_child_weight_row_id");
    instr.SetDecode(state == CONV_CHILD_STATE_WEIGHT_ROW_FETCH);

    auto last_kern_row = child.state(CONV_KERNEL_ROW_NUM);
    auto last_kern_row_ext = Concat(BvConst(0, kern_row.bit_width()-last_kern_row.bit_width()),
                                    last_kern_row);
    
    auto next_kern_row = Ite(kern_row >= last_kern_row_ext, 
                             kern_row_init, kern_row + Concat(BvConst(0,1), row_stride));

    auto next_state = 
      Ite(kern_row >= last_kern_row_ext,
          BvConst(CONV_CHILD_STATE_ACT_INPUT_COL, ACCEL_CONV_CHILD_STATE_BITWIDTH),
          BvConst(CONV_CHILD_STATE_WEIGHT_COL_FETCH, ACCEL_CONV_CHILD_STATE_BITWIDTH));

    instr.SetUpdate(kern_row, next_kern_row);
    instr.SetUpdate(state, next_state);
  }

  { // instr ---- incrementing kern_col
    auto instr = child.NewInstr("accel_conv_child_weight_col_id");
    instr.SetDecode(state == CONV_CHILD_STATE_WEIGHT_COL_FETCH);

    auto last_kern_col = child.state(CONV_KERNEL_COL_NUM);
    auto last_kern_col_ext = Concat(BvConst(0, 1), last_kern_col);

    auto next_kern_col = Ite(kern_col >= last_kern_col_ext,
                             kern_col_init, kern_col + Concat(BvConst(0,1), col_stride));
    auto next_state =
      Ite(kern_col >= last_kern_col_ext,
          BvConst(CONV_CHILD_STATE_WEIGHT_ROW_FETCH, ACCEL_CONV_CHILD_STATE_BITWIDTH),
          BvConst(CONV_CHILD_STATE_WEIGHT_CHECK_BOUND, ACCEL_CONV_CHILD_STATE_BITWIDTH));

    instr.SetUpdate(kern_col, next_kern_col);
    instr.SetUpdate(state, next_state);
  }

  { // instr ---- check out-of-bound condition
    auto instr = child.NewInstr("accel_conv_check_out_of_bound");
    instr.SetDecode(state == CONV_CHILD_STATE_WEIGHT_CHECK_BOUND);

    auto is_out_of_bound = conv_out_of_bound(child, act_row, act_col, kern_row, kern_col);

    auto next_state = 
      Ite(is_out_of_bound, 
        BvConst(CONV_CHILD_STATE_WEIGHT_COL_FETCH, ACCEL_CONV_CHILD_STATE_BITWIDTH),
        BvConst(CONV_CHILD_STATE_WEIGHT_SEND_DP, ACCEL_CONV_CHILD_STATE_BITWIDTH));

    instr.SetUpdate(state, next_state);
  }

  { // instr ---- send weights and act to datapath
    auto instr = child.NewInstr("accel_conv_send_dp");
    instr.SetDecode(state == CONV_CHILD_STATE_WEIGHT_SEND_DP);
    

  }

}



} // hlscnn
} // ilang