	lw $a0, $zero, $imm, 0x100      # get n from address 0x100 that we initialized :0
	lw $a1, $zero, $imm, 0x101      # get k from address 0x101 that we initialized :2
	jal $imm, $zero, $zero, binom	# calc $v0 = binom(n,k) :4
	sw $v0, $zero, $imm, 0x102	    # store binom(n,k) in 0x102 :6
	halt $zero, $zero, $zero, 0  	# halt :8

binom:	
	add $sp, $zero, $imm, 1		# set $sp = 1 :9
	sll $sp, $sp, $imm, 11		# set $sp = 1 << 11 = 2048 :11
	add $sp, $sp, $imm, -4		# adjust stack for 4 items :13
	sw $ra, $sp, $imm, 3		# save return address :15
	sw $a0, $sp, $imm, 2		# save argument :17
	sw $a1, $sp, $imm, 1		# save argument :19
	sw $s0, $sp, $imm, 0		# save varaible (result of binom(n-1,k-1)) :21
	beq $imm, $a1, $zero, ret   # if (k==0) return 1 :23
	beq $imm, $a0, $a1, ret     # if (n==k) return 1 :25
	
	add $a0, $a0, $imm, -1      # $a0 = n-1 :27
	add $a1, $a1, $imm, -1      # $a1 = k-1 :29
	jal $imm, $zero, $zero, binom           # binom(n-1,k-1) :31
	add $s0, $v0, $zero, 0      # $s0 = binom(n-1,k-1) :32
	
	add $a1, $a1, $imm, 1       # $a1 = k :34
	jal $imm, $zero, $zero, binom           # binom(n-1,k) :36
	add $v0, $s0, $v0, 0        # $v0 = binom(n-1,k-1) + binom(n-1,k) :37
	beq $imm, $zero, $zero, exit	# return :39
	
ret:    
	add $v0, $zero, $imm, 1     # $v0 = 1 :41

exit:   
	lw $s0, $sp, $imm, 0		# restore $s0
	lw $a1, $sp, $imm, 1		# restore $a1
	lw $a0, $sp, $imm, 2		# restore $a0
	lw $ra, $sp, $imm, 3        # restore $ra

	add $sp, $sp, $imm, 16      # pop 4 items from stack
	beq $ra, $zero, $zero, 0	# return
	
	.word 0x100 5               # arbitrary numbers for the binom func
	.word 0x101 2

