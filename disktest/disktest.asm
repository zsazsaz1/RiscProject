add $t3, $zero, $zero, 0	# set $t3 = 0, signifies we need to halt at the end
add $t0, $zero, $imm, 6		# set $t0 = 6
out $imm, $t0, $zero, DiskInterrupt	# set irqhandler as DiskInterrupt
add $t0, $zero, $imm, 1		# set $t0 = 1
out $t0, $zero, $imm, 1		# enable irq1
add $s2, $zero, $imm, Nop	# set $s2 = Nop
add $a0, $zero, $imm, -1	# set $a0 = -1 (sector for disksector from now on)
add $a1, $zero, $imm, -128	# set $a1 = -128 (address for diskbuffer from now on)
jal $imm, $zero, $zero, Check	# jump to Check
halt $zero, $zero, $zero, 0	# halt

Nop:
	beq $imm, $zero, $t2, Check 	# if $t2 = $t0 = 0 jump to Check
	beq $s2, $zero, $zero, 0	# jump to $s2 (= Nop)

DiskInterrupt:
	add $t2, $zero, $zero, 0 	# $t2 = 0 means for us the disk finished its work
	out $zero, $zero, $imm, 4	# clear irq1 status
	beq $ra, $t3, $imm, 1  		# if $t3 = 1 jump to halt
	reti $zero, $zero, $zero, 0	# return from interrupt

Check:
	add $t0, $zero, $imm, 4			# set $t0 = 4 
	blt $imm, $a0, $t0, ReadSector	# if $a0 < 4 jump to ReadSector
	beq $imm, $a0, $t0, WriteXOR 	# if $a0 = 4 jump to WriteXOR
	

ReadSector:
	add $a0, $a0, $imm, 1	    # $a0++
	add $a1, $a1, $imm, 128		# $a1 += 128
	add $t0, $zero, $imm, 15	# set $t0 = 15
    add $t1, $zero, $imm, 16	# set $t1 = 16
    out $a0, $zero, $t0, 0		# set disksector = $a0
    out $a1, $zero, $t1, 0		# set diskbuffer = $a1

	add $t0, $zero, $imm, 4		# set $t0 = 4
	beq $imm, $a0, $t0, Nop		# if $a0 = 4 jump to Nop
	add $t0, $zero, $imm, 14	# set $t0 = 14
	out $imm, $zero, $t0, 1		# set diskcmd = 1, read from sector
	add $t2, $zero, $imm, 1		# set $t2 = 1, disk is busy
	beq $imm, $zero, $zero, Nop	# jump to Nop

WriteXOR:
	add $s0, $zero, $zero, 0	# set $s0 = 0, signifies disksector for us 
	add $s1, $zero, $zero, 0	# set $s1 = 0, signifies current word for us

Loop:
	lw $t0, $s0, $s1, 0			# $t0 = Ram[$s0+$s1]
	xor $v0, $v0, $t0, 0		# $v0 = $v0 ^ $t0
	add $s0, $s0, $imm, 128		# set $s0 += 128 

	add $t0, $zero, $imm, 512	# set $t0 = 512
	bne $imm, $t0, $s0, Loop	# if $s0 != 512 jump back to Loop

	sw $v0, $s0, $s1, 0			# Ram[$s0+$s1] = $v0
	add $s0, $zero, $zero, 0	# resets $s0 = 0
	add $s1, $s1, $imm, 1		# set $s1 += 1 
	add $t0, $zero, $imm, 128	# set $t0 = 128
	blt $imm, $s1, $t0, Loop	# if $s1 < 128 jump back to Loop

	add $a0, $zero, $imm, 4	    # $a0 = 4
	add $a1, $zero, $imm, 512 	# $a1 = 512
	add $t0, $zero, $imm, 15	# set $t0 = 15
    add $t1, $zero, $imm, 16	# set $t1 = 16
    out $a0, $zero, $t0, 0		# set disksector = $a0 = 4
    out $a1, $zero, $t1, 0		# set diskbuffer = $a1 = 512
	add $t0, $zero, $imm, 14	# set $t0 = 14
	out $imm, $zero, $t0, 2 	# set diskcmd = 2, write to sector 4 
	add $t2, $zero, $imm, 1		# set $t2 = 1, disk is busy
	
	add $t3, $zero, $imm, 1		# set $t3 = 1
	beq $imm, $zero, $zero, Nop # jump to Nop
