        li $t0, 2
        # comment
LABEL:  addiu $s0, $s0, 1  # comment
        j LABEL
        bne $t0, $t1, LABEL

# expected output
# s0 = 1