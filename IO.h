#pragma once 
#include <inttypes.h>

/*extern struct IRQ {
	char irq0 : 1;
	char irq1 : 1;
	char irq2 : 1;
	char irqStatus0 : 1;
	char irqStatus1 : 1;
	char irqStatus2 : 1;
};

extern struct IRQ IRQ;*/

int32_t getIORegister(int8_t IORegisterNum);
void setIORegister(int8_t IORegisterNum, int32_t value);


