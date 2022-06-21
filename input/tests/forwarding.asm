li $t1, 1
li $t2, 1
li $t3, 1
li $t4, 1

subu $s0, $t1, $t2
and  $s1, $s0, $t3
or   $s2, $t4, $s0
addu $s3, $s0, $s0
sw   $t4, 100($s0)

# expected output
# s0 = 1
# s1 = 0
# s2 = 1
# s3 = 0
