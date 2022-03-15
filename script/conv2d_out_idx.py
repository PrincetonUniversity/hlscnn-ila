from math import ceil

in_chans = 8
in_rows = 8
in_cols = 8
num_filters = 10
kern_rows = 3
kern_cols = 3
kernel_stride = 2

out_rows = int((in_rows - kern_rows) / kernel_stride + 1)
out_cols = int((in_cols - kern_cols) / kernel_stride + 1)

def old_out_idx_gen(i_row, i_col, k_row, k_col):
    """ Old way of getting the output index given the input and kernel index """
    o_row = i_row + (int(kern_rows/2)) - k_row
    o_col = i_col + (int(kern_cols/2)) - k_col
    return o_row, o_col

def new_out_idx_gen(i_row, i_col, k_row, k_col):
    """ New way of getting output index from input and kernel indice """
    o_row = int((i_row - k_row) / kernel_stride)
    o_col = int((i_col - k_col) / kernel_stride)
    return o_row, o_col

def old_out_of_bound(i_row, i_col, k_row, k_col):
    """ The old out-of-bound checking function
    """
    o_row, o_col = old_out_idx_gen(i_row, i_col, k_row, k_col)
    cond = ((o_row >= in_rows) or (o_col >= in_cols) or (o_row < 0) or (o_col < 0))
    print("\tOut of bound checking [OLD]:", o_row, o_col, cond)
    return cond

# def new_out_addr_gen()
def new_out_of_bound(i_row, i_col, k_row, k_col):
    """ THe new out-of-bound checking function
    """
    o_row, o_col = new_out_idx_gen(i_row, i_col, k_row, k_col)
    cond = (o_row >= out_rows) or (o_col >= out_cols) or (o_row < 0) or (o_col < 0)
    print("\tOut of bound checking [New]:", o_row, o_col, cond)
    return cond

def act_addr_gen(i_row, i_col, chan_blk_id):
    return (
        chan_blk_id * in_rows * in_cols * 8 + 
        i_row * in_cols * 8 + 
        i_col * 8
    ) * 2

def old_out_addr_gen(i_row, i_col, k_row, k_col, filter_idx):
    """ Old output activation address generation
    """
    o_row, o_col = old_out_idx_gen(i_row, i_col, k_row, k_col)
    chan_block_id = int(filter_idx / 8)
    return int((
        (chan_block_id * in_rows * in_cols * 8) + 
        o_row * in_cols * 8 +
        o_col * 8
    ) * 2 / 16)

def new_out_addr_gen(i_row, i_col, k_row, k_col, filter_idx):
    """ New output address generation
    """
    o_row, o_col = new_out_idx_gen(i_row, i_col, k_row, k_col)
    chan_block_id = int(filter_idx / 8)
    return chan_block_id * out_rows * out_cols + o_row * out_cols + o_col


""" Old Conv loop """
out_elem_cnt = 0
for n in range(num_filters):
    for i_chanblk in range(ceil(in_chans / 8)):
        for i_row in range(in_rows):
            for i_col in range(in_cols):
                print("[Fetch_INPUT_ACT]: " + 
                      f"in_chanblk: {i_chanblk} " +
                      f"i_row: {i_row} " +
                      f"i_col: {i_col} " + 
                      f"block address: {act_addr_gen(i_row, i_col, i_chanblk):0x}")
                for k_row in range(i_row % kernel_stride, kern_rows, kernel_stride):
                    for k_col in range(i_col % kernel_stride, kern_cols, kernel_stride):
                        if (old_out_of_bound(i_row, i_col, k_row, k_col)):
                            continue
                        print(
                            "[Output_ACT]: " + 
                            f"filter_id: {n} " +
                            f"in_chan_blk: {i_chanblk} " +
                            f"act_row: {i_row} " + 
                            f"act_col: {i_col} " +
                            f"k_row: {k_row} " +
                            f"k_col: {k_col} " + 
                            f"output_chan_blk: {int(n/8)}"
                            )
                        # old_out_of_bound(i_row, i_col, k_row, k_col)
                        # new_out_of_bound(i_row, i_col, k_row, k_col)
                        print("\tAddr:", old_out_addr_gen(i_row, i_col, k_row, k_col, n))
                        out_elem_cnt += 1
                print("\n")

print("Old out elements count:", out_elem_cnt)

""" New Conv loop
"""
out_elem_cnt = 0
for n in range(num_filters):
    for i_chanblk in range(ceil(in_chans / 8)):
        for i_row in range(in_rows):
            for i_col in range(in_cols):
                print("[Fetch_INPUT_ACT]: " + 
                      f"in_chanblk: {i_chanblk} " +
                      f"i_row: {i_row} " +
                      f"i_col: {i_col} " + 
                      f"block address: {act_addr_gen(i_row, i_col, i_chanblk):0x}")
                for k_row in range(i_row % kernel_stride, kern_rows, kernel_stride):
                    for k_col in range(i_col % kernel_stride, kern_cols, kernel_stride):
                        if (new_out_of_bound(i_row, i_col, k_row, k_col)):
                            continue
                        print(
                            "[Output_ACT]: " + 
                            f"filter_id: {n} " +
                            f"in_chan_blk: {i_chanblk} " +
                            f"act_row: {i_row} " + 
                            f"act_col: {i_col} " +
                            f"k_row: {k_row} " +
                            f"k_col: {k_col} " + 
                            f"output_chan_blk: {int(n/8)}"
                            )
                        # old_out_of_bound(i_row, i_col, k_row, k_col)
                        # new_out_of_bound(i_row, i_col, k_row, k_col)
                        print("\tAddr:", new_out_addr_gen(i_row, i_col, k_row, k_col, n), 
                                         new_out_idx_gen(i_row, i_col, k_row, k_col))
                        out_elem_cnt += 1
                print("\n")

print("New out element count: ", out_elem_cnt)