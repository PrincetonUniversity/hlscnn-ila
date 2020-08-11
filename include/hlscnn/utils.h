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

// File: utils.h

#ifndef UTILS_H__
#define UTILS_H__

#include <ilang/ilang++.h>
#include <hlscnn/hlscnn_top.h>

namespace ilang {
namespace hlscnn {

inline void DefineCfgReg_helper(Ila& m, const std::string& reg_name) {
  m.NewBvState(reg_name, CFG_REG_BITWIDTH);
}

inline std::string GetStateName(const std::string& name, const int& idx) {
  return (name + "_" + std::to_string(idx));
}

inline ExprRef act_gen_get_addr(Ila& child, const ExprRef& input_row,
                                            const ExprRef& input_col,
                                            const ExprRef& chan_block) {
//channel_block_address = base_addr + ((channel_block_idx*input_rows*input_cols*CHANNEL_BLOCK_SIZE) 
// + in_row*(input_cols*CHANNEL_BLOCK_SIZE) + in_col*CHANNEL_BLOCK_SIZE)*(ACTIVATION_TOT_WIDTH/8);
  auto base_addr = child.state(CFG_REG_ACCEL_CONV_ACT_BASE_ADDR);
  auto input_row_ext = Concat(BvConst(0,32-input_row.bit_width()), input_row);
  auto input_col_ext = Concat(BvConst(0,32-input_col.bit_width()), input_col);
  auto chan_block_ext = Concat(BvConst(0,32-chan_block.bit_width()), chan_block);
  auto chan_block_size = BvConst(CHANNEL_BLOCK_SIZE, 32);
  auto act_tot_width = BvConst(ACT_TOTAL_BITWIDTH, 32);

  auto act_addr = base_addr + 
                  ((chan_block_ext * input_row_ext * input_col_ext * chan_block_size) +
                    input_row_ext * (input_col_ext * chan_block_size) +
                    input_col_ext * chan_block_size) * (ACT_TOTAL_BITWIDTH/8);
  
  return act_addr;
}

inline ExprRef conv_out_of_bound(Ila& child, const ExprRef& input_row,
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
  

}

ExprRef GetCfgRegAlignedData(const Ila& m);

void SetConfigRegWrInstr(Ila& m, const int& reg_idx, const std::string& reg_name);

}

}

#endif // UTILS_H__