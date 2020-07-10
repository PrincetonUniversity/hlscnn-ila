// File: main.cc

#include <hlscnn/hlscnn_top.h>

#include <fstream>
#include <ilang/util/log.h>

using namespace ilang;

int main() {
  // get the ILA model
  auto hlscnn = GetHlscnnIla("hlscnn");

  // example - export ILA to Verilog
  ILA_INFO << "Export " << hlscnn << " to hlscnn.v";
  std::ofstream fw("hlscnn.v");
  hlscnn.ExportToVerilog(fw);
  fw.close();

  return 0;
}
