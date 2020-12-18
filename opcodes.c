#include <stdio.h>
#include <inttypes.h>
#include "opcodes.h"
#include "singltons.h"

int32_t getImmediate() 
{
	PC++;
	int isImmSigned = (InstructionRam[PC] & 0x80000) >> 19;
	if (isImmSigned)
	{
		Registers[1] = -((InstructionRam[PC] ^ 0xFFFFF) + 1);
	}
	else
	{
		Registers[1] = InstructionRam[PC];
	}
}

void arithmeticOpCodeStart(int rd, int rs, int rt)
{
	if (1 == rd)
	{
		printf("rd in add cant be $imm");
		exit(1);
	}
	if (rs == 1 || rt == 1) {
		getImmediate();
	}
}

void arithmeticOpCodeStop(int rd, int rs, int rt)
{
	if (0 == rd)
	{
		Registers[0] = 0;
	}
	PC++;
}

void jumpOpCodeStart(int rd, int rs, int rt)
{
	if (1 == rd || 1 == rs || 1 == rt)
	{
		getImmediate();
	}
}

void jumpToReg(int reg)
{
	PC = Registers[reg] & 0x3FF;
}

void add (int rd, int rs, int rt)
{
	arithmeticOpCodeStart(rd, rs, rt);
	Registers[rd] = Registers[rs] + Registers[rt];
	arithmeticOpCodeStop(rd, rs, rt);
}

void sub (int rd, int rs, int rt)
{
	arithmeticOpCodeStart(rd, rs, rt);
	Registers[rd] = Registers[rs] - Registers[rt];
	arithmeticOpCodeStop(rd, rs, rt);
}

void and (int rd, int rs, int rt)
{
	arithmeticOpCodeStart(rd, rs, rt);
	Registers[rd] = Registers[rs] & Registers[rt];
	arithmeticOpCodeStop(rd, rs, rt);
}

void or (int rd, int rs, int rt)
{

	arithmeticOpCodeStart(rd, rs, rt);
	Registers[rd] = Registers[rs] | Registers[rt];
	arithmeticOpCodeStop(rd, rs, rt);
}

void xor (int rd, int rs, int rt)
{

	arithmeticOpCodeStart(rd, rs, rt);
	Registers[rd] = Registers[rs] ^ Registers[rt];
	arithmeticOpCodeStop(rd, rs, rt);
}
void mul (int rd, int rs, int rt)
{

	arithmeticOpCodeStart(rd, rs, rt);
	Registers[rd] = Registers[rs] * Registers[rt];
	arithmeticOpCodeStop(rd, rs, rt);
}

void sll (int rd, int rs, int rt)
{
	arithmeticOpCodeStart(rd, rs, rt);
	Registers[rd] = (unsigned)Registers[rs] << Registers[rt];
	arithmeticOpCodeStop(rd, rs, rt);
}

void sra (int rd, int rs, int rt)
{
	arithmeticOpCodeStart(rd, rs, rt);
	Registers[rd] = Registers[rs] >> Registers[rt];
	arithmeticOpCodeStop(rd, rs, rt);
}

void srl (int rd, int rs, int rt)
{
	arithmeticOpCodeStart(rd, rs, rt);
	Registers[rd] = (unsigned)Registers[rs] >> Registers[rt];
	arithmeticOpCodeStop(rd, rs, rt);
}

void beq (int rd, int rs, int rt)
{
	jumpOpCodeStart(rd, rs, rt);
	if (Registers[rs] == Registers[rt])
	{
		jumpToReg(rd);
	}
	else
	{
		PC++;
	}
}

void bne (int rd, int rs, int rt)
{
	jumpOpCodeStart(rd, rs, rt);
	if (Registers[rs] != Registers[rt])
	{
		jumpToReg(rd);
	}
	else
	{
		PC++;
	}
}

void blt (int rd, int rs, int rt)
{
	jumpOpCodeStart(rd, rs, rt);
	if (Registers[rs] < Registers[rt])
	{
		jumpToReg(rd);
	}
	else
	{
		PC++;
	}
}

void bgt (int rd, int rs, int rt)
{
	jumpOpCodeStart(rd, rs, rt);
	if (Registers[rs] > Registers[rt])
	{
		jumpToReg(rd);
	}
	else
	{
		PC++;
	}
}

void ble (int rd, int rs, int rt)
{
	jumpOpCodeStart(rd, rs, rt);
	if (Registers[rs] <= Registers[rt])
	{
		jumpToReg(rd);
	}
	else
	{
		PC++;
	}
}

void bge (int rd, int rs, int rt)
{
	jumpOpCodeStart(rd, rs, rt);
	if (Registers[rs] >= Registers[rt])
	{
		jumpToReg(rd);
	}
	else
	{
		PC++;
	}
}

void jal (int rd, int rs, int rt)
{
	if (1 == rd)
	{
		getImmediate();
	}
	Registers[15] = PC + 1;
	jumpToReg(rd); // maybe need to check if rd is reg 15
	
}

void lw (int rd, int rs, int rt)
{
	arithmeticOpCodeStart(rd, rs, rt);
	Registers[rd] = Ram[Registers[rs] + Registers[rt]];
	arithmeticOpCodeStop(rd, rs, rt);
}

void sw (int rd, int rs, int rt)
{
	jumpOpCodeStart(rd, rs, rt);
	Ram[Registers[rs] + Registers[rt]] = Registers[rd];
	PC++;
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
	exit(0);
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