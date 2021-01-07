	add $sp, $zero, $imm, 1		# set $sp = 1 
	sll $sp, $sp, $imm, 11		# set $sp = 1 << 11 = 2048
	lw $a0, $zero, $imm, 0x100      # get n from address 0x100 that we initialized 
	lw $a1, $zero, $imm, 0x101      # get k from address 0x101 that we initialized 
	jal $imm, $zero, $zero, binom	# calc $v0 = binom(n,k) 
	sw $v0, $zero, $imm, 0x102	# store binom(n,k) in 0x102 
	halt $zero, $zero, $zero, 0  	# halt 

binom:	
	add $sp, $sp, $imm, -4		# adjust stack for 4 items 
	sw $ra, $sp, $imm, 3		# save return address 
	sw $a0, $sp, $imm, 2		# save argument 
	sw $a1, $sp, $imm, 1		# save argument 
	sw $s0, $sp, $imm, 0		# save varaible (result of binom(n-1,k-1)) 

	beq $imm, $a1, $zero, ret   # if (k==0) return 1 
	beq $imm, $a0, $a1, ret     # if (n==k) return 1 
	
	add $a0, $a0, $imm, -1      # $a0 = n-1 
	add $a1, $a1, $imm, -1      # $a1 = k-1 
	jal $imm, $zero, $zero, binom           # binom(n-1,k-1) 
	add $s0, $v0, $zero, 0      # $s0 = binom(n-1,k-1) 
	
	add $a1, $a1, $imm, 1       # $a1 = k                  
	jal $imm, $zero, $zero, binom           # binom(n-1,k) 
	add $v0, $s0, $v0, 0        # $v0 = binom(n-1,k-1) + binom(n-1,k) 
	beq $imm, $zero, $zero, exit	# return 
	
ret:    
	add $v0, $zero, $imm, 1     # $v0 = 1 

exit:   
	lw $s0, $sp, $imm, 0		# restore $s0
	lw $a1, $sp, $imm, 1		# restore $a1
	lw $a0, $sp, $imm, 2		# restore $a0
	lw $ra, $sp, $imm, 3        # restore $ra

	add $sp, $sp, $imm, 4       # pop 4 items from stack
	beq $ra, $zero, $zero, 0	# return
	
	.word 0x100 5               # arbitrary numbers for the binom func
	.word 0x101 2

