	add $sp, $zero, $imm, 1	           	 # set $sp = 1
	sll $sp, $sp, $imm, 11		         # set $sp = 1 << 11 = 2048

	lw $a0, $zero, $imm, 0x100           # get R from address 0x100 that we initialized 
	add $t0, $zero, $imm, 255	         # set $t0 = 255
	out $t0, $zero, $imm, 21	         # write to monitordata 255 (white)

	jal $imm, $zero, $zero, circle	     # start circle 
	halt $zero, $zero, $zero, 0  	     # halt
	
circle: 
	add $sp, $sp, $imm, -5		         # adjust stack for 2 items
	sw $ra, $sp, $imm, 4		         # save $ra
	sw $a0, $sp, $imm, 3		         # save $a0
	sw $s0, $sp, $imm, 2		         # save $s0
	sw $s1, $sp, $imm, 1		         # save $s1
	sw $s2, $sp, $imm, 0		         # save $s2

	add $s0, $a0, $zero, 0               # $s0 = R 

	add $s1, $imm, $s0, -1               # $s1 = i = R
	add $s1, $s1, $imm, 1                # $s1 = R+1

	add $t2, $zero, $zero, 0             # $t2 = y = 0  

LOOP1: 
	add $s1, $s1, $imm, -1               # i-- (start from R)
	blt $imm, $s1, $zero, done           # for(i=R-1;i>0;i--)

	add $s2, $imm, $zero, -1             # $s2 = j = -1

	mul $t0, $s0, $s0, 0                 # $t0 = R^2
	mul $t1, $s1, $s1, 0                 # $t1 = i^2
	sub $t0, $t0, $t1, 0                 # $t0 = R^2-i^2
	                                  
LOOP:	
	mul $t1, $t2, $t2, 0                 # $t1 = result = y^2
	bgt $imm, $t1, $t0, sqrt             # if (y^2 > R^2-i^2) return y-1
	add $t2, $imm, $t2, 1                # y++
	beq $imm, $zero, $zero, LOOP         # continue the loop 
sqrt:
	add $t2, $t2, $imm, -1               # $t2 = y-1 = sqrt(R^2-i^2)

LOOP2:
	add $s2, $s2, $imm, 1                # j++ (start from 0)
	bge $imm, $s2, $t2, LOOP1            # for(j=0;j<y;j++)  --- we only need to calculate each round 1/4 circle and the rest is symmetrical 
	
	add $t0, $s1, $imm, 175	             # $t0 = i + 175
	out $t0, $zero, $imm, 19	         # write to monitorx i + 175
	add $t0, $s2, $imm, 143	             # $t0 = j + 143
	out $t0, $zero, $imm, 20	         # write to monitory j + 143
	add $t0, $zero, $imm, 1		         # set $t0 = 1
	out $t0, $zero, $imm, 18	         # write to monitorcmd

	mul $t0, $s1, $imm, -1               # $t0 = -i
	add $t0, $t0, $imm, 175	             # $t0 = -i + 175
	out $t0, $zero, $imm, 19	         # write to monitorx -i + 175
	add $t0, $zero, $imm, 1		         # set $t0 = 1
	out $t0, $zero, $imm, 18	         # write to monitorcmd

	mul $t0, $s2, $imm, -1               # $t0 = -j
	add $t0, $t0, $imm, 143	             # $t0 = -j + 143
	out $t0, $zero, $imm, 20	         # write to monitory -j + 143
	add $t0, $zero, $imm, 1		         # set $t0 = 1
	out $t0, $zero, $imm, 18	         # write to monitorcmd

	add $t0, $s1, $imm, 175	             # $t0 = i + 175
	out $t0, $zero, $imm, 19	         # write to monitorx i + 175
	add $t0, $zero, $imm, 1		         # set $t0 = 1
	out $t0, $zero, $imm, 18	         # write to monitorcmd

	beq $imm, $zero, $zero, LOOP2        # continue the loop 

done:
	lw $s2, $sp, $imm, 0		         # restore $s2
	lw $s1, $sp, $imm, 1		         # restore $s1
	lw $s0, $sp, $imm, 2		         # restore $s0
	lw $a0, $sp, $imm, 3		         # restore $a0
	lw $ra, $sp, $imm, 4		         # restore $ra
	add $sp, $sp, $imm, 5		         # pop 5 items from stack
	beq $ra, $zero, $zero, 0	         # return


.word 0x100 8                            # radius for the circle 

	
