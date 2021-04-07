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

// File: utils.cc

#include <ilang/ilang++.h>
#include <hlscnn/hlscnn_top.h>

namespace ilang {
namespace hlscnn{

ExprRef GetCfgRegAlignedData(const Ila& m) {
  auto start = Concat(BvConst(0, 28),
                      Extract(m.input(TOP_SLAVE_ADDR_IN), 3, 0));
  // start = start << 3; // multiply by 8 to get the byte level address of data

  auto data_in_0 = m.input(TOP_SLAVE_DATA_IN_0);
  auto data_in_1 = m.input(TOP_SLAVE_DATA_IN_1);
  auto data_in_2 = m.input(TOP_SLAVE_DATA_IN_2);
  auto data_in_3 = m.input(TOP_SLAVE_DATA_IN_3);
  auto data_in_4 = m.input(TOP_SLAVE_DATA_IN_4);
  auto data_in_5 = m.input(TOP_SLAVE_DATA_IN_5);
  auto data_in_6 = m.input(TOP_SLAVE_DATA_IN_6);
  auto data_in_7 = m.input(TOP_SLAVE_DATA_IN_7);
  auto data_in_8 = m.input(TOP_SLAVE_DATA_IN_8);
  auto data_in_9 = m.input(TOP_SLAVE_DATA_IN_9);
  auto data_in_10 = m.input(TOP_SLAVE_DATA_IN_10);
  auto data_in_11 = m.input(TOP_SLAVE_DATA_IN_11);
  auto data_in_12 = m.input(TOP_SLAVE_DATA_IN_12);
  auto data_in_13 = m.input(TOP_SLAVE_DATA_IN_13);
  auto data_in_14 = m.input(TOP_SLAVE_DATA_IN_14);
  auto data_in_15 = m.input(TOP_SLAVE_DATA_IN_15);

  auto aligned_data =
    Ite(start == 0, Concat(data_in_3, Concat(data_in_2, Concat(data_in_1, data_in_0))),
    Ite(start == 1, Concat(data_in_4, Concat(data_in_3, Concat(data_in_2, data_in_1))),
    Ite(start == 2, Concat(data_in_5, Concat(data_in_4, Concat(data_in_3, data_in_2))),
    Ite(start == 3, Concat(data_in_6, Concat(data_in_5, Concat(data_in_4, data_in_3))),
    Ite(start == 4, Concat(data_in_7, Concat(data_in_6, Concat(data_in_5, data_in_4))),
    Ite(start == 5, Concat(data_in_8, Concat(data_in_7, Concat(data_in_6, data_in_5))),
    Ite(start == 6, Concat(data_in_9, Concat(data_in_8, Concat(data_in_7, data_in_6))),
    Ite(start == 7, Concat(data_in_10, Concat(data_in_9, Concat(data_in_8, data_in_7))),
    Ite(start == 8, Concat(data_in_11, Concat(data_in_10, Concat(data_in_9, data_in_8))),
    Ite(start == 9, Concat(data_in_12, Concat(data_in_11, Concat(data_in_10, data_in_9))),
    Ite(start == 10, Concat(data_in_13, Concat(data_in_12, Concat(data_in_11, data_in_10))),
    Ite(start == 11, Concat(data_in_14, Concat(data_in_13, Concat(data_in_12, data_in_11))),
                     Concat(data_in_15, Concat(data_in_14, Concat(data_in_13, data_in_12)))
                     ))))))))))));

  return aligned_data;
}

void SetConfigRegWrInstr(Ila& m, const int& reg_idx, const std::string& reg_name) {
  // define config write instructions
  auto is_write = (m.input(TOP_SLAVE_IF_WR) & ~m.input(TOP_SLAVE_IF_RD));
  // masked address.
  // "Mask off the top 8 bits, which represent the device memory map
  // offset from the CPU.""
  auto masked_addr = Concat(BvConst(0, 8), 
                            Extract(m.input(TOP_SLAVE_ADDR_IN), 23, 0));
  auto not_vir_access = (m.input(VIRTUAL_SOC_ACCESS) == 0);
  auto is_config_addr = (masked_addr < SPAD0_BASE_ADDR) & not_vir_access;

  // get the aligned data. Reg data is only 32 bit

  // " For config reg writes only:
  //
  // Get the right 32-bit slice of the 128-bit data. This can be determined
  // by looking at the last four bits of the address - 128-bit data is 16
  // bytes, which takes 4 bits of addressing. Take that value and multiply
  // it by 8 (8 bits per byte) to get the start of the right 32-bit slice.
  // "

  auto aligned_data = GetCfgRegAlignedData(m);

  auto reg_id = URem(masked_addr >> CFG_REG_SIZE_BITWIDTH,
                     BvConst(NumCfgRegisters, masked_addr.bit_width()));
  
  std::string instr_name = "CFG_WR_" + reg_name;
  
  {
    auto instr = m.NewInstr(instr_name);
    instr.SetDecode(is_write & is_config_addr & (reg_id == reg_idx));

    instr.SetUpdate(m.state(reg_name), aligned_data);
  }
}

ExprRef act_gen_get_addr(const Ila& child, const ExprRef& in_row,
                                            const ExprRef& in_col,
                                            const ExprRef& chan_block_idx) {
//channel_block_address = base_addr + ((channel_block_idx*input_rows*input_cols*CHANNEL_BLOCK_SIZE) 
// + in_row*(input_cols*CHANNEL_BLOCK_SIZE) + in_col*CHANNEL_BLOCK_SIZE)*(ACTIVATION_TOT_WIDTH/8);
  auto base_addr = child.state(CFG_REG_ACCEL_CONV_ACT_BASE_ADDR);
  auto in_row_ext = Concat(BvConst(0,32-in_row.bit_width()), in_row);
  auto in_col_ext = Concat(BvConst(0,32-in_col.bit_width()), in_col);
  auto chan_block_ext = Concat(BvConst(0,32-chan_block_idx.bit_width()), chan_block_idx);

  auto chan_block_size = BvConst(CHANNEL_BLOCK_SIZE, 32);
  auto act_tot_width = BvConst(ACT_TOTAL_BITWIDTH, 32);

  auto input_rows_ext = Concat(BvConst(0, 32-CONV_INPUT_ROW_NUM_BITWIDTH),
                                child.state(CONV_INPUT_ROW_NUM));
  auto input_cols_ext = Concat(BvConst(0, 32-CONV_INPUT_COL_NUM_BITWIDTH),
                                child.state(CONV_INPUT_COL_NUM));

  auto act_addr = base_addr + 
                  ((chan_block_ext * input_rows_ext * input_cols_ext * chan_block_size) +
                    in_row_ext * (input_cols_ext * chan_block_size) +
                    in_col_ext * chan_block_size) * (ACT_TOTAL_BITWIDTH/8);
  
  return act_addr;
}

ExprRef conv_out_of_bound(const Ila& child, const ExprRef& input_row,
                                                   const ExprRef& input_col,
                                                   const ExprRef& k_row,
                                                   const ExprRef& k_col)
{
  // implement the out_of_bound member function
  // virtual bool out_of_bound(unsigned input_row, unsigned input_col, unsigned k_row, unsigned k_col) {
	// 	signed out_row = input_row + ((last_kern_row/2) - k_row);
	// 	signed out_col = input_col + ((last_kern_col/2) - k_col);
	// 	bool   idx_out_of_bound = false;
	// 	if((out_row >= last_row) || (out_col >= last_col) || (out_row < 0) || (out_col < 0) ) {
	// 		idx_out_of_bound = true;
	// 	}
	// 	return idx_out_of_bound;
	// }
  auto last_kernel_row = child.state(CONV_KERNEL_ROW_NUM);
  auto last_kernel_col = child.state(CONV_KERNEL_COL_NUM);
  auto last_row = child.state(CONV_INPUT_ROW_NUM);
  auto last_col = child.state(CONV_INPUT_COL_NUM);

  // input_row and input_col have the largest bit-width
  auto ext_bitwidth = input_row.bit_width();
  // input_row and input_col should have the same bitwidth.
  ILA_ASSERT(input_row.bit_width() == input_col.bit_width());

  auto last_kernel_row_ext = Concat(BvConst(0, ext_bitwidth-last_kernel_row.bit_width()),
                                    last_kernel_row);
  auto last_kernel_col_ext = Concat(BvConst(0, ext_bitwidth-last_kernel_col.bit_width()),
                                    last_kernel_col);
  auto last_row_ext = Concat(BvConst(0, ext_bitwidth-last_row.bit_width()), last_row);
  auto last_col_ext = Concat(BvConst(0, ext_bitwidth-last_col.bit_width()), last_col);
  auto k_row_ext = Concat(BvConst(0, ext_bitwidth-k_row.bit_width()), k_row);
  auto k_col_ext = Concat(BvConst(0, ext_bitwidth-k_col.bit_width()), k_col);

  auto cond_0 = 
        ((input_row + last_kernel_row_ext / BvConst(2,ext_bitwidth) - k_row_ext) >= last_row_ext);
  auto cond_1 = 
        ((input_col + last_kernel_col_ext / BvConst(2,ext_bitwidth) - k_col_ext) >= last_col_ext);
  auto cond_2 = 
        ((input_row + last_kernel_row_ext / BvConst(2,ext_bitwidth)) < k_row_ext);
  auto cond_3 = 
        ((input_col + last_kernel_col_ext / BvConst(2,ext_bitwidth)) < k_col_ext);
  
  auto is_out_of_bound = cond_0 | cond_1 | cond_2 | cond_3;

  return is_out_of_bound;
}

ExprRef WtGetAddr(const Ila& child, const ExprRef& filter_id,
                                               const ExprRef& k_row,
                                               const ExprRef& k_col,
                                               const ExprRef& chan_block)
{
  // channel_block_address = base_addr + 
  // ((filter_idx*kernel_rows*kernel_cols*last_channel_block*CHANNEL_BLOCK_SIZE) + 
  // (channel_block_idx*kernel_rows*kernel_cols*CHANNEL_BLOCK_SIZE) + 
  // k_row*(kernel_cols*CHANNEL_BLOCK_SIZE) + k_col*CHANNEL_BLOCK_SIZE) * 
  // (WEIGHT_TOT_WIDTH/8)/(NIC_MEM_ELEM_WIDTH);
  auto base_addr = child.state(CONV_WEIGHT_BASE);
  auto kernel_rows = child.state(CONV_KERNEL_ROW_NUM);
  auto kernel_cols = child.state(CONV_KERNEL_COL_NUM);
  auto input_channels = child.state(CONV_INPUT_CHAN_NUM);

  auto chan_block_size = BvConst(CHANNEL_BLOCK_SIZE, input_channels.bit_width());
  auto last_chan_block = 
    Ite(URem(input_channels, chan_block_size) == 0,
        input_channels / chan_block_size, input_channels / chan_block_size + 1);

  auto filter_id_ext = Concat(BvConst(0, 32-filter_id.bit_width()), filter_id);
  auto k_row_ext = Concat(BvConst(0, 32-k_row.bit_width()), k_row);
  auto k_col_ext = Concat(BvConst(0, 32-k_col.bit_width()), k_col);
  auto chan_block_ext = Concat(BvConst(0, 32-chan_block.bit_width()), chan_block);

  auto kernel_rows_ext = Concat(BvConst(0, 32-kernel_rows.bit_width()), kernel_rows);
  auto kernel_cols_ext = Concat(BvConst(0, 32-kernel_cols.bit_width()), kernel_cols);

  auto last_chan_block_ext = Concat(BvConst(0, 32-last_chan_block.bit_width()), last_chan_block);

  // update 08242020: no need to add the base address for ILA mem access.
  auto addr = (
    (filter_id_ext * kernel_rows_ext * kernel_cols_ext * last_chan_block_ext * CHANNEL_BLOCK_SIZE) +
    (chan_block_ext * kernel_rows_ext * kernel_cols_ext * CHANNEL_BLOCK_SIZE) +
    k_row_ext * (kernel_cols_ext * CHANNEL_BLOCK_SIZE) +
    k_col_ext * CHANNEL_BLOCK_SIZE
    ) * (WEIGHT_TOTAL_BITWIDTH/8) / BvConst(NIC_MEM_ELEM_BYTEWIDTH, 32);
  
  return addr;
}

ExprRef OutActGetAddr(const Ila& child, const ExprRef& input_row,
                                        const ExprRef& input_col,
                                        const ExprRef& k_row,
                                        const ExprRef& k_col,
                                        const ExprRef& filter_idx)
{
  auto last_kernel_row = child.state(CONV_KERNEL_ROW_NUM);
  auto last_kernel_col = child.state(CONV_KERNEL_COL_NUM);
  auto last_row = child.state(CONV_INPUT_ROW_NUM);
  auto last_col = child.state(CONV_INPUT_COL_NUM);

  auto base_addr = child.state(CONV_SPAD_OUTPUT_BASE);
  // extend bitwidth to spad addr bitwidth (32)
  auto ext_bitwidth = base_addr.bit_width();

  auto input_row_ext = Concat(BvConst(0, ext_bitwidth-input_row.bit_width()), input_row);
  auto input_col_ext = Concat(BvConst(0, ext_bitwidth-input_col.bit_width()), input_col);
  auto k_row_ext = Concat(BvConst(0, ext_bitwidth-k_row.bit_width()), k_row);
  auto k_col_ext = Concat(BvConst(0, ext_bitwidth-k_col.bit_width()), k_col);
  auto filter_idx_ext = Concat(BvConst(0, ext_bitwidth-filter_idx.bit_width()), filter_idx);

  auto last_kernel_row_ext = Concat(BvConst(0, ext_bitwidth-last_kernel_row.bit_width()), 
                                    last_kernel_row);
  auto last_kernel_col_ext = Concat(BvConst(0, ext_bitwidth-last_kernel_col.bit_width()),
                                    last_kernel_col);
  auto last_row_ext = Concat(BvConst(0, ext_bitwidth-last_row.bit_width()), last_row);
  auto last_col_ext = Concat(BvConst(0, ext_bitwidth-last_col.bit_width()), last_col);

  auto out_row = input_row_ext - k_row_ext + last_kernel_row_ext/BvConst(2, ext_bitwidth);
  auto out_col = input_col_ext - k_col_ext + last_kernel_col_ext/BvConst(2, ext_bitwidth);

  auto out_act_addr = 
        (
          (filter_idx_ext * last_row_ext * last_col_ext * CHANNEL_BLOCK_SIZE) +
          out_row * (last_col_ext * CHANNEL_BLOCK_SIZE) +
          out_col * CHANNEL_BLOCK_SIZE
        ) * (ACT_TOTAL_BITWIDTH/8) / BvConst(NIC_MEM_ELEM_BYTEWIDTH, ext_bitwidth);

  return out_act_addr;
}

ExprRef WtIsLastPsum(const Ila& child, const ExprRef& act_row,
                                       const ExprRef& act_col,
                                       const ExprRef& k_row,
                                       const ExprRef& k_col,
                                       const ExprRef& chan_block)
{
  auto last_kernel_row = child.state(CONV_KERNEL_ROW_NUM);
  auto last_kernel_col = child.state(CONV_KERNEL_COL_NUM);
  auto input_channels = child.state(CONV_INPUT_CHAN_NUM);
  auto chan_block_size = BvConst(CHANNEL_BLOCK_SIZE, input_channels.bit_width());
  auto last_chan_block = 
    Ite(URem(input_channels, chan_block_size) == 0,
        input_channels / chan_block_size, input_channels / chan_block_size + 1);
  
  auto last_kernel_row_ext = Concat(BvConst(0, k_row.bit_width()-last_kernel_row.bit_width()),
                                    last_kernel_row);
  auto last_kernel_col_ext = Concat(BvConst(0, k_col.bit_width()-last_kernel_col.bit_width()),
                                    last_kernel_col);
  auto last_chan_block_ext = Concat(BvConst(0, chan_block.bit_width()-last_chan_block.bit_width()),
                                    last_chan_block);

  auto is_last_psum = (k_row == last_kernel_row_ext - 1) & 
                      (k_col == last_kernel_col_ext - 1) &
                      (chan_block == last_chan_block_ext - 1);

  return is_last_psum;
}

} // namespace hlscnn
} // namespace ilang