#define _CRT_SECURE_NO_DEPRECATE

#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include "singltons.h"
#include "opcodes.h"
#include "IO.h"
 
#define IMEMIN 1
#define DMEMIN 2
#define DISKIN 3
#define IRQ2IN 4
#define DMEMOUT 5
#define REGOUT 6
#define TRACE 7 

void asssertFileOpen(FILE* file, char filename[]) {
	if (NULL == file) {
		printf("Error opening file %s!\n", filename);
		exit(1);
	}
}

void readFileAsHex(char filename[], int outArray[], int size)
{
	unsigned int memory;
	FILE* file = fopen(filename, "r");

	asssertFileOpen(file, filename);

	int i = 0;
	while(i < size && fscanf(file, "%x", &memory) != EOF)
	{
		outArray[i] = memory;
		i++;
	}

	while (i < size) {
		outArray[i] = 0;
		i++;
	}

	fclose(file);
}

void writeFileAsHex(char filename[], int inArray[], int size)
{
	FILE* file = fopen(filename, "w");

	asssertFileOpen(file, filename);

	for (int i = 0; i < size; i++)
	{
		fprintf(file, "%08X\n", inArray[i]);
	}
}

void writeTraceToFile(char filename[], int16_t lastPC, int32_t registers[], int32_t instruction)
{
	FILE* file = fopen(filename, "a");

	asssertFileOpen(file, filename);

	fprintf(file, "%03X %05X", lastPC, instruction);
	for (int i = 0; i < REGISTER_COUNT; i++)
	{
		fprintf(file, " %08X", registers[i]);
	}

	fprintf(file, "\n");
	fclose(file);
}

int32_t RegistersCopy[REGISTER_COUNT];

void CopyRegisters()
{
	for (int i = 0; i < REGISTER_COUNT; i++)
	{
		RegistersCopy[i] = Registers[i];
	}
}

int main(int argc, char* argv[])
{
	readFileAsHex(argv[IMEMIN], InstructionRam, INSTRUCTION_RAM_SIZE);
	readFileAsHex(argv[DMEMIN], Ram, RAM_SIZE);
	readFileAsHex(argv[DISKIN], Disk, DISK_SIZE);
	
	irq2in = fopen(argv[IRQ2IN], "r");
	asssertFileOpen(irq2in, argv[IRQ2IN]);
	getNextIrq2StopCycle();

	while (!ShouldExit) {
		if (irqEnable & irqStatus && !Interupted)
		{
			if (irqStatus & 0b100) // irq2status == 1
			{
				irqStatus &= 0b011; // irq2status = 0
			}
			irqreturn = PC;
			PC = irqhandler;
			Interupted = 1;
		}
		int16_t lastPC = PC;
		CopyRegisters();
		int32_t currentInstruction = InstructionRam[PC];
		int8_t currentOpCode = (currentInstruction & 0xFF000) >> 12;
		int8_t rd = (currentInstruction & 0xF00) >> 8;
		int8_t rs = (currentInstruction & 0xF0) >> 4;
		int8_t rt = (currentInstruction & 0xF);
		OpcodeMap[currentOpCode](rd, rs, rt);
		RegistersCopy[1] = Registers[1];
		writeTraceToFile(argv[TRACE], lastPC, RegistersCopy, currentInstruction);
	}

	writeFileAsHex(argv[DMEMOUT], Ram, RAM_SIZE);
	writeFileAsHex(argv[REGOUT], Registers, REGISTER_COUNT);
	fclose(irq2in);

	return 0;
}