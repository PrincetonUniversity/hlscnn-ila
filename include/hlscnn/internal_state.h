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

// File: internal_state.h

#ifndef _INTERNAL_STATE_H__
#define _INTERNAL_STATE_H__

#include <hlscnn/conv_param.h>

namespace ilang {
namespace hlscnn {

//////////////////////////////////////////////////
//      internal states for Master AXI interface
//////////////////////////////////////////////////
#define ACCEL_SPAD_WR_ADDR "accel_spad_wr_addr"
#define ACCEL_SPAD_WR_ADDR_BITWIDTH TOP_SLAVE_ADDR_IN_BITWIDTH

#define ACCEL_MASTER_AXI_CHILD_VALID 1
#define ACCEL_MASTER_AXI_CHILD_INVALID 0

#define ACCEL_MASTER_AXI_CHILD_VALID_FLAG "accel_master_axi_child_valid_flag"
#define ACCEL_MASTER_AXI_CHILD_VALID_FLAG_BITWIDTH 1

#define ACCEL_MASTER_AXI_CHILD_STATE "accel_master_axi_child_state"
#define ACCEL_MASTER_AXI_CHILD_STATE_BITWIDTH 3

#define MASTER_AXI_CHILD_STATE_SPAD0_RD 0
#define MASTER_AXI_CHILD_STATE_SPAD1_RD 1
#define MASTER_AXI_CHILD_STATE_ACT_RD 2

#define ACCEL_MASTER_AXI_CHILD_RD_RECV_VALID_FLAG                                     \
  "accel_master_axi_child_rd_recv_valid_flag"
#define ACCEL_MASTER_AXI_CHILD_RD_RECV_VALID_FLAG_BITWIDTH 1
/////////////////////////////////////////////
//      interanl states of Conv
/////////////////////////////////////////////
#define ACCEL_CONV_CHILD_VALID 1
#define ACCEL_CONV_CHILD_INVALID 0

#define ACCEL_CONV_CHILD_VALID_FLAG "accel_conv_child_valid_flag"
#define ACCEL_CONV_CHILD_VALID_FLAG_BITWIDTH 1

#define ACCEL_CONV_CHILD_STATE "accel_conv_child_state"
#define ACCEL_CONV_CHILD_STATE_BITWIDTH 4

#define CONV_CHILD_STATE_IDLE 0
#define CONV_CHILD_STATE_ACT_FILTER_ID 1
#define CONV_CHILD_STATE_ACT_INPUT_CHANNEL_BLOCK 2
#define CONV_CHILD_STATE_ACT_INPUT_ROW 3
#define CONV_CHILD_STATE_ACT_INPUT_COL 4

#define CONV_CHILD_STATE

#define CONV_CHILD_FILTER_ID "conv_child_filter_id"
#define CONV_CHILD_FILTER_ID_BITWIDTH CONV_FILTER_SIZE_T

#define CONV_CHILD_CHAN_BLOCK_ID "conv_child_chan_block_id"
#define CONV_CHILD_CHAN_BLOCK_ID_BITWIDTH CONV_CHANNEL_SIZE_T

#define CONV_CHILD_INPUT_ROW_ID "conv_child_input_row_id"
#define CONV_CHILD_INPUT_ROW_ID_BITWIDTH CONV_ROW_SIZE_T

#define CONV_CHILD_INPUT_COL_ID "conv_child_input_col_id"
#define CONV_CHILD_INPUT_COL_ID_BITWIDTH CONV_ROW_SIZE_T






} // namespace hlscnn
} // namespace ilang;

#endif // _INTERNAL_STATE_H__