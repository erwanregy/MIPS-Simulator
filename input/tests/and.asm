li $t0, 0
li $t1, 0
and $s0, $t0, $t1

li $t0, 0
li $t1, 1
and $s1, $t0, $t1

li $t0, 1
li $t1, 0
and $s2, $t0, $t1

li $t0, 1
li $t1, 1
and $s3, $t0, $t1

# expected output:
# s0 = 0
# s1 = 0
# s2 = 0
# s3 = 1