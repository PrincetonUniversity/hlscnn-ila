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
                      Extract(m.input(TOP_ADDR_IN), 3, 0));
  start = start << 3; // multiply by 8 to get the byte level address of data

  auto data_in_0 = m.input(TOP_DATA_IN_0);
  auto data_in_1 = m.input(TOP_DATA_IN_1);
  auto data_in_2 = m.input(TOP_DATA_IN_2);
  auto data_in_3 = m.input(TOP_DATA_IN_3);
  auto data_in_4 = m.input(TOP_DATA_IN_4);
  auto data_in_5 = m.input(TOP_DATA_IN_5);
  auto data_in_6 = m.input(TOP_DATA_IN_6);
  auto data_in_7 = m.input(TOP_DATA_IN_7);
  auto data_in_8 = m.input(TOP_DATA_IN_8);
  auto data_in_9 = m.input(TOP_DATA_IN_9);
  auto data_in_10 = m.input(TOP_DATA_IN_10);
  auto data_in_11 = m.input(TOP_DATA_IN_11);
  auto data_in_12 = m.input(TOP_DATA_IN_12);
  auto data_in_13 = m.input(TOP_DATA_IN_13);
  auto data_in_14 = m.input(TOP_DATA_IN_14);
  auto data_in_15 = m.input(TOP_DATA_IN_15);

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
  auto is_write = (m.input(TOP_IF_WR) & ~m.input(TOP_IF_RD));
  // masked address.
  // "Mask off the top 8 bits, which represent the device memory map
  // offset from the CPU.""
  auto masked_addr = Concat(BvConst(0, 8), 
                            Extract(m.input(TOP_ADDR_IN), 23, 0));
  auto is_config_addr = (masked_addr < SPAD0_BASE_ADDR);

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
                     BvConst(0, masked_addr.bit_width()));
  
  std::string instr_name = "CFG_WR_" + reg_name;
  
  {
    auto instr = m.NewInstr(instr_name);
    instr.SetDecode(is_write & is_config_addr & (reg_id == reg_idx));

    instr.SetUpdate(m.state(reg_name), aligned_data);
  }
}

} // namespace hlscnn
} // namespace ilang