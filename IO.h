#pragma once 
#include <inttypes.h>

int32_t getIORegister(int8_t IORegisterNum);
void setIORegister(int8_t IORegisterNum, int32_t value);

extern char irqEnable;
extern char irqStatus;
extern int16_t irqhandler;
extern int16_t irqreturn;
extern char timerenable; // 1 bit
extern int32_t timercurrent;
extern int32_t timermax;