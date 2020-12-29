#define _CRT_SECURE_NO_DEPRECATE

#include <stdio.h>
#include <inttypes.h>
#include "opcodes.h"
#include "singltons.h"
#include "IO.h"

#define IMM 1
#define RA 15

unsigned int irq2stopCycles;
char resetIrq2Status = 0;
FILE* irq2in;

void getNextIrq2StopCycle()
{
	char error = fscanf(irq2in, "%d", &irq2stopCycles);

	if (EOF == error)
	{
		rewind(irq2in);
	}
}

char timerIntterupt = 0;
char irq2Intterupt = 0;

void CycleIncreament()
{
	Cycle++;

	if (timerIntterupt)
	{
		irqStatus |= 0b001; // irq0status == 1
		timerIntterupt = 0;
	}
	if (irq2Intterupt)
	{
		irqStatus |= 0b100; // irq2status == 1
		irq2Intterupt = 0;
	}

	if (timerenable)
	{
		if (timercurrent == timermax)
		{
			timerIntterupt = 1;
			timercurrent = 0;
		}
		else
		{
			timercurrent++;
		}
	}

	if (Cycle == irq2stopCycles)
	{
		irq2Intterupt = 1;
		getNextIrq2StopCycle();
	}

}

void PCAndCycleIncrement()
{
	PC++;
	CycleIncreament();
}

int32_t getImmediate() 
{
	PCAndCycleIncrement();
	int isImmSigned = (InstructionRam[PC] & 0x80000) >> 19;
	if (isImmSigned)
	{
		Registers[IMM] = -((InstructionRam[PC] ^ 0xFFFFF) + 1);
	}
	else
	{
		Registers[IMM] = InstructionRam[PC];
	}
}

void arithmeticOpCodeStart(int rd, int rs, int rt)
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

void arithmeticOpCodeStop(int rd, int rs, int rt)
{
	if (0 == rd)
	{
		Registers[0] = 0;
	}
	PCAndCycleIncrement();
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
	CycleIncreament();
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
		PCAndCycleIncrement();
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
		PCAndCycleIncrement();
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
		PCAndCycleIncrement();
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
		PCAndCycleIncrement();
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
		PCAndCycleIncrement();
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
	PCAndCycleIncrement();
}

void reti (int rd, int rs, int rt)
{
	PC = getIORegister(7);
	Interupted = 0;
	CycleIncreament();
}

void in (int rd, int rs, int rt)
{
	arithmeticOpCodeStart(rd, rs, rt);
	Registers[rd] = getIORegister(Registers[rs] + Registers[rt]);
	arithmeticOpCodeStop(rd, rs, rt);
}

void out (int rd, int rs, int rt)
{
	jumpOpCodeStart(rd, rs, rt);
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