# Instruction-level Abstraction (ILA) of HLSCNN Accelerator

This is the ILA model of [HLSCNN](https://github.com/ttambe/HLSCNN_Accel) accelerator (all right reserved - Harvard University)

*** important! ***
This branch is based on a modified version of the HLSCNN design. Modifications include:
- Expanding the scratchpad sizes
- Grouping the output activation by CHANNEL_BLOCK_SIZE
- Correct output activation indexing
- Expanding the weight bitwidth from 8bit to 16bit
