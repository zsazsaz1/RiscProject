#pragma once

extern void (*OpcodeMap[22])(int, int, int);
extern unsigned int irq2stopCycles;
extern FILE* irq2in;
void getNextIrq2StopCycle();