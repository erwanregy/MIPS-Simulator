li $t0, 0
li $t1, 0
or $s0, $t0, $t1

li $t0, 0
li $t1, 1
or $s1, $t0, $t1

li $t0, 1
li $t1, 0
or $s2, $t0, $t1

li $t0, 1
li $t1, 1
or $s3, $t0, $t1

# expected output:
# s0 = 0
# s1 = 1
# s2 = 1
# s3 = 1