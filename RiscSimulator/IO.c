#define _CRT_SECURE_NO_DEPRECATE

#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include "singltons.h"
#include "IO.h"

uint8_t Monitor[MONITOR_X][MONITOR_Y];

char irqEnable = 0; // first bit is irq0, second 1, and third 2.
char irqStatus = 0; // first bit is irq0, second 1, and third 2.
int16_t irqhandler = 0; // TD 10 bits
int16_t irqreturn = 0; // TD 10 bits
uint32_t leds = 0;
int32_t reserved = 0;
char timerenable = 0; // 1 bit
uint32_t timercurrent = 0;
uint32_t timermax = 0;
char diskcmd = 0; // 2 bits
char disksector = 0; // 7 bits
int16_t diskbuffer = 0; // 12 bits
char diskstatus = 0; // 1 bit
char monitorcmd = 0; // 1 bit
int16_t monitorx = 0; // 9 bits
int16_t monitory = 0; // 9 bits
unsigned char monitordata = 0;
uint32_t currentDiskTaskCycle;
char* hwregtraceFileName;
char* ledsFileName;

void asssertFileOpen(FILE* file, char filename[]);

char* getIORegisterName(int8_t IORegisterNum);
void writePixel();
void readFromDisk();
void writeToDisk();

void writeHwregtraceToFile(char filename[], int32_t cycle, char ReadorWrite[], int8_t IORegisterNum, int32_t data)
{
	char* name = getIORegisterName(IORegisterNum);
	FILE* file = fopen(filename, "a");

	asssertFileOpen(file, filename);

	fprintf(file, "%d %s %s %08X", cycle, ReadorWrite, name, data);

	fprintf(file, "\n");
	fclose(file);
}

void writeLedsStatusTofile(char filename[], int32_t ledsStatus)
{
	FILE* file = fopen(filename, "a");

	asssertFileOpen(file, filename);

	fprintf(file, "%d %08X", Cycle, ledsStatus);

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
		break;
	case 1:
		retValue = (irqEnable & 0b010) ? 1 : 0;
		break;
	case 2:
		retValue = (irqEnable & 0b100) ? 1 : 0;
		break;
	case 3:
		retValue = (irqStatus & 0b001) ? 1 : 0;
		break;
	case 4:
		retValue = (irqStatus & 0b010) ? 1 : 0;
		break;
	case 5:
		retValue = (irqStatus & 0b100) ? 1 : 0;
		break;
	case 6:
		retValue = irqhandler & 0x3FF;
		break;
	case 7:
		retValue = irqreturn & 0x3FF;
		break;
	case 8:
		retValue = Cycle;
		break;
	case 9:
		retValue = leds;
		break;
	case 10:
		retValue = reserved;
		break;
	case 11:
		retValue = timerenable & 1;
		break;
	case 12:
		retValue = timercurrent;
		break;
	case 13:
		retValue = timermax;
		break;
	case 14:
		retValue = diskcmd & 0b11;
		break;
	case 15:
		retValue = disksector & 0xBF;
		break;
	case 16:
		retValue = diskbuffer & 0xFFF;
		break;
	case 17:
		retValue = diskstatus & 0b1;
		break;
	case 18:
		retValue = 0; //monitorcmd & 0b1 
		break;
	case 19:
		retValue = monitorx & 0x1FF;
		break;
	case 20:
		retValue = monitory & 0x1FF;
		break;
	case 21:
		retValue = monitordata;
		break;
	default:
		printf("Attempt to access unrecognized IO register");
		exit(1);
		return -1;
	}
	writeHwregtraceToFile(hwregtraceFileName, Cycle, "READ", IORegisterNum, retValue);
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
		//clks is a read only variable for us
		break;
	case 9:
		leds = value;
		writeLedsStatusTofile(ledsFileName, value);
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
		if (0 == diskcmd) // in order to prevent the assembler access to the disk while it's in the middle of a previous task
		{
			diskcmd = value & 0b11;
			if ((1 == diskcmd) | (2 == diskcmd))
			{
				diskstatus = 1;
				currentDiskTaskCycle = Cycle;
			}
		}
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
		writePixel(); //-----
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
		return;
	}
	writeHwregtraceToFile(hwregtraceFileName, Cycle, "WRITE", IORegisterNum, value);
}

char* getIORegisterName(int8_t IORegisterNum)
{
	switch (IORegisterNum)
	{
	case 0:
		return "irq0enable";
	case 1:
		return "irq1enable";
	case 2:
		return "irq2enable";
	case 3:
		return "irq0status";
	case 4:
		return "irq1status";
	case 5:
		return "irq2status";
	case 6:
		return "irqhandler";
	case 7:
		return "irqreturn";
	case 8:
		return "clks";
	case 9:
		return "leds";
	case 10:
		return "reserved";
	case 11:
		return "timerenable";
	case 12:
		return "timercurrent";
	case 13:
		return "timermax";
	case 14:
		return "diskcmd";
	case 15:
		return "disksector";
	case 16:
		return "diskbuffer";
	case 17:
		return "diskstatus";
	case 18:
		return "monitorcmd";
	case 19:
		return "monitorx";
	case 20:
		return "monitory";
	case 21:
		return "monitordata";
	default:
		printf("Attempt to access unrecognized IO register");
		exit(1);
		return "1";
	}

}

void writePixel() {
	Monitor[monitorx][monitory] = monitordata;
}

void monitorInitializer()
{
	for (int16_t i = 0; i < MONITOR_X; i++)
	{
		for (int16_t j = 0; j < MONITOR_Y; j++)
		{
			Monitor[i][j] = 0;
		}
	}
}

void writeMonitorToFile(char monitorFileName[], char monitorYuvFileName[])
{
	FILE* monitorFile = fopen(monitorFileName, "w");
	FILE* monitorYuvFile = fopen(monitorYuvFileName, "wb");

	asssertFileOpen(monitorFile, monitorFileName);
	asssertFileOpen(monitorYuvFile, monitorFileName);

	for (int16_t i = 0; i < MONITOR_Y; i++)
	{
		for (int16_t j = 0; j < MONITOR_X; j++)
		{
			fprintf(monitorFile, "%02X\n", Monitor[j][i]);
			fwrite(&Monitor[j][i], sizeof(uint8_t), 1, monitorYuvFile);
		}
	}

	fclose(monitorFile);
	fclose(monitorYuvFile);
}

void readFromDisk()
{
	int16_t diskSectorPointer = disksector * SECTOR_SIZE;
	for (int16_t i = 0; i < SECTOR_SIZE; i++)
	{
		Ram[diskbuffer + i] = Disk[diskSectorPointer + i];
	}
}

void writeToDisk()
{
	int16_t diskSectorPointer = disksector * SECTOR_SIZE;
	for (int16_t i = 0; i < SECTOR_SIZE; i++)
	{
		Disk[diskSectorPointer + i] = Ram[diskbuffer + i];
	}
}