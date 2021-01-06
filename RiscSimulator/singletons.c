#define _CRT_SECURE_NO_DEPRECATE

#include <stdio.h>
#include "singltons.h"
#include "IO.h"

int32_t Registers[REGISTER_COUNT] = { 0 };
int16_t PC = 0; // need to be 10 bits
int32_t Cycle = 0; // need to be 10 bits
int32_t InstructionRam[INSTRUCTION_RAM_SIZE]; // need to be 20 bits
int32_t Ram[RAM_SIZE];
int32_t Disk[DISK_SIZE];
int8_t Monitor[352][288];
char Interupted = 0;
char ShouldExit = 0;


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

char irq2Intterupt = 0;

void CycleIncreament()
{
	Cycle++;

	if (irq2Intterupt)
	{
		irqStatus |= 0b100; // irq2status == 1
		irq2Intterupt = 0;
	}

	if (timerenable)
	{
		if (timercurrent == timermax)
		{
			irqStatus |= 0b001; // irq0status == 1
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

	if (Cycle == (currentDiskTaskCycle + DISK_IO_CYCLES))
	{
		if (1 == diskcmd)
		{
			readFromDisk();
		}
		else if (2 == diskcmd)
		{
			writeToDisk();
		}
		diskcmd = 0;
		diskstatus = 0;
		irqStatus |= 0b010; // irq1status == 1
	}
}

void PCAndCycleIncrement()
{
	PC++;
	CycleIncreament();
}
