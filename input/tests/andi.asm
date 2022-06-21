li $t0, 0
andi $s0, $t0, 0

li $t0, 0
andi $s1, $t0, 1

li $t0, 1
andi $s2, $t0, 0

li $t0, 1
andi $s3, $t0, 1

# expected output:
# s0 = 0
# s1 = 0
# s2 = 0
# s3 = 1
