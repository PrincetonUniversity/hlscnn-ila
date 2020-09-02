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

// File: arch_state.cc

#include <ilang/ilang++.h>
#include <hlscnn/hlscnn_top.h>

namespace ilang {
namespace hlscnn {

void DefineArchState(Ila& m) {
  // scratchpad0 and scratchpad1 are declared as memstate here
  m.NewMemState(SCRATCH_PAD_0, TOP_SLAVE_ADDR_IN_BITWIDTH, SCRATCH_PAD_DATA_BITWIDTH);
  m.state(SCRATCH_PAD_0).SetEntryNum(SPAD_BYTE_ENTRY_NUM);

  m.NewMemState(SCRATCH_PAD_1, TOP_SLAVE_ADDR_IN_BITWIDTH, SCRATCH_PAD_DATA_BITWIDTH);
  m.state(SCRATCH_PAD_1).SetEntryNum(SPAD_BYTE_ENTRY_NUM);

  // ---------------------------------------------------------------------------------
  // Declare a virtual memeory state for external memory
  // ---------------------------------------------------------------------------------
  m.NewMemState(VIRTUAL_SOC_MEMORY, TOP_SLAVE_ADDR_IN_BITWIDTH, VIRTUAL_SOC_MEMORY_DATA_BITWIDTH);
  m.state(VIRTUAL_SOC_MEMORY).SetEntryNum(VIRTUAL_SOC_MEMORY_BYTE_ENTRY_NUM);
}


} // namespace hlscnn
} // namespace ilang
