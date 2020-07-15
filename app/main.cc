// File: main.cc

#include <hlscnn/hlscnn_top.h>

#include <fstream>
#include <ilang/util/log.h>

using namespace ilang;

int main() {
  // get the ILA model
  auto hlscnn = hlscnn::GetHlscnnIla("hlscnn");

  ILA_INFO << "#instr: " << hlscnn.instr_num();
  ILA_INFO << "#input: " << hlscnn.input_num();
  ILA_INFO << "#state: " << hlscnn.state_num();
  for (auto i = 0; i < hlscnn.instr_num(); i++) {
    ILA_INFO << "instr." << i << " " << hlscnn.instr(i);
  }

  // // example - export ILA to Verilog
  // ILA_INFO << "Export " << hlscnn << " to hlscnn.v";
  // std::ofstream fw("hlscnn.v");
  // hlscnn.ExportToVerilog(fw);
  // fw.close();

  return 0;
}
