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
# s0 = 1
# s1 = 0
# s2 = 0
# s3 = 0