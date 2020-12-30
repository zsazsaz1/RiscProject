	jal $imm, $zero, $zero, bubble         # call bubble (no input arguments) :0
	halt $zero, $zero, $zero, 0	       # halt :2

bubble:
	add $sp, $sp, $imm, -2                 # adjust stack for 2 items :3 
	sw $s0, $sp, $imm, 1                   # save $s0
	sw $s1, $sp, $imm, 0                   # save $s1

	add $s0, $zero, $imm, -1               # $s0 = i = -1
	add $t0, $zero, $imm, 16               # $t0 = N = 16 (length of the array)

LOOP1: 
	add $s0, $s0, $imm, 1                  # i++ (strat from 0)
	bge $s0, $t0, $imm, end                # if (i>=N) end this loop 
	add $s1, $zero, $zero, 0               # $s1 = j = 0

LOOP2:
	sub $t1, $t0, $s0, 0                   # $t1 = N-i
	add $t1, $t1, $imm, -1		             # $t1 = N-i-1
	bge $s1, $t1, $imm, LOOP1              # if (j>=N-i-1) end this loop 

	lw $t1, $s1, $imm, 1024                # $t1 = arr[j]
	add $s1, $s1, $imm ,1                  # j = j+1
	lw $t2, $s1, $imm, 1024                # $t2 = arr[j+1]

	blt $t1, $t2, $imm, swap               # if(arr[j]<arr[j+1]) swap
	beq $imm, $zero, $zero, LOOP2          # continue the loop

swap:
	sw $t1, $s1, $imm, 1024                # arr[j+1] = arr[j]
	add $s1, $s1, $imm, -1                 # j = j-1 (which is the original j)
	sw $t2, $s1, $imm, 1024                # arr[j] = arr[j+1]
	add $s1, $s1, $imm, 1                  # j = j+1
	beq $imm, $zero, $zero, LOOP2          # continue the loop  

end:
	lw $s1, $sp, $imm, 0		        # restore $s1
	lw $s0, $sp, $imm, 1	                # restore $s0
	add $sp, $sp, $imm, 2		        # pop 2 items from stack
	beq $ra, $zero, $zero, 0	        # return

.word 1024 1
.word 1025 2
.word 1026 3
.word 1027 4
.word 1028 5
.word 1029 6
.word 1030 7
.word 1031 8
.word 1032 9
.word 1033 10
.word 1034 11
.word 1035 12
.word 1036 13
.word 1037 14
.word 1038 15
.word 1039 16
