// File: main.cc

#include <hlscnn/hlscnn_top.h>

#include <fstream>
#include <ilang/util/log.h>

using namespace ilang;

int main() {
  SetToStdErr(1);
  // get the ILA model
  auto hlscnn = hlscnn::GetHlscnnIla("hlscnn");

  ILA_INFO << "spad_capacity: " << std::hex << SPAD_CAPACITY;

  ILA_INFO << "#child: " << hlscnn.child_num();
  ILA_INFO << "#instr: " << hlscnn.instr_num();
  ILA_INFO << "#input: " << hlscnn.input_num();
  ILA_INFO << "#state: " << hlscnn.state_num();
  for (auto i = 0; i < hlscnn.instr_num(); i++) {
    ILA_INFO << "instr." << i << " " << hlscnn.instr(i);
  }
  for (auto i = 0; i < hlscnn.child_num(); i++) {
    ILA_INFO << "child #" << i << '\t' << hlscnn.child(i);
    ILA_INFO << "child #instr: " << hlscnn.child(i).instr_num();
    ILA_INFO << "child #state: " << hlscnn.child(i).state_num();
    for (auto j = 0; j < hlscnn.child(i).instr_num(); j++) {
      ILA_INFO << "child instr." << j << " " << hlscnn.child(i).instr(j);
    }
  }

  ExportSysCSim(hlscnn, "./sim_model", false);

  // // example - export ILA to Verilog
  // ILA_INFO << "Export " << hlscnn << " to hlscnn.v";
  // std::ofstream fw("hlscnn.v");
  // hlscnn.ExportToVerilog(fw);
  // fw.close();

  return 0;
}
