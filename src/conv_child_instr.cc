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

auto mul_in = SortRef::BV(WEIGHT_TOTAL_BITWIDTH);
auto mul_out = SortRef::BV(PSUM_TOTAL_BITWIDTH);
FuncRef ConvMacPsumMul("ConvMacPsumMul", mul_out, mul_in, mul_in);

auto psum_type = SortRef::BV(PSUM_TOTAL_BITWIDTH);
auto act_psum_type = SortRef::BV(ACT_TOTAL_BITWIDTH);
FuncRef Psum2Act("Psum2Act", act_psum_type, psum_type);

auto act_type = SortRef::BV(ACT_TOTAL_BITWIDTH);
FuncRef ActAdd("ActAdd", act_type, act_type, act_type);
FuncRef ActRelu("ActRelu", act_type, act_type);

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

  child.NewBvState(CONV_CHILD_WEIGHT_ADDR, CONV_CHILD_WEIGHT_ADDR_BITWIDTH);

  child.NewBvState(CONV_CHILD_ACTIVATION_PSUM, CONV_CHILD_ACTIVATION_PSUM_BITWIDTH);
  
  child.NewBvState(CONV_CHILD_ACT_REQ_LENGTH, CONV_CHILD_ACT_REQ_LENGTH_BITWIDTH);
  child.NewBvState(CONV_CHILD_ACT_FETCH_CNTR, CONV_CHILD_ACT_FETCH_CNTR_BITWIDTH);
  
  for (int i = 0; i < CONV_VECTOR_SIZE; i++) {
    // act array
    auto act_v_name = GetStateName(CONV_CHILD_ACT_ARRAY, i);
    child.NewBvState(act_v_name, CONV_CHILD_ACT_ARRAY_BITWIDTH);
    // weight array
    auto weight_v_name = GetStateName(CONV_CHILD_WEIGHT_ARRAY, i);
    child.NewBvState(weight_v_name, CONV_CHILD_WEIGHT_ARRAY_BITWIDTH);
    // oact array
    auto oact_v_name = GetStateName(CONV_CHILD_O_ACT_ARRAY, i);
    child.NewBvState(oact_v_name, CONV_CHILD_O_ACT_ARRAY_BITWIDTH);
    // out array
    auto out_v_name = GetStateName(CONV_CHILD_OUT_ARRAY, i);
    child.NewBvState(out_v_name, CONV_CHILD_OUT_ARRAY_BITWIDTH);
  }
  
  child.AddInit(state == CONV_CHILD_STATE_IDLE);

  // Declare child instructions, seperating activation fetching, weigth fetching 
  // and datapath
  DefineConvActFetch(child);
  DefineConvWeightFetch(child);
  DefineConvDatapath(child);  
}

void DefineConvActFetch(Ila& child) {
  
  auto state = child.state(ACCEL_CONV_CHILD_STATE);

  // child states
  auto filter_idx = child.state(CONV_CHILD_FILTER_ID);
  auto chan_block = child.state(CONV_CHILD_CHAN_BLOCK_ID);
  auto input_row = child.state(CONV_CHILD_INPUT_ROW_ID);
  auto input_col = child.state(CONV_CHILD_INPUT_COL_ID);

  { // instr ---- start Activation fetching
    // initilizting the loop parameter, setting them to zero, thus the next state should
    // jump to weight fetching
    auto instr = child.NewInstr("accel_conv_child_start");
    instr.SetDecode(state == CONV_CHILD_STATE_IDLE);

    instr.SetUpdate(filter_idx, BvConst(0, CONV_CHILD_FILTER_ID_BITWIDTH));
    instr.SetUpdate(chan_block, BvConst(0, CONV_CHILD_CHAN_BLOCK_ID_BITWIDTH));
    instr.SetUpdate(input_row, BvConst(0, CONV_CHILD_INPUT_ROW_ID_BITWIDTH));
    instr.SetUpdate(input_col, BvConst(0, CONV_CHILD_INPUT_COL_ID_BITWIDTH));

    //TODO: at the start, the next state should directly jump to the weight fetching!
    auto next_state = BvConst(CONV_CHILD_STATE_ACT_SET_REQ_LEN,
                              ACCEL_CONV_CHILD_STATE_BITWIDTH);
    // reset the out_array
    for (auto i = 0; i < CONV_VECTOR_SIZE; i++) {
      instr.SetUpdate(child.state(GetStateName(CONV_CHILD_OUT_ARRAY, i)), 
                      BvConst(0, CONV_CHILD_OUT_ARRAY_BITWIDTH));
    }
    
    instr.SetUpdate(state, next_state);
  }

  { // instr ---- conv done 
    auto instr = child.NewInstr("accel_conv_done");
    instr.SetDecode(state == CONV_CHILD_STATE_DONE);

    instr.SetUpdate(state, 
      BvConst(CONV_CHILD_STATE_IDLE, ACCEL_CONV_CHILD_STATE_BITWIDTH));
    instr.SetUpdate(child.state(ACCEL_CONV_CHILD_VALID_FLAG),
      BvConst(ACCEL_CONV_CHILD_INVALID, ACCEL_CONV_CHILD_VALID_FLAG_BITWIDTH));
  }

  { // instr ---- setting filter_idx
    // incrementing filter_idx, or conv done
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
    auto req_len = child.state(CONV_CHILD_ACT_REQ_LENGTH);

    auto next_input_col = Ite(input_col >= last_col_ext,
                              BvConst(0, input_col.bit_width()), input_col + req_len);
    
    auto next_state = 
      Ite(input_col >= last_col_ext,
          BvConst(CONV_CHILD_STATE_ACT_INPUT_ROW, ACCEL_CONV_CHILD_STATE_BITWIDTH),
          BvConst(CONV_CHILD_STATE_ACT_SET_REQ_LEN, ACCEL_CONV_CHILD_STATE_BITWIDTH));

    instr.SetUpdate(input_col, next_input_col);
    instr.SetUpdate(state, next_state);
  }

  { // instr ---- setting act request length
    auto instr = child.NewInstr("accel_conv_child_act_set_req_length");
    instr.SetDecode(state == CONV_CHILD_STATE_ACT_SET_REQ_LEN);

    auto last_col = child.state(CONV_INPUT_COL_NUM);
    auto last_col_ext = Concat(BvConst(0, 1), last_col);
    auto col_remain = last_col_ext - input_col;

    auto req_len = child.state(CONV_CHILD_ACT_REQ_LENGTH);
    auto act_fetch_cntr = child.state(CONV_CHILD_ACT_FETCH_CNTR);

    auto req_len_next = Ite(col_remain > CONV_BURST_LENGTH,
                            BvConst(CONV_BURST_LENGTH, CONV_CHILD_ACT_REQ_LENGTH_BITWIDTH),
                            col_remain);
    auto act_fetch_cntr_next = BvConst(0, CONV_CHILD_ACT_FETCH_CNTR_BITWIDTH);
    
    instr.SetUpdate(req_len, req_len_next);
    instr.SetUpdate(act_fetch_cntr, req_len_next);

    auto next_state = BvConst(CONV_CHILD_STATE_ACT_FETCH_ACT,
                              ACCEL_CONV_CHILD_STATE_BITWIDTH);
    
    instr.SetUpdate(state, next_state);
  }

  { // instr ---- fetching activations from external memory
    // Use an internal memory to simulate the external memory
    auto instr = child.NewInstr("accel_conv_child_act_fetch_activations");
    instr.SetDecode(state == CONV_CHILD_STATE_ACT_FETCH_ACT);

    // fetch the activation from internal memory
    //channel_block_address = base_addr + ((channel_block_idx*input_rows*input_cols*CHANNEL_BLOCK_SIZE) 
    // + in_row*(input_cols*CHANNEL_BLOCK_SIZE) + in_col*CHANNEL_BLOCK_SIZE)*(ACTIVATION_TOT_WIDTH/8);
    auto act_addr = act_gen_get_addr(child, input_row, input_col, chan_block);
    instr.SetUpdate(child.state(TOP_MASTER_RD_ADDR_OUT), act_addr);

    auto vir_mem = child.state(VIRTUAL_SOC_MEMORY);
    for (auto i = 0; i < CONV_VECTOR_SIZE; i++) {
      auto elem = child.state(GetStateName(CONV_CHILD_ACT_ARRAY, i));
      auto act_byte_0 = Load(vir_mem, act_addr + 2*i);
      auto act_byte_1 = Load(vir_mem, act_addr + 2*i + 1);
      instr.SetUpdate(elem, Concat(act_byte_1, act_byte_0));
    }

    auto cntr = child.state(CONV_CHILD_ACT_FETCH_CNTR);
    instr.SetUpdate(cntr, cntr+1);
    
    auto next_state = BvConst(CONV_CHILD_STATE_WEIGHT_INIT,
                              ACCEL_CONV_CHILD_STATE_BITWIDTH);

    instr.SetUpdate(state, next_state);    
  }

  // { // instr ---- sending activation fetching request
  //   // TODO: check correctness! The use of the virtual internal memory as external memory
  //   auto instr = child.NewInstr("accel_conv_child_act_send_rd_req");
  //   instr.SetDecode(state == CONV_CHILD_STATE_ACT_SEND_RD_REQ);
    
  //   //channel_block_address = base_addr + ((channel_block_idx*input_rows*input_cols*CHANNEL_BLOCK_SIZE) 
  //   // + in_row*(input_cols*CHANNEL_BLOCK_SIZE) + in_col*CHANNEL_BLOCK_SIZE)*(ACTIVATION_TOT_WIDTH/8);
  //   auto act_addr = act_gen_get_addr(child, input_row, input_col, chan_block);

  //   // push the act fetching request to the master AXI interface
  //   instr.SetUpdate(child.state(ACCEL_MASTER_AXI_CHILD_VALID_FLAG),
  //         BvConst(ACCEL_CONV_CHILD_VALID, ACCEL_MASTER_AXI_CHILD_VALID_FLAG_BITWIDTH));
  //   instr.SetUpdate(child.state(TOP_MASTER_RD_ADDR_OUT), act_addr);
  //   instr.SetUpdate(child.state(TOP_MASTER_IF_RD), 
  //                   BvConst(ACCEL_CONV_CHILD_VALID, TOP_MASTER_IF_RD_BITWIDTH));
  //   instr.SetUpdate(child.state(ACCEL_MASTER_AXI_CHILD_STATE),
  //           BvConst(MASTER_AXI_CHILD_STATE_ACT_RD, ACCEL_MASTER_AXI_CHILD_STATE_BITWIDTH));
    
  //   // setting the next state
  //   auto next_state = BvConst(CONV_CHILD_STATE_ACT_RECV_RD_RESP, ACCEL_CONV_CHILD_STATE_BITWIDTH);
  //   instr.SetUpdate(state, next_state);
  // }

  // { // instr ---- receiving activations
  //   // TODO: this instr is to be abstracted. we shouldn't model the low level AXI details
  //   auto instr = child.NewInstr("accel_conv_child_act_recv_rd_resp");
  //   // this instructions should wait for the resp of axi master rd
  //   auto decode_cond = ((state == CONV_CHILD_STATE_ACT_RECV_RD_RESP) &
  //         child.input(TOP_MASTER_RD_RESP_VALID_FLAG) == ACCEL_CONV_CHILD_VALID);

  //   instr.SetDecode(decode_cond);

  //   for (auto i = 0; i < CONV_VECTOR_SIZE; i++) {
  //     auto elem = child.state(GetStateName(CONV_CHILD_ACT_ARRAY, i));
  //     auto act_in_l = child.input(GetStateName(TOP_MASTER_DATA_IN, 2*i));
  //     auto act_in_h = child.input(GetStateName(TOP_MASTER_DATA_IN, 2*i+1));
      
  //     instr.SetUpdate(elem, Concat(act_in_h, act_in_l));
  //   }

  //   // set axi master rd command off
  //   instr.SetUpdate(child.state(ACCEL_MASTER_AXI_CHILD_VALID_FLAG),
  //         BvConst(ACCEL_CONV_CHILD_INVALID, ACCEL_MASTER_AXI_CHILD_VALID_FLAG_BITWIDTH));
  //   instr.SetUpdate(child.state(TOP_MASTER_IF_RD),
  //                   BvConst(ACCEL_CONV_CHILD_INVALID, TOP_MASTER_IF_RD_BITWIDTH));
  //   instr.SetUpdate(child.state(ACCEL_MASTER_AXI_CHILD_STATE),
  //                   BvConst(MASTER_AXI_CHILD_STATE_IDLE, ACCEL_MASTER_AXI_CHILD_STATE_BITWIDTH));

  //   auto next_state = BvConst(CONV_CHILD_STATE_WEIGHT_INIT,
  //                             ACCEL_CONV_CHILD_STATE_BITWIDTH);

  //   instr.SetUpdate(state, next_state);
  // }
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
    // this part setting the kern_row and kern_col to zero, thus the next state should jump to
    // check out-of-bound, no need to increment the loop param for this one
    auto instr = child.NewInstr("accel_conv_child_weight_init");
    instr.SetDecode(state == CONV_CHILD_STATE_WEIGHT_INIT);

    instr.SetUpdate(kern_row, kern_row_init);
    instr.SetUpdate(kern_col, kern_col_init);

    auto next_state = BvConst(CONV_CHILD_STATE_WEIGHT_CHECK_BOUND,
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

    instr.SetUpdate(kern_row, next_kern_row);

    // auto next_state = 
    //   Ite(kern_row >= last_kern_row_ext,
    //       BvConst(CONV_CHILD_STATE_ACT_INPUT_COL, ACCEL_CONV_CHILD_STATE_BITWIDTH),
    //       BvConst(CONV_CHILD_STATE_WEIGHT_COL_FETCH, ACCEL_CONV_CHILD_STATE_BITWIDTH));
    // update 08172020: next state update
    auto req_cntr = child.state(CONV_CHILD_ACT_FETCH_CNTR);
    auto req_len = Extract(child.state(CONV_CHILD_ACT_REQ_LENGTH), 
                            req_cntr.bit_width() - 1, 0);
    auto last_act_req = (req_cntr == req_len);

    auto next_state = 
      Ite(kern_row >= last_kern_row_ext,
        Ite(last_act_req,
            BvConst(CONV_CHILD_STATE_ACT_INPUT_COL, ACCEL_CONV_CHILD_STATE_BITWIDTH),
            BvConst(CONV_CHILD_STATE_ACT_FETCH_ACT, ACCEL_CONV_CHILD_STATE_BITWIDTH)),
        BvConst(CONV_CHILD_STATE_WEIGHT_COL_FETCH, ACCEL_CONV_CHILD_STATE_BITWIDTH));

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

    // TODO: this address should be vector level (128bit) address
    auto weight_req_addr = WtGetAddr(child, filter_idx, kern_row, kern_col, chan_block);
    auto spad_addr_base = weight_req_addr * NIC_MEM_ELEM_BYTEWIDTH;
    auto spad0 = child.state(SCRATCH_PAD_0);

    for (auto i = 0; i < CONV_VECTOR_SIZE; i++) {
      auto wt_array_element = child.state(GetStateName(CONV_CHILD_WEIGHT_ARRAY, i));
      auto wt_byte_0 = Load(spad0, spad_addr_base + i*2);
      auto wt_byte_1 = Load(spad0, spad_addr_base + i*2 + 1);
      instr.SetUpdate(wt_array_element, Concat(wt_byte_1, wt_byte_0));
    }

    auto next_state = BvConst(CONV_CHILD_STATE_DP_MAC_PSUM,
                              ACCEL_CONV_CHILD_STATE_BITWIDTH);

    instr.SetUpdate(state, next_state);
  }
}

void DefineConvDatapath(Ila& child) {
  auto state = child.state(ACCEL_CONV_CHILD_STATE);

  { // instr ---- calculate the mac_psum
    auto instr = child.NewInstr("conv_child_dp_mac_psum");
    instr.SetDecode(state == CONV_CHILD_STATE_DP_MAC_PSUM);

    auto mac_psum = BvConst(0, PSUM_TOTAL_BITWIDTH);

    for (auto i = 0; i < CONV_VECTOR_SIZE; i++) {
      auto weight = child.state(GetStateName(CONV_CHILD_WEIGHT_ARRAY, i));
      auto act = child.state(GetStateName(CONV_CHILD_ACT_ARRAY, i));
      mac_psum = mac_psum + ConvMacPsumMul(weight, act);
    }

    auto act_psum = Psum2Act(mac_psum);
    instr.SetUpdate(child.state(CONV_CHILD_ACTIVATION_PSUM), act_psum);  

    auto next_state = BvConst(CONV_CHILD_STATE_FETCH_OUT_ACT,
                              ACCEL_CONV_CHILD_STATE_BITWIDTH);
    instr.SetUpdate(state, next_state);
  }

  { // instr ---- fetching previous activations from spad1
    auto instr = child.NewInstr("conv_child_fetch_act_spad1");
    instr.SetDecode(state == CONV_CHILD_STATE_FETCH_OUT_ACT);

    auto act_row = child.state(CONV_CHILD_INPUT_ROW_ID);
    auto act_col = child.state(CONV_CHILD_INPUT_COL_ID);
    auto k_row = child.state(CONV_CHILD_KERNEL_ROW_ID);
    auto k_col = child.state(CONV_CHILD_KERNEL_COL_ID);
    auto act_filter_id = child.state(CONV_CHILD_FILTER_ID);
    
    //TODO: this address should be vector level address (128bit)
    auto oactfetch_addr = OutActGetAddr(child, act_row, act_col, k_row, k_col, act_filter_id);
    auto spad1_base_addr = oactfetch_addr * NIC_MEM_ELEM_BYTEWIDTH;
    auto spad1 = child.state(SCRATCH_PAD_1);

    for (auto i = 0; i < CONV_VECTOR_SIZE; i++) {
      auto oact_element = child.state(GetStateName(CONV_CHILD_O_ACT_ARRAY, i));
      auto oact_byte_0 = Load(spad1, spad1_base_addr + 2*i);
      auto oact_byte_1 = Load(spad1, spad1_base_addr + 2*i + 1);
      instr.SetUpdate(oact_element, Concat(oact_byte_1, oact_byte_0));
    }

    auto next_state = BvConst(CONV_CHILD_STATE_BIAS_RELU, ACCEL_CONV_CHILD_STATE_BITWIDTH);
    instr.SetUpdate(state, next_state);
  }

  { // instr ---- add bias and relu
    // TODO: if this instr execute too slow, can divided them into 3 instructions
    auto instr = child.NewInstr("conv_child_dp_bias_relu");
    instr.SetDecode(state == CONV_CHILD_STATE_BIAS_RELU);

    auto psum_val = child.state(CONV_CHILD_ACTIVATION_PSUM);

    // ------------------------------------------------------------------
    auto wbk_row = child.state(CONV_CHILD_KERNEL_ROW_ID);
    auto wbk_col = child.state(CONV_CHILD_KERNEL_COL_ID);
    auto wbact_chblk = child.state(CONV_CHILD_CHAN_BLOCK_ID);

    auto is_first_psum = (wbk_row==0) & (wbk_col==0) & (wbact_chblk==0);
    auto en_accum = child.state(CONV_ENABLE_ACCUM);

    auto ofilter_idx = child.state(CONV_OFILTER_IDX);
    auto wbact_idx = URem(ofilter_idx - 1, BvConst(CONV_VECTOR_SIZE, ofilter_idx.bit_width()));
    auto oact_element = GetActVectorState(child, CONV_CHILD_O_ACT_ARRAY, wbact_idx);

    auto oact_out = Ite(is_first_psum & (en_accum == 0),
                        psum_val, ActAdd(psum_val, oact_element));
    // ------------------------------------------------------------------
    auto wbact_row = child.state(CONV_CHILD_INPUT_ROW_ID);
    auto wbact_col = child.state(CONV_CHILD_INPUT_COL_ID);

    auto is_last_psum = WtIsLastPsum(child, wbact_row, wbact_col, wbk_row, wbk_col, wbact_chblk);
    auto en_bias = child.state(CONV_ENABLE_BIAS);
    auto chan_bias = child.state(CONV_CHAN_BIAS);

    oact_out = Ite(is_last_psum & (en_bias != 0), 
                   ActAdd(oact_out, chan_bias), oact_out);
    
    auto en_relu = child.state(CONV_ENABLE_RELU);

    oact_out = Ite(is_last_psum & (en_relu != 0),
                   ActRelu(oact_out), oact_out);
    // ------------------------------------------------------------------
    for (auto i = 0; i < CONV_VECTOR_SIZE; i++) {
      auto out_element = child.state(GetStateName(CONV_CHILD_OUT_ARRAY, i));
      auto out_element_next = Ite(wbact_idx == i, oact_out, out_element);
      instr.SetUpdate(out_element, out_element_next);
    }

    auto next_state = BvConst(CONV_CHILD_STATE_OUT, ACCEL_CONV_CHILD_STATE_BITWIDTH);
    instr.SetUpdate(state, next_state);
  }

  { // instr ---- write result into spad1
    auto instr = child.NewInstr("conv_child_output");
    instr.SetDecode(state == CONV_CHILD_STATE_OUT);

    auto act_row = child.state(CONV_CHILD_INPUT_ROW_ID);
    auto act_col = child.state(CONV_CHILD_INPUT_COL_ID);
    auto k_row = child.state(CONV_CHILD_KERNEL_ROW_ID);
    auto k_col = child.state(CONV_CHILD_KERNEL_COL_ID);
    auto act_filter_id = child.state(CONV_CHILD_FILTER_ID);
    
    //TODO: this address should be vector level address (128bit)
    auto out_addr = OutActGetAddr(child, act_row, act_col, k_row, k_col, act_filter_id);
    auto spad1_base_addr = out_addr * NIC_MEM_ELEM_BYTEWIDTH;
    auto spad1 = child.state(SCRATCH_PAD_1);
    auto spad1_next = spad1;

    for (auto i = 0; i < CONV_VECTOR_SIZE; i++) {
      auto out_element = child.state(GetStateName(CONV_CHILD_OUT_ARRAY, i));
      auto out_byte_0 = Extract(out_element, 7, 0);
      auto out_byte_1 = Extract(out_element, 15, 8);
      spad1_next = Store(spad1_next, spad1_base_addr + 2*i, out_byte_0);
      spad1_next = Store(spad1_next, spad1_base_addr + 2*i + 1, out_byte_1);
    }

    instr.SetUpdate(spad1, spad1_next);

    // next state should jump back to the innerest loop, which is incrementing kern_col
    auto next_state = 
      BvConst(CONV_CHILD_STATE_WEIGHT_COL_FETCH, ACCEL_CONV_CHILD_STATE_BITWIDTH);

    instr.SetUpdate(state, next_state);
  }

}


} // hlscnn
} // ilang