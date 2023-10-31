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
#include <vector>

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
  child.NewBvState(CONV_CHILD_FILTER_ID, CONV_CHILD_FILTER_ID_BITWIDTH);
  child.NewBvState(CONV_CHILD_CHAN_BLOCK_ID, CONV_CHILD_CHAN_BLOCK_ID_BITWIDTH);
  child.NewBvState(CONV_CHILD_INPUT_ROW_ID, CONV_CHILD_INPUT_ROW_ID_BITWIDTH);
  child.NewBvState(CONV_CHILD_INPUT_COL_ID, CONV_CHILD_INPUT_COL_ID_BITWIDTH);
  // declare a new state specifically for holding input col index for act fetching
  child.NewBvState(CONV_CHILD_INPUT_COL_ID_LOOP, CONV_CHILD_INPUT_COL_ID_LOOP_BITWIDTH);

  child.NewBvState(CONV_CHILD_KERNEL_COL_ID, CONV_CHILD_KERNEL_COL_ID_BITWIDTH);
  child.NewBvState(CONV_CHILD_KERNEL_ROW_ID, CONV_CHILD_KERNEL_ROW_ID_BITWIDTH);

  child.NewBvState(CONV_CHILD_WEIGHT_ADDR, CONV_CHILD_WEIGHT_ADDR_BITWIDTH);

  child.NewBvState(CONV_CHILD_ACTIVATION_PSUM, CONV_CHILD_ACTIVATION_PSUM_BITWIDTH);
  
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
  }
  
  // Declare child instructions, seperating activation fetching, weigth fetching 
  // and datapath
  DefineConvActFetch(child);
  DefineConvWeightFetch(child);
  DefineConvDatapath(child);  
}

void DefineConvActFetch(Ila& child) {
  
  auto state = child.state(ACCEL_CONV_CHILD_STATE);
  auto is_child_valid = 
    (child.state(ACCEL_CONV_CHILD_VALID_FLAG) == ACCEL_CONV_CHILD_VALID);

  // child states
  auto filter_idx = child.state(CONV_CHILD_FILTER_ID);
  auto chan_block = child.state(CONV_CHILD_CHAN_BLOCK_ID);
  auto input_row = child.state(CONV_CHILD_INPUT_ROW_ID);
  auto input_col = child.state(CONV_CHILD_INPUT_COL_ID);

  { // instr ---- start Activation fetching
    // initilizting the loop parameter, setting them to zero, thus the next state should
    // jump to weight fetching
    auto instr = child.NewInstr("accel_conv_child_start");
    instr.SetDecode(is_child_valid & (state == CONV_CHILD_STATE_IDLE));

    instr.SetUpdate(filter_idx, BvConst(0, CONV_CHILD_FILTER_ID_BITWIDTH));
    instr.SetUpdate(chan_block, BvConst(0, CONV_CHILD_CHAN_BLOCK_ID_BITWIDTH));
    instr.SetUpdate(input_row, BvConst(0, CONV_CHILD_INPUT_ROW_ID_BITWIDTH));
    instr.SetUpdate(input_col, BvConst(0, CONV_CHILD_INPUT_COL_ID_BITWIDTH));
    // skip the increment of iterator, and jump to the activation fetching
    auto next_state = BvConst(CONV_CHILD_STATE_ACT_FETCH_ACT,
                              ACCEL_CONV_CHILD_STATE_BITWIDTH);
    
    instr.SetUpdate(state, next_state);
  }

  { // instr ---- conv done 
    auto instr = child.NewInstr("accel_conv_done");
    instr.SetDecode(is_child_valid & (state == CONV_CHILD_STATE_DONE));

    instr.SetUpdate(state, 
      BvConst(CONV_CHILD_STATE_IDLE, ACCEL_CONV_CHILD_STATE_BITWIDTH));
    instr.SetUpdate(child.state(ACCEL_CONV_CHILD_VALID_FLAG),
      BvConst(ACCEL_CONV_CHILD_INVALID, ACCEL_CONV_CHILD_VALID_FLAG_BITWIDTH));
  }

  { // instr ---- setting filter_idx
    // incrementing filter_idx, or conv done
    auto instr = child.NewInstr("accel_conv_child_act_filter_idx");
    instr.SetDecode(is_child_valid & (state == CONV_CHILD_STATE_ACT_FILTER_ID));

    auto num_filters = child.state(CONV_OFILTER_IDX);
    auto num_filters_ext = Concat(BvConst(0, filter_idx.bit_width() - num_filters.bit_width()),
                                  num_filters);

    auto next_filter_id = Ite(filter_idx >= num_filters_ext - 1,
                              BvConst(0, filter_idx.bit_width()), filter_idx + 1);
    // udpate 08232020: FSM next state fixed (chan_block --> send req)
    auto next_state = 
      Ite(filter_idx >= num_filters_ext - 1,
            BvConst(CONV_CHILD_STATE_DONE, ACCEL_CONV_CHILD_STATE_BITWIDTH),
            BvConst(CONV_CHILD_STATE_ACT_FETCH_ACT, ACCEL_CONV_CHILD_STATE_BITWIDTH));

    instr.SetUpdate(filter_idx, next_filter_id);
    instr.SetUpdate(state, next_state);
  }

  { // instr ---- incrementing input channel block id
    auto instr = child.NewInstr("accel_conv_child_act_input_chan_block_id");
    instr.SetDecode(is_child_valid &
                    (state == CONV_CHILD_STATE_ACT_INPUT_CHANNEL_BLOCK));

    // last_channel_block = frac_ceil(input_channels, channel_block_size);
    auto input_channels = child.state(CONV_INPUT_CHAN_NUM);
    auto chan_block_size = BvConst(CHANNEL_BLOCK_SIZE, input_channels.bit_width());
    auto last_chan_block = 
      Ite(URem(input_channels, chan_block_size) == 0,
          input_channels / chan_block_size, input_channels / chan_block_size + 1);
    
    auto last_chan_blk_ext = Concat(BvConst(0, chan_block.bit_width()-last_chan_block.bit_width()),
                                    last_chan_block);
    
    auto next_chan_block = Ite(chan_block >= last_chan_blk_ext - 1,
                               BvConst(0, chan_block.bit_width()), chan_block + 1);
    // update 08232020: FSM next state fixed
    auto next_state = 
      Ite(chan_block >= last_chan_blk_ext - 1,
          BvConst(CONV_CHILD_STATE_ACT_FILTER_ID, ACCEL_CONV_CHILD_STATE_BITWIDTH),
          BvConst(CONV_CHILD_STATE_ACT_FETCH_ACT, ACCEL_CONV_CHILD_STATE_BITWIDTH));
  
    instr.SetUpdate(chan_block, next_chan_block);
    instr.SetUpdate(state, next_state);
  }

  { // instr ---- incrementing input row id
    auto instr = child.NewInstr("accel_conv_child_act_input_row_id");
    instr.SetDecode(is_child_valid & (state == CONV_CHILD_STATE_ACT_INPUT_ROW));

    auto last_row = child.state(CONV_INPUT_ROW_NUM);
    auto row_stride = 1;

    auto last_row_ext = Concat(BvConst(0, input_row.bit_width() - last_row.bit_width()),
                                last_row);
    
    auto next_input_row = Ite(input_row >= last_row_ext - 1,
                              BvConst(0, input_row.bit_width()), input_row + row_stride);
    // update 08232020: FSM next state fixed
    auto next_state = 
      Ite(input_row >= last_row_ext - 1,
          BvConst(CONV_CHILD_STATE_ACT_INPUT_CHANNEL_BLOCK, ACCEL_CONV_CHILD_STATE_BITWIDTH),
          BvConst(CONV_CHILD_STATE_ACT_FETCH_ACT, ACCEL_CONV_CHILD_STATE_BITWIDTH));
    
    instr.SetUpdate(input_row, next_input_row);
    instr.SetUpdate(state, next_state);
  }

  { // instr ---- incrementing input col
    auto instr = child.NewInstr("accel_conv_child_act_input_col_id");
    instr.SetDecode(is_child_valid & (state == CONV_CHILD_STATE_ACT_INPUT_COL));

    auto last_col = child.state(CONV_INPUT_COL_NUM);
    auto last_col_ext = Concat(BvConst(0, input_col.bit_width() - last_col.bit_width()),
                                last_col);
    auto col_stride = 1;
    auto next_input_col = Ite(input_col >= last_col_ext - 1,
                              BvConst(0, input_col.bit_width()), input_col + col_stride);
    auto next_state = 
      Ite(input_col >= last_col_ext - 1,
          BvConst(CONV_CHILD_STATE_ACT_INPUT_ROW, ACCEL_CONV_CHILD_STATE_BITWIDTH),
          BvConst(CONV_CHILD_STATE_ACT_FETCH_ACT, ACCEL_CONV_CHILD_STATE_BITWIDTH));

    instr.SetUpdate(input_col, next_input_col);
    instr.SetUpdate(state, next_state);
  }

  { // instr ---- fetching activations from spad1
    auto instr = child.NewInstr("accel_conv_child_act_fetch_activations");
    instr.SetDecode(is_child_valid & (state == CONV_CHILD_STATE_ACT_FETCH_ACT));

    /* Modification for a unifed spad for both input and output activations:
       remove the HLSCNN master axi interface for fetching data from external memory
       required input data should already be in spad1 for computation
    */
    // this is a byte level address
    auto act_addr = act_gen_get_addr(child, input_row, input_col, chan_block);
    auto spad_addr = act_addr - SPAD1_BASE_ADDR;
    auto spad1 = child.state(SCRATCH_PAD_1);

    for (auto i = 0; i < CONV_VECTOR_SIZE; i++) {
      auto elem = child.state(GetStateName(CONV_CHILD_ACT_ARRAY, i));
      auto act_byte_0 = Load(spad1, spad_addr + 2*i);
      auto act_byte_1 = Load(spad1, spad_addr + 2*i + 1);
      instr.SetUpdate(elem, Concat(act_byte_1, act_byte_0));
    }
    
    auto next_state = BvConst(CONV_CHILD_STATE_WEIGHT_INIT,
                              ACCEL_CONV_CHILD_STATE_BITWIDTH);

    instr.SetUpdate(state, next_state);    
  }
}

void DefineConvWeightFetch(Ila& child) {
  
  auto state = child.state(ACCEL_CONV_CHILD_STATE);
  auto is_child_valid = 
        (child.state(ACCEL_CONV_CHILD_VALID_FLAG) == ACCEL_CONV_CHILD_VALID);

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
    instr.SetDecode(is_child_valid & (state == CONV_CHILD_STATE_WEIGHT_INIT));

    instr.SetUpdate(kern_row, kern_row_init);
    instr.SetUpdate(kern_col, kern_col_init);

    auto next_state = BvConst(CONV_CHILD_STATE_WEIGHT_CHECK_BOUND,
                              ACCEL_CONV_CHILD_STATE_BITWIDTH);
    
    instr.SetUpdate(state, next_state);
  }

  { // instr ---- incrementing kern_row
    auto instr = child.NewInstr("accel_conv_child_weight_row_id");
    instr.SetDecode(is_child_valid & (state == CONV_CHILD_STATE_WEIGHT_ROW_FETCH));

    auto last_kern_row = child.state(CONV_KERNEL_ROW_NUM);
    auto last_kern_row_ext = Concat(BvConst(0, kern_row.bit_width()-last_kern_row.bit_width()),
                                    last_kern_row);
    
    // update 10282021: fixed the bound condition, should be last_kern_row - row_stride instead of last_kern_row - 1
    auto row_stride_ext = Concat(BvConst(0, 1), row_stride);
    auto next_kern_row = Ite(kern_row + row_stride_ext >= last_kern_row_ext, 
                             kern_row_init, kern_row + row_stride_ext);

    instr.SetUpdate(kern_row, next_kern_row);

    // update 08232020: after incrementing row id, the next instr should be check_out_of_bound
    // instead of incrementing col num, which has been down in the previous instruction.
    auto next_state = 
      Ite(kern_row + row_stride_ext >= last_kern_row_ext,
          BvConst(CONV_CHILD_STATE_ACT_INPUT_COL, ACCEL_CONV_CHILD_STATE_BITWIDTH),
          BvConst(CONV_CHILD_STATE_WEIGHT_CHECK_BOUND, ACCEL_CONV_CHILD_STATE_BITWIDTH));

    instr.SetUpdate(state, next_state);
  }

  { // instr ---- incrementing kern_col
    auto instr = child.NewInstr("accel_conv_child_weight_col_id");
    instr.SetDecode(is_child_valid & (state == CONV_CHILD_STATE_WEIGHT_COL_FETCH));

    auto last_kern_col = child.state(CONV_KERNEL_COL_NUM);
    auto last_kern_col_ext = Concat(BvConst(0, 1), last_kern_col);

    // 10282021: Same fix on bound condition as above
    auto col_stride_ext = Concat(BvConst(0,1), col_stride);
    auto next_kern_col = Ite(kern_col + col_stride_ext >= last_kern_col_ext,
                             kern_col_init, kern_col + col_stride_ext);
    auto next_state =
      Ite(kern_col + col_stride_ext >= last_kern_col_ext,
          BvConst(CONV_CHILD_STATE_WEIGHT_ROW_FETCH, ACCEL_CONV_CHILD_STATE_BITWIDTH),
          BvConst(CONV_CHILD_STATE_WEIGHT_CHECK_BOUND, ACCEL_CONV_CHILD_STATE_BITWIDTH));

    instr.SetUpdate(kern_col, next_kern_col);
    instr.SetUpdate(state, next_state);
  }

  { // instr ---- check out-of-bound condition
    auto instr = child.NewInstr("accel_conv_check_out_of_bound");
    instr.SetDecode(is_child_valid & (state == CONV_CHILD_STATE_WEIGHT_CHECK_BOUND));

    auto is_out_of_bound = conv_out_of_bound(child, act_row, act_col, kern_row, kern_col);

    // 11012021: Check whether the intial kernel row/col id is larger than limit
    auto last_kern_col = Concat(BvConst(0, 1), child.state(CONV_KERNEL_COL_NUM));
    auto last_kern_row = Concat(BvConst(0, 1), child.state(CONV_KERNEL_ROW_NUM));
    // flag is true when the inital kernel idx is larger than the kernel size
    auto check_kernel_init_value = (kern_row >= last_kern_row) | (kern_col >= last_kern_col);

    auto next_state = 
      Ite(is_out_of_bound | check_kernel_init_value, 
        BvConst(CONV_CHILD_STATE_WEIGHT_COL_FETCH, ACCEL_CONV_CHILD_STATE_BITWIDTH),
        BvConst(CONV_CHILD_STATE_WEIGHT_SEND_DP, ACCEL_CONV_CHILD_STATE_BITWIDTH));

    instr.SetUpdate(state, next_state);
  }

  { // instr ---- send weights and act to datapath
    auto instr = child.NewInstr("accel_conv_send_dp");
    instr.SetDecode(is_child_valid & (state == CONV_CHILD_STATE_WEIGHT_SEND_DP));

    // this address should be vector level (128bit) address
    // unlike get addr for activation, weight address still remain vector level to support
    // 8-bit datatype
    auto base_addr = child.state(CONV_WEIGHT_BASE);
    auto weight_req_addr = WtGetAddr(child, filter_idx, kern_row, kern_col, chan_block);
    // Try changing the weight's bitwidth from 8bits to 16bits
    // update 08252020: The weight data is expanded, the address should cut in half;
    // auto spad_addr_base = weight_req_addr * (NIC_MEM_ELEM_BYTEWIDTH/2);
    /* Move the base addr addition here to support different bitwidth of weight data */
    auto spad_addr_base = base_addr + weight_req_addr * NIC_MEM_ELEM_BYTEWIDTH - SPAD0_BASE_ADDR;
    auto spad0 = child.state(SCRATCH_PAD_0);

    // update 08252020: The weight data should be expand from 8bit to 16bit when reading
    for (auto i = 0; i < CONV_VECTOR_SIZE; i++) {
      auto wt_array_element = child.state(GetStateName(CONV_CHILD_WEIGHT_ARRAY, i));
      // auto wt_byte_0 = BvConst(0, SCRATCH_PAD_DATA_BITWIDTH);
      // auto wt_byte_1 = Load(spad0, spad_addr_base + i);
      auto wt_byte_0 = Load(spad0, spad_addr_base + 2*i);
      auto wt_byte_1 = Load(spad0, spad_addr_base + 2*i + 1);

      instr.SetUpdate(wt_array_element, Concat(wt_byte_1, wt_byte_0));
    }

    auto next_state = BvConst(CONV_CHILD_STATE_DP_MAC_PSUM,
                              ACCEL_CONV_CHILD_STATE_BITWIDTH);

    instr.SetUpdate(state, next_state);
  }
}

void DefineConvDatapath(Ila& child) {
  auto state = child.state(ACCEL_CONV_CHILD_STATE);
  auto is_child_valid = 
        (child.state(ACCEL_CONV_CHILD_VALID_FLAG) == ACCEL_CONV_CHILD_VALID);

  { // instr ---- calculate the mac_psum
    auto instr = child.NewInstr("conv_child_dp_mac_psum");
    instr.SetDecode(is_child_valid & (state == CONV_CHILD_STATE_DP_MAC_PSUM));

    auto mac_psum = BvConst(0, PSUM_TOTAL_BITWIDTH);

    for (auto i = 0; i < CONV_VECTOR_SIZE; i++) {
      auto weight = child.state(GetStateName(CONV_CHILD_WEIGHT_ARRAY, i));
      auto act = child.state(GetStateName(CONV_CHILD_ACT_ARRAY, i));
      std::vector<ExprRef> conv_mac_in = {mac_psum, weight, act};
      mac_psum = ConvMac(conv_mac_in);
    }

    auto act_psum = ConvMacPsum2Act(mac_psum);
    instr.SetUpdate(child.state(CONV_CHILD_ACTIVATION_PSUM), act_psum);  

    auto next_state = BvConst(CONV_CHILD_STATE_FETCH_OUT_ACT,
                              ACCEL_CONV_CHILD_STATE_BITWIDTH);
    instr.SetUpdate(state, next_state);
  }

  { // instr ---- fetching previous activations from spad1
    auto instr = child.NewInstr("conv_child_fetch_act_spad1");
    instr.SetDecode(is_child_valid & (state == CONV_CHILD_STATE_FETCH_OUT_ACT));

    auto act_row = child.state(CONV_CHILD_INPUT_ROW_ID);
    auto act_col = child.state(CONV_CHILD_INPUT_COL_ID);
    auto k_row = child.state(CONV_CHILD_KERNEL_ROW_ID);
    auto k_col = child.state(CONV_CHILD_KERNEL_COL_ID);
    auto act_filter_id = child.state(CONV_CHILD_FILTER_ID);
    
    auto oactfetch_addr = OutActGetAddr(child, act_row, act_col, k_row, k_col, act_filter_id);
    // the oactfetch_addr should already be a byte level address
    auto spad1_base_addr = oactfetch_addr - SPAD1_BASE_ADDR;
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
    instr.SetDecode(is_child_valid & (state == CONV_CHILD_STATE_BIAS_RELU));

    auto psum_val = child.state(CONV_CHILD_ACTIVATION_PSUM); //16

    // ------------------------------------------------------------------

    auto wbk_row = child.state(CONV_CHILD_KERNEL_ROW_ID);
    auto wbk_col = child.state(CONV_CHILD_KERNEL_COL_ID);
    auto wbact_chblk = child.state(CONV_CHILD_CHAN_BLOCK_ID);

    auto is_first_psum = (wbk_row==0) & (wbk_col==0) & (wbact_chblk==0);
    auto en_accum = child.state(CONV_ENABLE_ACCUM);

    // auto ofilter_idx = child.state(CONV_OFILTER_IDX);
    auto ofilter_idx = child.state(CONV_CHILD_FILTER_ID);
    auto wbact_idx = URem(ofilter_idx, BvConst(CONV_VECTOR_SIZE, ofilter_idx.bit_width()));
    auto oact_element = GetActVectorState(child, CONV_CHILD_O_ACT_ARRAY, wbact_idx);

    // oact_out is 32 bit
    auto oact_out = Ite(is_first_psum & (en_accum == 0),
                        ActAdd2Psum(psum_val, BvConst(0, ACT_TOTAL_BITWIDTH)), 
                        ActAdd2Psum(psum_val, oact_element));
    // ------------------------------------------------------------------

    auto wbact_row = child.state(CONV_CHILD_INPUT_ROW_ID);
    auto wbact_col = child.state(CONV_CHILD_INPUT_COL_ID);

    auto is_last_psum = WtIsLastPsum(child, wbact_row, wbact_col, wbk_row, wbk_col, wbact_chblk);
    auto en_bias = child.state(CONV_ENABLE_BIAS);
    auto chan_bias = child.state(CONV_CHAN_BIAS);

    oact_out = Ite(is_last_psum & (en_bias != 0), 
                   ConvAddBias(oact_out, chan_bias), oact_out);
    
    auto en_relu = child.state(CONV_ENABLE_RELU);

    oact_out = Ite(is_last_psum & (en_relu != 0), PsumRelu(oact_out), oact_out);
    auto oact_out_act = Psum2Act(oact_out);

    // ------------------------------------------------------------------
    for (auto i = 0; i < CONV_VECTOR_SIZE; i++) {
      auto out_element = child.state(GetStateName(CONV_CHILD_O_ACT_ARRAY, i));
      auto out_element_next = Ite(wbact_idx == i, oact_out_act, out_element);
      instr.SetUpdate(out_element, out_element_next);
    }

    auto next_state = BvConst(CONV_CHILD_STATE_OUT, ACCEL_CONV_CHILD_STATE_BITWIDTH);
    instr.SetUpdate(state, next_state);
  }

  { // instr ---- write result into spad1
    auto instr = child.NewInstr("conv_child_output");
    instr.SetDecode(is_child_valid & (state == CONV_CHILD_STATE_OUT));

    auto act_row = child.state(CONV_CHILD_INPUT_ROW_ID);
    auto act_col = child.state(CONV_CHILD_INPUT_COL_ID);
    auto k_row = child.state(CONV_CHILD_KERNEL_ROW_ID);
    auto k_col = child.state(CONV_CHILD_KERNEL_COL_ID);
    auto act_filter_id = child.state(CONV_CHILD_FILTER_ID);
    
    // this output address should be byte level
    auto out_addr = OutActGetAddr(child, act_row, act_col, k_row, k_col, act_filter_id);
    auto spad1_base_addr = out_addr - SPAD1_BASE_ADDR;
    auto spad1 = child.state(SCRATCH_PAD_1);
    auto spad1_next = spad1;

    for (auto i = 0; i < CONV_VECTOR_SIZE; i++) {
      auto out_element = child.state(GetStateName(CONV_CHILD_O_ACT_ARRAY, i));
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