#pragma once 
#include <inttypes.h>

#define MONITOR_X 352
#define MONITOR_Y 288

#define DISK_IO_CYCLES 1024
#define SECTOR_SIZE 512

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
extern uint32_t currentDiskTaskCycle;
extern char diskcmd;
extern char diskstatus;

int32_t getIORegister(int8_t IORegisterNum);
void setIORegister(int8_t IORegisterNum, int32_t value);
void monitorInitializer();
void writeMonitorToFile(char monitorFileName[], char monitorYuvFileName[]);
void readFromDisk();
void writeToDisk();