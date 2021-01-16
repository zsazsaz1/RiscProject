#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include "opcodes.h"
#include "singltons.h"
#include "IO.h"

/*
in case one of the registers is expected to be an immediate, this functions gets the imm inside the register, positive or negative
*/
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

/*
input arguments: 
The 3 registers used in the current operation respectively.
The function is used at the start of some opcodes to check if an immediate is used and if so, calling getImmediate(), and that nothing is being writen to register imm
*/
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

/*
input arguments:
The 3 registers used in the current operation respectively.
The function is used at the end of some opcodes to check if something was written on register zero, and if so, changing it back to zero, and increases PC and cycle by one
*/
void assignOperationStop(int rd, int rs, int rt)
{
	if (0 == rd)
	{
		Registers[0] = 0;
	}
	PCAndCycleIncrement();
}

/*
input arguments:
The 3 registers used in the current operation respectively.
The function is used at the start of some opcodes to check if an immediate is used and if so, calling getImmediate()
*/
void nonAssignStart(int rd, int rs, int rt)
{
	if (1 == rd || 1 == rs || 1 == rt)
	{	
		getImmediate();
	}
}

/*
input arguments:
A register used in the current operation (always the rd register)
The function is used in operation that uses jump, it changes the program counter to the new value inside the respective register (only the first 10 bits), 
after that it only increases cycle by one
*/
void jumpToReg(int reg)
{
	PC = Registers[reg] & 0x3FF;
	CycleIncreament();
}

/*
input arguments:
The 3 registers used in the current operation respectively.
Adds registers values.
The operation adds the values in registers rs and rt, and stores it in register rd
*/
void add (int rd, int rs, int rt)
{
	assignOperationStart(rd, rs, rt);
	Registers[rd] = Registers[rs] + Registers[rt];
	assignOperationStop(rd, rs, rt);
}

/*
input arguments:
The 3 registers used in the current operation respectively.
Substracts registers values.
The operation subtracts the value in register rs from the one in rt, and stores it in register rd
*/
void sub (int rd, int rs, int rt)
{
	assignOperationStart(rd, rs, rt);
	Registers[rd] = Registers[rs] - Registers[rt];
	assignOperationStop(rd, rs, rt);
}

/*
input arguments:
The 3 registers used in the current operation respectively.
The operation uses the Logical operation "and" between the values in registers rs and rt, and stores it in register rd
*/
void and (int rd, int rs, int rt)
{
	assignOperationStart(rd, rs, rt);
	Registers[rd] = Registers[rs] & Registers[rt];
	assignOperationStop(rd, rs, rt);
}

/*
input arguments:
The 3 registers used in the current operation respectively.
The operation uses the Logical operation "or" between the values in registers rs and rt, and stores it in register rd
*/
void or (int rd, int rs, int rt)
{

	assignOperationStart(rd, rs, rt);
	Registers[rd] = Registers[rs] | Registers[rt];
	assignOperationStop(rd, rs, rt);
}

/*
input arguments:
The 3 registers used in the current operation respectively.
The operation uses the Logical operation "xor" between the values in registers rs and rt, and stores it in register rd
*/
void xor (int rd, int rs, int rt)
{

	assignOperationStart(rd, rs, rt);
	Registers[rd] = Registers[rs] ^ Registers[rt];
	assignOperationStop(rd, rs, rt);
}

/*
input arguments:
The 3 registers used in the current operation respectively.
multiplies registers values.
The operation multiplies the values in register rs and rt, and stores it in register rd
*/
void mul (int rd, int rs, int rt)
{

	assignOperationStart(rd, rs, rt);
	Registers[rd] = Registers[rs] * Registers[rt];
	assignOperationStop(rd, rs, rt);
}

/*
input arguments:
The 3 registers used in the current operation respectively.
The operation uses the bitwise operation "Logical left shift" on the value in registers rs by amount as the value in register rt, and stores it in register rd
*/
void sll (int rd, int rs, int rt)
{
	assignOperationStart(rd, rs, rt);
	Registers[rd] = (unsigned)Registers[rs] << Registers[rt];
	assignOperationStop(rd, rs, rt);
}

/*
input arguments:
The 3 registers used in the current operation respectively.
The operation uses the bitwise operation "Arithmetic right shift" (sometime termed as signed shift, but doesn't restrict signed operands) 
on the value in registers rs by amount as the value in register rt, and stores it in register rd
*/
void sra (int rd, int rs, int rt)
{
	assignOperationStart(rd, rs, rt);
	Registers[rd] = Registers[rs] >> Registers[rt];
	assignOperationStop(rd, rs, rt);
}

/*
input arguments:
The 3 registers used in the current operation respectively.
The operation uses the bitwise operation "Logical left shift" on the value in registers rs by amount as the value in register rt, and stores it in register rd
*/
void srl (int rd, int rs, int rt)
{
	assignOperationStart(rd, rs, rt);
	Registers[rd] = (unsigned)Registers[rs] >> Registers[rt];
	assignOperationStop(rd, rs, rt);
}

/*
input arguments:
The 3 registers used in the current operation respectively.
if the values in registers rs and rt are equal, move PC to the value in register rd, else, increases PC and cycle by one 
*/
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

/*
input arguments:
The 3 registers used in the current operation respectively.
if the values in registers rs and rt are not equal, move PC to the value in register rd, else, increases PC and cycle by one 
*/
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

/*
input arguments:
The 3 registers used in the current operation respectively.
if the values in registers rs is lower than the one in rt, move PC to the value in register rd, else, increases PC and cycle by one
*/
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

/*
input arguments:
The 3 registers used in the current operation respectively.
if the values in registers rs is bigger than the one in rt, move PC to the value in register rd, else, increases PC and cycle by one
*/
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

/*
input arguments:
The 3 registers used in the current operation respectively.
if the values in registers rs is lower or equal than the one in rt, move PC to the value in register rd, else, increases PC and cycle by one 
*/
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

/*
input arguments:
The 3 registers used in the current operation respectively.
if the values in registers rs is bigger or equal than the one in rt, move PC to the value in register rd, else, increases PC and cycle by one
*/
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

/*
input arguments:
The 3 registers used in the current operation respectively.
if rd is the immediate register, call getImmediate() to get it,
after that, store what the next PC should be in register RA (return address) and move PC to the value in register rd 
*/
void jal (int rd, int rs, int rt)
{
	if (1 == rd)
	{
		getImmediate();
	}
	Registers[RA] = PC + 1;
	jumpToReg(rd); // maybe need to check if rd is reg 15, but we assume the programmer knows what he's doing
	
}

/*
input arguments:
The 3 registers used in the current operation respectively.
Loads a word from the Ram to a register.
Loads a word from the Ram on the index of the sum of values in registers rs and rt and stores it in register rd
*/
void lw (int rd, int rs, int rt)
{
	assignOperationStart(rd, rs, rt);
	Registers[rd] = Ram[Registers[rs] + Registers[rt]];
	assignOperationStop(rd, rs, rt);
}

/*
input arguments:
The 3 registers used in the current operation respectively.
Saves a word from a register to the Ram.
Saves the word in register rd to the Ram on the index of the sum of values in registers rs and rt
*/
void sw (int rd, int rs, int rt)
{
	nonAssignStart(rd, rs, rt);
	Ram[Registers[rs] + Registers[rt]] = Registers[rd];
	PCAndCycleIncrement();
}

/*
input arguments:
The 3 registers used in the current operation respectively.
The operation used at the end of interruption.
It changes PC to the first 10 bits of irqreturn, resets Interrupted back to 0 and increases cycle by one
*/
void reti (int rd, int rs, int rt)
{
	PC = irqreturn & 0x3FF; //10 bits
	Interupted = 0;
	CycleIncreament();
}

/*
input arguments:
The 3 registers used in the current operation respectively.
Gets a value from an IO register.
It stores in register rd the value in the IO register which is the sum of the values in registers rs and rt
*/
void in (int rd, int rs, int rt)
{
	assignOperationStart(rd, rs, rt);
	Registers[rd] = getIORegister(Registers[rs] + Registers[rt]);
	assignOperationStop(rd, rs, rt);
}

/*
input arguments:
The 3 registers used in the current operation respectively.
Sets a value to an IO register.
It sets the value in register rd in the IO register which is the sum of the values in registers rs and rt
*/
void out (int rd, int rs, int rt)
{
	nonAssignStart(rd, rs, rt);
	setIORegister(Registers[rs] + Registers[rt], Registers[rd]);
	PCAndCycleIncrement();
}

/*
input arguments:
The 3 registers used in the current operation respectively.
Used at the end of an assembly code to exit.
Increases the cycle and changes ShouldExit to 1 so the simulator will know to get out of code (the while loop in main) and start the ending operations
*/
void halt (int rd, int rs, int rt)
{
	CycleIncreament();
	ShouldExit = 1;
}

/*
maps the operations to their respective codes 
*/
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