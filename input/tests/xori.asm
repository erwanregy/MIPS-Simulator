li $t0, 0
ori $s0, $t0, 0

li $t0, 0
ori $s1, $t0, 1

li $t0, 1
ori $s2, $t0, 0

li $t0, 1
ori $s3, $t0, 1

# expected output:
# s0 = 0
# s1 = 1
# s2 = 1
# s3 = 0