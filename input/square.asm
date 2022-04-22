# s0: integers between 0 and 200
# s1: register address
# s2: incrementing odd numbers
# s3: upper limit
# s4: squares of s0

		li    $s0, 1			# s0 = 1
		li    $s1, 0			# s1 = 0 (base address of data memory)
		li    $s2, 1			# s2 = 1
		li    $s3, 200			# s3 = 200
		li    $s4, 1			# s4 = 1
REPEAT: sw    $s4, ($s1)		# save s4 to memory at s1
		addiu $s0, $s0, 1		# s0++
		addiu $s1, $s1, 4		# s1 += 4 (byte addressed word address)
		addiu $s2, $s2, 2		# s2 += 2
		addu  $s4, $s4, $s2		# s4 += s2
		bne   $s0, $s3, REPEAT	# loop if s0 != s3
