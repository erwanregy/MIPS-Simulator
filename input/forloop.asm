main:
	li $a0, 5234
	li $a1, 4524

multiply:	
	li $t0, 0
	li $t1, 0
LOOP:
	addu $t0, $t0, $a1
	addiu $t1, $t1, 1
	bne $t1, $a0, LOOP
	move $v0, $t1