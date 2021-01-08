	add $t0, $zero, $imm, 6		# set $t0 = 6
	out $imm, $t0, $zero, LedInterrupt	# set irqhandler as LedInterrupt
	out $imm, $zero, $zero, 1 	# enable irq0
	add $t0, $zero, $imm, 13	# set $t0 = 13
	out $imm, $t0, $zero, 1023 	# set timermax = 1023 (because it includes zero)
	add $t0, $zero, $imm, 11	# set $t0 = 12
	add $s0, $zero, $imm, Nop	# set $s0 = Nop, makes cycles more reliable not using $imm (seeing 1024 clear cycles between each Interrupt on trace file)
	out $imm, $t0, $zero, 1 	# set timerenable = 1024
	jal $imm, $zero, $zero, Nop # jump to Nop and save next addres in $ra
	halt $zero, $zero, $zero, 0	# halt

Nop:
	add $zero, $zero, $zero, 0  # litteraly nothing
	beq $s0, $zero, $zero, 0	# jump to $s0 (= Nop)

LedInterrupt:
	add $t0, $zero, $imm, 9 	# set $t0 = 9
	in $t1, $t0, $zero, 0   	# set $t1 = leds
	sll $t1, $t1, $imm, 1       # shift leds by 1
	add $t1, $t1, $imm, 1       # add to 1 leds
	out $t1, $t0, $zero, 0		# set leds = $t1
	beq $ra, $t1, $imm, -1		# go to halt if all 32 bits are 1
	out $zero, $zero, $imm, 3	# clear irq0 status
	reti $zero, $zero, $zero, 0	# return from interrupt

