// File: hlscnn_top.cc

#include <hlscnn/hlscnn_top.h>

#include <ilang/util/log.h>

namespace ilang {

Ila GetHlscnnIla(const std::string& model_name) {
  auto m = Ila(model_name);

  // define interface and architectural states
  auto addr = m.NewBvInput("addr", 16);
  auto data = m.NewBvInput("data", 8);
  auto ctrl = m.NewBvState("ctrl", 8);
  auto mode = m.NewBvState("mode", 8);

  // define ILA valid function
  m.SetValid(addr >= 0xFF00 & addr < 0xFF10);

  // define instruction - decode and state update functions
  auto instr_wr_ctrl = m.NewInstr("WrCtrl");
  instr_wr_ctrl.SetDecode(addr == 0xFF00);
  instr_wr_ctrl.SetUpdate(ctrl, Ite(data < 8, data, ctrl));

  // TODO

  return m;
}

}; // namespace ilang
