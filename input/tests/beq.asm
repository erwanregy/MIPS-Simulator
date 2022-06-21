        li $t0, 1
LABEL:  addiu $s0, $s0, 1
        addiu $t1, $t1, 1
        beq $t0, $t1, LABEL

# expected output
# s0 = 2