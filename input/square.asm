# s0: register address
# s1: upper address limit
# s2: odd numbers
# s3: squares

		li    $s0, 4			# s0 = 4 (starting address)
		li    $s2, 1			# s2 = 1
		li    $s1, 800			# s1 = 800
		li    $s3, 1			# s3 = 1
REPEAT: sw    $s3, ($s0)		# save s3 to memory at s0
		addiu $s0, $s0, 4		# s0 += 4 (byte addressed word address)
		addiu $s2, $s2, 2		# s3 += 2
		addu  $s3, $s3, $s2		# s4 += s2
		bne   $s0, $s1, REPEAT	# loop if s0 != s1