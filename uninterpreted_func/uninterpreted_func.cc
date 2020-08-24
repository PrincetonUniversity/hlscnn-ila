#include <hlscnn.h>
#include <systemc.h>

  sc_biguint<32> hlscnn::ConvMacPsumMul(sc_biguint<32> psum, sc_biguint<16> wt, sc_biguint<16> act) {
    
    ac_int<32, false> psum_ac = psum.to_uint();
    ac_int<16, false> wt_ac = wt.to_uint();
    ac_int<16, false> act_ac = act.to_uint();

    conv_activation_t act_op;
    conv_weight_t wt_op;
    conv_psum_nornd_t macc_psum_int;

    wt_op.set_slc<16>(0, wt_ac);
    act_op.set_slc<16>(0, act_ac);
    macc_psum_int.set_slc<32>(0, psum_ac);

    macc_psum_int += wt_op*act_op;
    
    ac_int<32, false> out_ac = macc_psum_int.slc<32>(0);
    sc_biguint<32> out = out_ac.to_uint();
    return out;
  }

  sc_biguint<16> hlscnn::ConvMacPsum2Act(sc_biguint<32> in) {
    ac_int<32, false> in_ac = in.to_uint();

    conv_psum_nornd_t macc_psum_int;
    conv_activation_rnd_t macc_psum;
    conv_activation_t act_out;

    macc_psum_int.set_slc<32>(0, in_ac);
    macc_psum = conv_psum_t(macc_psum_int);
    act_out = conv_activation_t(macc_psum);

    ac_int<16, false> out_ac = act_out.slc<16>(0);
    sc_biguint<16> out = out_ac.to_uint();
    
    return out;
  }

  sc_biguint<32> hlscnn::ActAdd2Psum(sc_biguint<16> arg_0, sc_biguint<16> arg_1) {
    // convert the inputs to activations type
    ac_int<16, false> arg_0_ac = arg_0.to_uint();
    ac_int<16, false> arg_1_ac = arg_1.to_uint();

    conv_psum_t out_psum;
    conv_activation_t arg_0_act, arg_1_act;
    arg_0_act.set_slc<16>(0, arg_0_ac);
    arg_1_act.set_slc<16>(0, arg_1_ac);
    out_psum = arg_0_act + arg_1_act;

    ac_int<32, false> out_ac = out_psum.slc<32>(0);
    sc_biguint<32> out = out_ac.to_uint();

    return out_ac;
  }

  sc_biguint<32> hlscnn::PsumRelu(sc_biguint<32> arg_0) {
    // convert the input to psum type
    ac_int<32, false> arg_0_ac = arg_0.to_uint();

    conv_psum_t out_psum, arg_0_psum;
    arg_0_psum.set_slc<32>(0, arg_0_ac);
    out_psum = (arg_0_psum > 0) ? arg_0_psum : conv_psum_t(0);

    ac_int<32, false> out_ac = out_psum.slc<32>(0);
    sc_biguint<32> out = out_ac.to_uint();
    
    return out;
  }

  sc_biguint<16> hlscnn::Psum2Act(sc_biguint<32> arg_0) {
    ac_int<32, false> arg_0_ac = arg_0.to_uint();
    
    conv_psum_t arg_0_psum;
    conv_activation_t out_act;

    arg_0_psum.set_slc<32>(0, arg_0_ac);
    out_act = conv_activation_t(arg_0_psum);

    ac_int<16, false> out_ac = out_act.slc<16>(0);
    sc_biguint<16> out = out_ac.to_uint();

    return out;
  }
