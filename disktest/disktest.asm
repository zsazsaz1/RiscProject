add $s0, $zero, $zero, 0	# set $s0 = 0, signifies we need to halt at the end
add $t0, $zero, $imm, 6		# $t0 = 6
out $imm, $t0, $zero, DiskInterrupt	# set irqhandler as DiskInterrupt
add $t0, $zero, $imm, 1		# $t0 = 1
out $t0, $zero, $imm, 1		# enable irq1
add $s2, $zero, $imm, Nop	# set $s2 = Nop
add $a0, $zero, $imm, -1	# $a0 = -1 (sector for disksector from now on)
add $a1, $zero, $imm, -128	# $a1 = -128 (address for diskbuffer from now on)
jal $imm, $zero, $zero, Check	# jump to Check
halt $zero, $zero, $zero, 0	# halt

Nop:
	add $t0, $zero, $imm, 17	# $t0 = 17
	in $t1, $zero, $t0, 0		# $t1 = diskstatus
	beq $imm, $zero, $t1, Check # if $t1 = diskstatus = 0 jump to Check
	beq $s2, $zero, $zero, 0	# jump to $s2 (= Nop)

DiskInterrupt:
	out $zero, $zero, $imm, 4	# clear irq1 status
	beq $ra, $s0, $imm, 1  		# if $s0 = 1 jump to halt
	reti $zero, $zero, $zero, 0	# return from interrupt

Check:
	add $t0, $zero, $imm, 4			# $t0 = 4 
	blt $imm, $a0, $t0, ReadSector	# if $a0 < 4 jump to ReadSector
	beq $imm, $a0, $t0, WriteXOR 	# if $a0 = 4 jump to WriteXOR
	

ReadSector:
	add $a0, $a0, $imm, 1	    # $a0++
	add $a1, $a1, $imm, 128		# $a1 += 128
	add $t0, $zero, $imm, 15	# $t0 = 15
    add $t1, $zero, $imm, 16	# $t1 = 16
    out $a0, $zero, $t0, 0		# set disksector = $a0
    out $a1, $zero, $t1, 0		# set diskbuffer = $a1

	add $t0, $zero, $imm, 4		# $t0 = 4
	beq $imm, $a0, $t0, Nop		# if $a0 = 4 jump to Nop
	add $t0, $zero, $imm, 14	# $t0 = 14
	out $imm, $zero, $t0, 1		# set diskcmd = 1, read from sector
	beq $imm, $zero, $zero, Nop	# jump to Nop

WriteXOR:
	add $t0, $zero, $zero, 0	# $t0 = 0, signifies disksector 
	add $t1, $zero, $zero, 0	# $t1 = 0, signifies current word 

Loop:
	lw $t2, $t0, $t1, 0			# $t2 = Ram[$t0+$t1]
	xor $v0, $v0, $t2, 0		# $v0 = $v0 ^ $t2
	add $t0, $t0, $imm, 128		# $t0 += 128 

	add $t2, $zero, $imm, 512	# $t2 = 512
	bne $imm, $t2, $t0, Loop	# if $t0 != 512 jump back to Loop

	sw $v0, $t0, $t1, 0			# Ram[$t0+$t1] = $v0
	add $t0, $zero, $zero, 0	# resets $t0 = 0
	add $t1, $t1, $imm, 1		# $t1 += 1 
	add $v0, $zero, $zero, 0	# resets $v0 = 0
	add $t2, $zero, $imm, 128	# $t2 = 128
	blt $imm, $t1, $t2, Loop	# if $t1 < 128 jump back to Loop

	add $a0, $zero, $imm, 4	    # $a0 = 4
	add $a1, $zero, $imm, 512 	# $a1 = 512
	add $t0, $zero, $imm, 15	# $t0 = 15
    add $t1, $zero, $imm, 16	# $t1 = 16
    out $a0, $zero, $t0, 0		# set disksector = $a0 = 4
    out $a1, $zero, $t1, 0		# set diskbuffer = $a1 = 512
	add $t0, $zero, $imm, 14	# $t0 = 14
	out $imm, $zero, $t0, 2 	# set diskcmd = 2, write to sector 4 
	
	add $s0, $zero, $imm, 1		# set $s0 = 1
	beq $imm, $zero, $zero, Nop # jump to Nop
