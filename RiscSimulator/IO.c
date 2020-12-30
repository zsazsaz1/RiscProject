#define _CRT_SECURE_NO_DEPRECATE

#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include "singltons.h"

char irqEnable = 0; // first bit is irq0, second 1, and third 2.
char irqStatus = 0; // first bit is irq0, second 1, and third 2.
int16_t irqhandler = 0; // TD 10 bits
int16_t irqreturn = 0; // TD 10 bits
int32_t clks = 0;
int32_t leds = 0;
int32_t reserved = 0;
char timerenable = 0; // 1 bit
int32_t timercurrent = 0;
int32_t timermax = 0;
char diskcmd = 0; // 2 bits
char disksector = 0; // 7 bits
int16_t diskbuffer = 0; // 12 bits
char diskstatus = 0; // 1 bit
char monitorcmd = 0; // 1 bit
int16_t monitorx = 0; // 9 bits
int16_t monitory = 0; // 9 bits
char monitordata = 0;
char *hwregtraceFileName;

extern void asssertFileOpen(FILE* file, char filename[]);

void writeHwregtraceToFile(char filename[], int32_t cycle, char ReadorWrite[], char name[], int32_t data)
{
	FILE* file = fopen(filename, "a");

	asssertFileOpen(file, filename);

	fprintf(file, "%d %s %s %08X", cycle, ReadorWrite, name, data);

	fprintf(file, "\n");
	fclose(file);
}


int32_t getIORegister(int8_t IORegisterNum)
{
	int32_t retValue;
	switch (IORegisterNum)
	{
	case 0:
		retValue = (irqEnable & 0b001) ? 1 : 0;
	case 1:
		retValue = (irqEnable & 0b010) ? 1 : 0;
	case 2:
		retValue = (irqEnable & 0b100) ? 1 : 0;
	case 3:
		retValue = (irqStatus & 0b001) ? 1 : 0;
	case 4:
		retValue = (irqStatus & 0b010) ? 1 : 0;
	case 5:
		retValue = (irqStatus & 0b100) ? 1 : 0;
	case 6:
		retValue = irqhandler & 0x3FF;
	case 7:
		retValue = irqreturn & 0x3FF;
	case 8:
		retValue = clks;
	case 9:
		retValue = leds;
	case 10:
		retValue = reserved;
	case 11:
		retValue = timerenable & 1;
	case 12:
		retValue = timercurrent;
	case 13:
		retValue = timermax;
	case 14:
		retValue = diskcmd & 0b11;
	case 15:
		retValue = disksector & 0xBF;
	case 16:
		retValue = diskbuffer & 0xFFF;
	case 17:
		retValue = diskstatus & 0b1;
	case 18:
		retValue = monitorcmd & 0b1;
	case 19:
		retValue = monitorx & 0x1FF;
	case 20:
		retValue = monitory & 0x1FF;
	case 21:
		retValue = monitordata;
	default:
		printf("Attempt to access unrecognized IO register");
		exit(1);
		return  -1;
	}
	writeHwregtraceToFile(hwregtraceFileName, Cycle, "READ", getIORegName(IORegisterNum), retValue);
	return retValue;
}


void setIORegister(int8_t IORegisterNum, int32_t value)
{
	switch (IORegisterNum)
	{
	case 0:
		irqEnable = (0 == value) ? (irqEnable & 0b110) : (irqEnable | 0b001);
		break;
	case 1:
		irqEnable = (0 == value) ? (irqEnable & 0b101) : (irqEnable | 0b010);
		break;
	case 2:
		irqEnable = (0 == value) ? (irqEnable & 0b011) : (irqEnable | 0b100);
		break;
	case 3:
		irqStatus = (0 == value) ? (irqEnable & 0b110) : (irqEnable | 0b001);
		break;
	case 4:
		irqStatus = (0 == value) ? (irqEnable & 0b101) : (irqEnable | 0b010);
		break;
	case 5:
		irqStatus = (0 == value) ? (irqEnable & 0b011) : (irqEnable | 0b100);
		break;
	case 6:
		irqhandler = value & 0x3FF;
		break;
	case 7:
		irqreturn = value & 0x3FF;
		break;
	case 8:
		clks = value;
		break;
	case 9:
		leds = value;
		break;
	case 10:
		reserved = value;
		break;
	case 11:
		timerenable = value & 0b1;
		break;
	case 12:
		timercurrent = value;
		break;
	case 13:
		timermax = value;
		break;
	case 14:
		diskcmd = value & 0b11;
		break;
	case 15:
		disksector = value & 0xBF;
		break;
	case 16:
		diskbuffer = value & 0xFFF;
		break;
	case 17:
		diskstatus = value & 0b1;
		break;
	case 18:
		monitorcmd = value & 0b1;
		break;
	case 19:
		monitorx = value & 0x1FF;
		break;
	case 20:
		monitory = value & 0x1FF;
		break;
	case 21:
		monitordata = value & 0xFF;
		break;
	default:
		printf("Attempt to access unrecognized IO register");
		exit(1);
		break;

	}
	writeHwregtraceToFile(hwregtraceFileName, Cycle, "WRITE", getIORegName(IORegisterNum), value);
}

