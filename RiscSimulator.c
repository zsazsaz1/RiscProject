#define _CRT_SECURE_NO_DEPRECATE

#include <stdio.h>
#include <inttypes.h>
#include "singltons.h"
#include "opcodes.h"
#include "IO.h"

#define IMEMIN 1
#define DMEMIN 2
#define DMEMOUT 3 // should be 6

int readFileAsHex(char filename[], int outArray[], int size)
{
	unsigned int memory;
	FILE* instructionFile = fopen(filename, "r");

	if (NULL == instructionFile) {
		printf("Error opening file!\n");

		return -1;
	}

	int i = 0;
	int scanned;
	while(i < size && fscanf(instructionFile, "%x", &memory) != EOF)
	{
		outArray[i] = memory;
		i++;
	}

	while (i < size) {
		outArray[i] = 0;
		i++;
	}

	fclose(instructionFile);

	return 0;
}

int writeFileAsHex(char filename[], int inArray[], int size)
{
	unsigned int memory;
	FILE* instructionFile = fopen(filename, "w");

	if (NULL == instructionFile) {
		printf("Error opening file!\n");

		return -1;
	}

	for (int i = 0; i < size; i++)
	{
		fprintf(instructionFile, "%08X\n", inArray[i]);
	}
}

int main(int argc, char* argv[])
{
	readFileAsHex(argv[IMEMIN], InstructionRam, INSTRUCTION_RAM_SIZE);
	readFileAsHex(argv[DMEMIN], Ram, RAM_SIZE);

	while (!ShouldExit) {
		if (irqEnable & irqStatus && !Interupted)
		{
			irqreturn = PC;
			PC = irqhandler;
			Interupted = 1;
		}
		int32_t currentInstruction = InstructionRam[PC];
		int8_t currentOpCode = (currentInstruction & 0xFF000) >> 12;
		int8_t rd = (currentInstruction & 0xF00) >> 8;
		int8_t rs = (currentInstruction & 0xF0) >> 4;
		int8_t rt = (currentInstruction & 0xF);
		OpcodeMap[currentOpCode](rd, rs, rt);
	}

	writeFileAsHex(argv[DMEMOUT], Ram, RAM_SIZE);

	return 0;
}