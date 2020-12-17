#define _CRT_SECURE_NO_DEPRECATE

#include <stdio.h>
#include <inttypes.h>
#include "singltons.h";
#include "opcodes.h";


int readFileAsHex(char filename[], int out_array[], int size)
{
	int instruction;
	FILE* instructionFile = fopen(filename, "r");

	if (NULL == instructionFile) {
		printf("Error! opening file\n");

		return -1;
	}

	int i = 0;
	int scanned;
	do {

		scanned = fscanf(instructionFile, "%X", &instruction);
		if (scanned == EOF) {
			continue;
		}
		out_array[i] = instruction;

		i++;
	} while (scanned != EOF);

	while (i < INSTRUCTION_RAM_SIZE) {
		out_array[i] = 0;
		i++;
	}

	fclose(instructionFile);

	return 0;
}

int main(int argc, char* argv[])
{
	int a = readFileAsHex(argv[1], InstructionRam, INSTRUCTION_RAM_SIZE);
	int b = readFileAsHex(argv[2], Ram, RAM_SIZE);

	while (1) {
		int32_t currentInstruction = InstructionRam[PC];
		int8_t currentOpCode = (currentInstruction & 0xFF000) >> 12;


	}

	return 0;
}