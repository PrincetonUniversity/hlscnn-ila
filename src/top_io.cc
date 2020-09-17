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

// File: top_io.cc

#include <ilang/ilang++.h>
#include <hlscnn/hlscnn_top.h>

namespace ilang {
namespace hlscnn {

void DefineTopIO(Ila& m) {

  // HLSCNN has two AXI interface, a slave interface and a master interface
  
  /////////////////////////////////////////////////////////////
  //  Defining the slave AXI interface
  /////////////////////////////////////////////////////////////

  m.NewBvInput(TOP_SLAVE_IF_WR, TOP_SLAVE_IF_WR_BITWIDTH);
  m.NewBvInput(TOP_SLAVE_IF_RD, TOP_SLAVE_IF_RD_BITWIDTH);

  // Top addr and data input
  m.NewBvInput(TOP_SLAVE_ADDR_IN, TOP_SLAVE_ADDR_IN_BITWIDTH);

  m.NewBvInput(TOP_SLAVE_DATA_IN_0, TOP_SLAVE_DATA_IN_BITWIDTH);
  m.NewBvInput(TOP_SLAVE_DATA_IN_1, TOP_SLAVE_DATA_IN_BITWIDTH);
  m.NewBvInput(TOP_SLAVE_DATA_IN_2, TOP_SLAVE_DATA_IN_BITWIDTH);
  m.NewBvInput(TOP_SLAVE_DATA_IN_3, TOP_SLAVE_DATA_IN_BITWIDTH);
  m.NewBvInput(TOP_SLAVE_DATA_IN_4, TOP_SLAVE_DATA_IN_BITWIDTH);
  m.NewBvInput(TOP_SLAVE_DATA_IN_5, TOP_SLAVE_DATA_IN_BITWIDTH);
  m.NewBvInput(TOP_SLAVE_DATA_IN_6, TOP_SLAVE_DATA_IN_BITWIDTH);
  m.NewBvInput(TOP_SLAVE_DATA_IN_7, TOP_SLAVE_DATA_IN_BITWIDTH);
  m.NewBvInput(TOP_SLAVE_DATA_IN_8, TOP_SLAVE_DATA_IN_BITWIDTH);
  m.NewBvInput(TOP_SLAVE_DATA_IN_9, TOP_SLAVE_DATA_IN_BITWIDTH);
  m.NewBvInput(TOP_SLAVE_DATA_IN_10, TOP_SLAVE_DATA_IN_BITWIDTH);
  m.NewBvInput(TOP_SLAVE_DATA_IN_11, TOP_SLAVE_DATA_IN_BITWIDTH);
  m.NewBvInput(TOP_SLAVE_DATA_IN_12, TOP_SLAVE_DATA_IN_BITWIDTH);
  m.NewBvInput(TOP_SLAVE_DATA_IN_13, TOP_SLAVE_DATA_IN_BITWIDTH);
  m.NewBvInput(TOP_SLAVE_DATA_IN_14, TOP_SLAVE_DATA_IN_BITWIDTH);
  m.NewBvInput(TOP_SLAVE_DATA_IN_15, TOP_SLAVE_DATA_IN_BITWIDTH);

  /////////////////////////////////////////////////////////////////
  // Defining the master interface
  /////////////////////////////////////////////////////////////////

  // this two are output
  m.NewBvState(TOP_MASTER_IF_RD, TOP_MASTER_IF_RD_BITWIDTH);
  m.NewBvState(TOP_MASTER_RD_ADDR_OUT, TOP_MASTER_RD_ADDR_OUT_BITWIDTH);

  // Top master data in
  m.NewBvInput(TOP_MASTER_RD_RESP_VALID_FLAG, TOP_MASTER_RD_RESP_VALID_FLAG_BITWIDTH);

  m.NewBvInput(TOP_MASTER_DATA_IN_0, TOP_MASTER_DATA_IN_BITWIDTH);
  m.NewBvInput(TOP_MASTER_DATA_IN_1, TOP_MASTER_DATA_IN_BITWIDTH);
  m.NewBvInput(TOP_MASTER_DATA_IN_2, TOP_MASTER_DATA_IN_BITWIDTH);
  m.NewBvInput(TOP_MASTER_DATA_IN_3, TOP_MASTER_DATA_IN_BITWIDTH);
  m.NewBvInput(TOP_MASTER_DATA_IN_4, TOP_MASTER_DATA_IN_BITWIDTH);
  m.NewBvInput(TOP_MASTER_DATA_IN_5, TOP_MASTER_DATA_IN_BITWIDTH);
  m.NewBvInput(TOP_MASTER_DATA_IN_6, TOP_MASTER_DATA_IN_BITWIDTH);
  m.NewBvInput(TOP_MASTER_DATA_IN_7, TOP_MASTER_DATA_IN_BITWIDTH);
  m.NewBvInput(TOP_MASTER_DATA_IN_8, TOP_MASTER_DATA_IN_BITWIDTH);
  m.NewBvInput(TOP_MASTER_DATA_IN_9, TOP_MASTER_DATA_IN_BITWIDTH);
  m.NewBvInput(TOP_MASTER_DATA_IN_10, TOP_MASTER_DATA_IN_BITWIDTH);
  m.NewBvInput(TOP_MASTER_DATA_IN_11, TOP_MASTER_DATA_IN_BITWIDTH);
  m.NewBvInput(TOP_MASTER_DATA_IN_12, TOP_MASTER_DATA_IN_BITWIDTH);
  m.NewBvInput(TOP_MASTER_DATA_IN_13, TOP_MASTER_DATA_IN_BITWIDTH);
  m.NewBvInput(TOP_MASTER_DATA_IN_14, TOP_MASTER_DATA_IN_BITWIDTH);
  m.NewBvInput(TOP_MASTER_DATA_IN_15, TOP_MASTER_DATA_IN_BITWIDTH);


  
}
  
} // namespace hlscnn
} // namespace ilang