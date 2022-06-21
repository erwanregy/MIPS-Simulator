li $t0, 1
li $t1, 0
sltu $s0, $t0, $t1
sltu $s1, $t1, $t0

# expected output:
# s0 = 0
# s1 = 1