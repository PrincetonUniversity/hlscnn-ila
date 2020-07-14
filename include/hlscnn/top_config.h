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

// File: top_config.h

#ifndef TOP_CONFIG_H__
#define TOP_CONFIG_H__

namespace ilang {
namespace hlscnn {

  #define TOP_IF_WR "top_if_wr"
  #define TOP_IF_WR_BITWIDTH 1

  #define TOP_IF_RD "top_if_rd"
  #define TOP_IF_RD_BITWIDTH 1
  // AXI protocol
  // axi_cfg.h
  #define TOP_ADDR_IN "top_addr_in"
  #define TOP_ADDR_IN_BITWIDTH 32

  #define TOP_DATA_IN_0 "data_in_0"
  #define TOP_DATA_IN_1 "data_in_1"
  #define TOP_DATA_IN_2 "data_in_2"
  #define TOP_DATA_IN_3 "data_in_3"
  #define TOP_DATA_IN_4 "data_in_4"
  #define TOP_DATA_IN_5 "data_in_5"
  #define TOP_DATA_IN_6 "data_in_6"
  #define TOP_DATA_IN_7 "data_in_7"
  #define TOP_DATA_IN_8 "data_in_8"
  #define TOP_DATA_IN_9 "data_in_9"
  #define TOP_DATA_IN_10 "data_in_10"
  #define TOP_DATA_IN_11 "data_in_11"
  #define TOP_DATA_IN_12 "data_in_12"
  #define TOP_DATA_IN_13 "data_in_13"
  #define TOP_DATA_IN_14 "data_in_14"
  #define TOP_DATA_IN_15 "data_in_15"

  #define TOP_DATA_IN_BITWIDTH 8

  // memory address 
  // memory_map.h

  // scratch pad parameters
  #define SPAD_NUM_BANK 4
  #define SPAD_DATA_BYTE_WIDTH 16
  #define SPAD_CAPACITY (SPAD_NUM_BANK * 2048)

  // base addr for configurations
  #define CONFIG_BASE_ADDR 0X0
  // base address for scratchpad 0
  #define SPAD0_BASE_ADDR 0x4000
  // base address for scratchpad 1
  #define SPAD1_BASE_ADDR (SPAD0_BASE_ADDR + SPAD_DATA_BYTE_WIDTH * SPAD_CAPACITY)
  // upper bound for valid address
  #define MEM_ADDR_MAX (SPAD1_BASE_ADDR + SPAD_DATA_BYTE_WIDTH * SPAD_CAPACITY)
   
} // namespace hlscnn
} // namespace ilang



#endif // TOP_CONFIG_H__