#pragma once 
#include <inttypes.h>

#define MONITOR_X 352
#define MONITOR_Y 288

extern char irqEnable;
extern char irqStatus;
extern int16_t irqhandler;
extern int16_t irqreturn;
extern char timerenable; // 1 bit
extern uint32_t timercurrent;
extern uint32_t timermax;
extern char *hwregtraceFileName;
extern char *ledsFileName;
extern int16_t irqreturn;
extern uint8_t Monitor[MONITOR_X][MONITOR_Y];

int32_t getIORegister(int8_t IORegisterNum);
void setIORegister(int8_t IORegisterNum, int32_t value);
void monitorInitializer();
void writeMonitorToFile(char filename[]);