#include <stdio.h>
#include <inttypes.h>
#include "singltons.h"

int add (int rd, int rs, int rt)
{
	return 1;
}

int sub (int rd, int rs, int rt)
{
	return 1;
}

int and (int rd, int rs, int rt)
{
	return 1;
}

int or (int rd, int rs, int rt)
{
	return 1;
}

int xor (int rd, int rs, int rt)
{
	return 1;
}

int mul (int rd, int rs, int rt)
{
	return 1;
}

int sll (int rd, int rs, int rt)
{
	return 1;
}

int sra (int rd, int rs, int rt)
{
	return 1;
}

int srl (int rd, int rs, int rt)
{
	return 1;
}

int beq (int rd, int rs, int rt)
{
	return 1;
}

int bne (int rd, int rs, int rt)
{
	return 1;
}

int blt (int rd, int rs, int rt)
{
	return 1;
}

int bgt (int rd, int rs, int rt)
{
	return 1;
}

int ble (int rd, int rs, int rt)
{
	return 1;
}

int bge (int rd, int rs, int rt)
{
	return 1;
}

int jal (int rd, int rs, int rt)
{
	return 1;
}

int lw (int rd, int rs, int rt)
{
	return 1;
}

int sw (int rd, int rs, int rt)
{
	return 1;
}

int reti (int rd, int rs, int rt)
{
	return 1;
}

int in (int rd, int rs, int rt)
{
	return 1;
}

int out (int rd, int rs, int rt)
{
	return 1;
}

int halt (int rd, int rs, int rt)
{
	return 1;
}

int(*OpcodeMap[22])(int, int, int) = { 
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