#define _CRT_SECURE_NO_DEPRECATE

#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include "singltons.h"
#include "opcodes.h"
#include "IO.h"

// maps the arguments to their respective number, in the requested order for the cmd
enum FilenameArgs {
	IMEMIN = 1,
	DMEMIN,
	DISKIN,
	IRQ2IN,
	DMEMOUT,
	REGOUT,
	TRACE,
	HWREGTRACE,
	CYCLES,
	LEDS,
	MONITOR,
	MONITOR_YUV,
	DISKOUT
};

int32_t RegistersCopy[REGISTER_COUNT];

void asssertFileOpen(FILE* file, char filename[]);
void readFileAsHex(char filename[], int outArray[], int size);
void writeFileAsHex(char filename[], int inArray[], int size);
void writeToCyclesFileAsDec(char filename[], int32_t numOfAssemblyOp);
void writeTraceToFile(int16_t lastPC, int32_t registers[], int32_t instruction);
void CopyRegisters();

FILE* traceFile;

/*
input arguments:
argv and argc are how command line arguments are passed to main() in C.
argv is an array of strings, which is the arguments recieved in the command line.
argc (argument count) will be the number of strings pointed to by argv (argument vector). 
This will (in practice) be 1 plus the number of arguments, as virtually all implementations will prepend the name of the program to the array (the exe).

the main executes the code it-self.
At the start it opens the file that should be opened, and initialize what needs to be.
After the setup, it enters the while llop which is the assemly code itself (that we get from the instruction from the assembler) while writing to trace,
until the we get an halt operation, or we get to the end of the instructions.
After that we wrap up the simulator by writing to the out files and closing any opened files.
*/
int main(int argc, char* argv[])
{
	ledsFileName = argv[LEDS];
	hwregtraceFileName = argv[HWREGTRACE];
	readFileAsHex(argv[IMEMIN], InstructionRam, INSTRUCTION_RAM_SIZE);
	readFileAsHex(argv[DMEMIN], Ram, RAM_SIZE);
	readFileAsHex(argv[DISKIN], Disk, DISK_SIZE);
	monitorInitializer();

	traceFile = fopen(argv[TRACE], "w");
	asssertFileOpen(traceFile, argv[TRACE]);  

	irq2in = fopen(argv[IRQ2IN], "r");
	asssertFileOpen(irq2in, argv[IRQ2IN]);
	getNextIrq2StopCycle();

	int32_t numOfAssemblyOperations = 0;
	while (!ShouldExit) {
		if (irqEnable & irqStatus && !Interupted)
		{
			/*
			we understood from the fib example, that it is the assembly responsability to reset irq2.
			if (irqStatus & 0b100) // irq2status == 1
			{
				irqStatus &= 0b011; // irq2status = 0
			} */
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
		RegistersCopy[1] = Registers[1]; //because we need the immediate for the trace file, we moved down writing to the trace file, but copied all the relevant data so we'd have the imm that should be printed
		writeTraceToFile(lastPC, RegistersCopy, currentInstruction);

		numOfAssemblyOperations++;

		if (PC >= INSTRUCTION_RAM_SIZE)
		{
			ShouldExit = 1;
		}
	}

	writeFileAsHex(argv[DMEMOUT], Ram, RAM_SIZE);
	writeFileAsHex(argv[REGOUT], Registers, REGISTER_COUNT);
	writeFileAsHex(argv[DISKOUT], Disk, DISK_SIZE);
	writeToCyclesFileAsDec(argv[CYCLES], numOfAssemblyOperations);
	writeMonitorToFile(argv[MONITOR], argv[MONITOR_YUV]);

	fclose(irq2in);
	fclose(traceFile);

	return 0;
}

/*
input arguments:
file - the Operating System's pointer for a file.
filename - a string of the name of the file.

The function checks the file is found and it point to a file and not null, in case it doesn't it prints an error and exits
*/
void asssertFileOpen(FILE* file, char filename[])
{
	if (NULL == file) {
		printf("Error opening file %s!\n", filename);
		exit(1);
	}
}

/*
input arguments:
filename - a string of the name of the file.
outArray - an array of ints, that we read from.
size - the size of the array.

The function reads from the file and copy it to the given outArray in hexadecimal, if the file is shorter than the wanted size. it completes the remaining places with zeros.
*/
void readFileAsHex(char filename[], int outArray[], int size)
{
	unsigned int memory;
	FILE* file = fopen(filename, "r");

	asssertFileOpen(file, filename);

	int i = 0;
	while (i < size && fscanf(file, "%x", &memory) != EOF)
	{
		outArray[i] = memory;
		i++;
	}

	while (i < size) { //the function completes the array with 0 until it reaches the requested size
		outArray[i] = 0;
		i++;
	}

	fclose(file);
}

/*
input arguments:
filename - a string of the name of the file.
inArray - an array of ints, that we write from.
size - the size of the array.

The function writes to the file the given outArray in 8 hexadecimal numbers
*/
void writeFileAsHex(char filename[], int inArray[], int size)
{
	FILE* file = fopen(filename, "w");

	asssertFileOpen(file, filename);

	for (int i = 0; i < size; i++)
	{
		fprintf(file, "%08X\n", inArray[i]);
	}
}

/*
input arguments:
filename - a string of the name of the file.
numOfAssemblyOp - number of assembly operations used in total

The function writes to the file the number of cycles and number of assembly operations used in total
*/
void writeToCyclesFileAsDec(char filename[], int32_t numOfAssemblyOp)
{
	FILE* file = fopen(filename, "w");

	asssertFileOpen(file, filename);

	fprintf(file, "%d\n" "%d", Cycle, numOfAssemblyOp);

}

/*
input arguments:
lastPC - the last value of PC.
registers - the registers array.
instruction - an instruction (from the instruction memory)

The function writes to the trace file the last PC, the used instruction, and then the status of all registers
(each in hexadecimal with the number of character that is needed in accordance to their size)
*/
void writeTraceToFile(int16_t lastPC, int32_t registers[], int32_t instruction)
{
	fprintf(traceFile, "%03X %05X", lastPC, instruction);
	for (int i = 0; i < REGISTER_COUNT; i++)
	{
		fprintf(traceFile, " %08X", registers[i]);
	}

	fprintf(traceFile, "\n");
}

/*
The function copies the registers to a temp array called registersCopy.
The function is being used to cope with the problem, writing the parameters to the trace file, while having an Immediate.
In order to have the right PC of the start of the operation and the instruction, with the immediate being used in it, we used a bit of an ugly solution, but it works.
We copied all the relevant data (the PC at the start of the instruction, the instuction itself, and the registers), and changed only the immediate register of the copy to the immediate we received.
*/
void CopyRegisters()
{
	for (int i = 0; i < REGISTER_COUNT; i++)
	{
		RegistersCopy[i] = Registers[i];
	}
}