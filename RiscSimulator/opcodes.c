#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include "opcodes.h"
#include "singltons.h"
#include "IO.h"

void getImmediate() 
{
	PCAndCycleIncrement();
	int isImmSigned = (InstructionRam[PC] & 0x80000) >> 19;
	if (isImmSigned)
	{
		Registers[IMM] = -((InstructionRam[PC] ^ 0xFFFFF) + 1); // the inverse plus one
	}
	else
	{
		Registers[IMM] = InstructionRam[PC];
	}
}

void assignOperationStart(int rd, int rs, int rt)
{
	if (1 == rd)
	{
		printf("rd in add cant be $imm");
		exit(1);
	}
	if (1 == rs || 1 == rt) {
		getImmediate();
	}
}

void assignOperationStop(int rd, int rs, int rt)
{
	if (0 == rd)
	{
		Registers[0] = 0;
	}
	PCAndCycleIncrement();
}

void nonAssignStart(int rd, int rs, int rt)
{
	if (1 == rd || 1 == rs || 1 == rt)
	{	
		getImmediate();
	}
}

void jumpToReg(int reg)
{
	PC = Registers[reg] & 0x3FF;
	CycleIncreament();
}

void add (int rd, int rs, int rt)
{
	assignOperationStart(rd, rs, rt);
	Registers[rd] = Registers[rs] + Registers[rt];
	assignOperationStop(rd, rs, rt);
}

void sub (int rd, int rs, int rt)
{
	assignOperationStart(rd, rs, rt);
	Registers[rd] = Registers[rs] - Registers[rt];
	assignOperationStop(rd, rs, rt);
}

void and (int rd, int rs, int rt)
{
	assignOperationStart(rd, rs, rt);
	Registers[rd] = Registers[rs] & Registers[rt];
	assignOperationStop(rd, rs, rt);
}

void or (int rd, int rs, int rt)
{

	assignOperationStart(rd, rs, rt);
	Registers[rd] = Registers[rs] | Registers[rt];
	assignOperationStop(rd, rs, rt);
}

void xor (int rd, int rs, int rt)
{

	assignOperationStart(rd, rs, rt);
	Registers[rd] = Registers[rs] ^ Registers[rt];
	assignOperationStop(rd, rs, rt);
}
void mul (int rd, int rs, int rt)
{

	assignOperationStart(rd, rs, rt);
	Registers[rd] = Registers[rs] * Registers[rt];
	assignOperationStop(rd, rs, rt);
}

void sll (int rd, int rs, int rt)
{
	assignOperationStart(rd, rs, rt);
	Registers[rd] = (unsigned)Registers[rs] << Registers[rt];
	assignOperationStop(rd, rs, rt);
}

void sra (int rd, int rs, int rt)
{
	assignOperationStart(rd, rs, rt);
	Registers[rd] = Registers[rs] >> Registers[rt];
	assignOperationStop(rd, rs, rt);
}

void srl (int rd, int rs, int rt)
{
	assignOperationStart(rd, rs, rt);
	Registers[rd] = (unsigned)Registers[rs] >> Registers[rt];
	assignOperationStop(rd, rs, rt);
}

void beq (int rd, int rs, int rt)
{
	nonAssignStart(rd, rs, rt);
	if (Registers[rs] == Registers[rt])
	{
		jumpToReg(rd);
	}
	else
	{
		PCAndCycleIncrement();
	}
}

void bne (int rd, int rs, int rt)
{
	nonAssignStart(rd, rs, rt);
	if (Registers[rs] != Registers[rt])
	{
		jumpToReg(rd);
	}
	else
	{
		PCAndCycleIncrement();
	}
}

void blt (int rd, int rs, int rt)
{
	nonAssignStart(rd, rs, rt);
	if (Registers[rs] < Registers[rt])
	{
		jumpToReg(rd);
	}
	else
	{
		PCAndCycleIncrement();
	}
}

void bgt (int rd, int rs, int rt)
{
	nonAssignStart(rd, rs, rt);
	if (Registers[rs] > Registers[rt])
	{
		jumpToReg(rd);
	}
	else
	{
		PCAndCycleIncrement();
	}
}

void ble (int rd, int rs, int rt)
{
	nonAssignStart(rd, rs, rt);
	if (Registers[rs] <= Registers[rt])
	{
		jumpToReg(rd);
	}
	else
	{
		PCAndCycleIncrement();
	}
}

void bge (int rd, int rs, int rt)
{
	nonAssignStart(rd, rs, rt);
	if (Registers[rs] >= Registers[rt])
	{
		jumpToReg(rd);
	}
	else
	{
		PCAndCycleIncrement();
	}
}

void jal (int rd, int rs, int rt)
{
	if (1 == rd)
	{
		getImmediate();
	}
	Registers[RA] = PC + 1;
	jumpToReg(rd); // maybe need to check if rd is reg 15, but we assume the programmer knows what he's doing
	
}

void lw (int rd, int rs, int rt)
{
	assignOperationStart(rd, rs, rt);
	Registers[rd] = Ram[Registers[rs] + Registers[rt]];
	assignOperationStop(rd, rs, rt);
}

void sw (int rd, int rs, int rt)
{
	nonAssignStart(rd, rs, rt);
	Ram[Registers[rs] + Registers[rt]] = Registers[rd];
	PCAndCycleIncrement();
}

void reti (int rd, int rs, int rt)
{
	PC = irqreturn & 0x3FF; //10 bits
	Interupted = 0;
	CycleIncreament();
}

void in (int rd, int rs, int rt)
{
	assignOperationStart(rd, rs, rt);
	Registers[rd] = getIORegister(Registers[rs] + Registers[rt]);
	assignOperationStop(rd, rs, rt);
}

void out (int rd, int rs, int rt)
{
	nonAssignStart(rd, rs, rt);
	setIORegister(Registers[rs] + Registers[rt], Registers[rd]);
	PCAndCycleIncrement();
}

void halt (int rd, int rs, int rt)
{
	CycleIncreament();
	ShouldExit = 1;
}

void(*OpcodeMap[22])(int, int, int) = { 
	add,
	sub,
	and,
	or,
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