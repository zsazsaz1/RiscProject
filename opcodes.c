#include <stdio.h>
#include <inttypes.h>
#include "opcodes.h"
#include "singltons.h"

void add (int rd, int rs, int rt)
{
	if (1 == rd)
	{
		printf("rd in add cant be $imm");
		exit(1);
	}
	if (rs == 1 || rt == 1) {
		Registers[1] = InstructionRam[PC + 1];
		PC++;
	}
	Registers[rd] = Registers[rs] + Registers[rt];

	PC++;
}

void sub (int rd, int rs, int rt)
{
	return 1;
}

void and (int rd, int rs, int rt)
{
	return 1;
}

void or (int rd, int rs, int rt)
{
	return 1;
}

void xor (int rd, int rs, int rt)
{
	return 1;
}

void mul (int rd, int rs, int rt)
{
	return 1;
}

void sll (int rd, int rs, int rt)
{
	return 1;
}

void sra (int rd, int rs, int rt)
{
	return 1;
}

void srl (int rd, int rs, int rt)
{
	return 1;
}

void beq (int rd, int rs, int rt)
{
	return 1;
}

void bne (int rd, int rs, int rt)
{
	return 1;
}

void blt (int rd, int rs, int rt)
{
	return 1;
}

void bgt (int rd, int rs, int rt)
{
	return 1;
}

void ble (int rd, int rs, int rt)
{
	return 1;
}

void bge (int rd, int rs, int rt)
{
	return 1;
}

void jal (int rd, int rs, int rt)
{
	return 1;
}

void lw (int rd, int rs, int rt)
{
	return 1;
}

void sw (int rd, int rs, int rt)
{
	return 1;
}

void reti (int rd, int rs, int rt)
{
	return 1;
}

void in (int rd, int rs, int rt)
{
	return 1;
}

void out (int rd, int rs, int rt)
{
	return 1;
}

void halt (int rd, int rs, int rt)
{
	return 1;
}

void(*OpcodeMap[22])(int, int, int) = { 
	add,
	sub,
	and,
	or ,
	xor,
	mul,
	sll,
	sra,
	srl,
	beq,
	bne,
	blt,
	bgt,
	ble,
	bge,
	jal,
	lw,
	sw,
	reti,
	in,
	out,
	halt
};