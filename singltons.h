#pragma once
#include <inttypes.h>

#define REGISTER_COUNT 16
#define INSTRUCTION_RAM_SIZE 1024
#define RAM_SIZE 4096
#define DISK_SIZE (64*(1<<10))

extern int32_t Registers[REGISTER_COUNT];
extern int16_t PC;
extern int32_t Cycle;
extern int32_t InstructionRam[INSTRUCTION_RAM_SIZE];
extern int32_t Ram[RAM_SIZE];
extern int32_t Disk[DISK_SIZE];
extern char Interupted;
extern char ShouldExit;
