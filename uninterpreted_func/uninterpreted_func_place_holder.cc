#include <hlscnn.h>
#include <systemc.h>

// conv_accel.h:799
// multiply-accumulate operation
sc_biguint<32> hlscnn::ConvMac(sc_biguint<32> psum, sc_biguint<16> wt, sc_biguint<16> act) {
  
  sc_biguint<32> out = 1;
  return out;
}

// conv_accel.h: 802
// convert the macc_psum_int into activation
sc_biguint<16> hlscnn::ConvMacPsum2Act(sc_biguint<32> in) {
  sc_biguint<16> out = 1;
  
  return out;
}

// conv_accel.h: 1083
// Add two activations into a psum type
sc_biguint<32> hlscnn::ActAdd2Psum(sc_biguint<16> arg_0, sc_biguint<16> arg_1) {
  sc_biguint<32> out = 1;

  return out;
}

// conv_accel.h:1091
// add bias
sc_biguint<32> hlscnn::ConvAddBias(sc_biguint<32> in, sc_biguint<16> bias) {
  
  sc_biguint<32> out = 1;

  return out;
}

// conv_accel.h:1097
// psum relu function
sc_biguint<32> hlscnn::PsumRelu(sc_biguint<32> arg_0) {
  // convert the input to psum type
  sc_biguint<32> out = 1;
  
  return out;
}

// conv_accel.h:1100
// convert a psum type into activation
sc_biguint<16> hlscnn::Psum2Act(sc_biguint<32> arg_0) {
  sc_biguint<16> out = 1;

  return out;
}
