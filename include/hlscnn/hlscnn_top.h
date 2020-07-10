// File: hlscnn_top.h

#ifndef HLSCNN_TOP_H__
#define HLSCNN_TOP_H__

#include <ilang/ilang++.h>
#include <string>

#include <hlscnn/config_reg.h>
#include <hlscnn/utils.h>

namespace ilang {

namespace hlscnn {

Ila GetHlscnnIla(const std::string& model_name = "hlscnn");

void DefineConfigReg(Ila& m);

}
};

#endif // HLSCNN_TOP_H__
