#pragma once
#include <inttypes.h>

#define REGISTER_COUNT 16
#define INSTRUCTION_RAM_SIZE 1024
#define RAM_SIZE 4096

extern int32_t Registers[];
extern int16_t PC;
extern int32_t InstructionRam[];
extern int32_t Ram[];