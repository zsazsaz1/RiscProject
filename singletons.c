#include "singltons.h"

int32_t Registers[REGISTER_COUNT] = { 0 };
int16_t PC = 0; // need to be 10 bits
int32_t Cycle = 0; // need to be 10 bits
int32_t InstructionRam[INSTRUCTION_RAM_SIZE]; // need to be 20 bits
int32_t Ram[RAM_SIZE];
char Interupted = 0;
char ShouldExit = 0;