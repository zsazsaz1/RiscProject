#define _CRT_SECURE_NO_DEPRECATE

#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include "singltons.h"
#include "IO.h"

uint8_t Monitor[MONITOR_X][MONITOR_Y];

char irqEnable = 0; // first bit is irq0, second 1, and third 2.
char irqStatus = 0; // first bit is irq0, second 1, and third 2.
int16_t irqhandler = 0; // 10 bits
int16_t irqreturn = 0; // 10 bits
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

/*
input arguments:
filename[] - the file name we write to.
cycle - the current cycle.
ReadorWrite[] - a string, which is going to contain the words "read" or "write" depending on the operation we are doing at the time.
IORegistersNum - the number of the IO Register that is being read or written to.
data - the value that is being read from the IO register or written to it.

The function write to the file hwregtrace whenever an IO reister is being read from or written to, it prints the cycle it happened, 
if it was read or write, the name of the consecutive IO register that is baing called and the data that was read or written to it.
*/
void writeHwregtraceToFile(char filename[], int32_t cycle, char ReadorWrite[], int8_t IORegisterNum, int32_t data)
{
	char* name = getIORegisterName(IORegisterNum);
	FILE* file = fopen(filename, "a");

	asssertFileOpen(file, filename);

	fprintf(file, "%d %s %s %08X", cycle, ReadorWrite, name, data);

	fprintf(file, "\n");
	fclose(file);
}

/*
input arguments:
filename[] - the file name we write to.
ledsStatus - 32 bits containing the status of the 32 leds, a bit with one says the respective led is on, and a 0 is off.

The function write to the file LedsStatus whenever an the leds IO register is being written, it prints the cycle it happened, and the new status of the leds.
*/
void writeLedsStatusTofile(char filename[], int32_t ledsStatus)
{
	FILE* file = fopen(filename, "a");

	asssertFileOpen(file, filename);

	fprintf(file, "%d %08X", Cycle, ledsStatus);

	fprintf(file, "\n");
	fclose(file);
}

/*
input arguments:
IORegisterNum - the number of the IO register that is being called.

result argument:
Unless the IO register can't give his original value, the functions returns the value stored in the respective IO register.
(if the IO register number doesn't exits, the funtion prints an error and returns -1.)

At the end the function uses writeToHwregtrace() with the "READ" string. 
*/
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
		retValue = 0; //calling for monitorcmd always return 0 
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

/*
input arguments:
IORegisterNum - the number of the IO register that is being called.
value - a value to be written to the respective IO register.

Unless the IO register is read only, the functions writes the value to respective IO register. some IO registers uses their corresponding function if being set.
(if the IO register number doesn't exits, the funtion prints an error and exits.)

At the end the function uses writeToHwregtrace() with the "WRITE" string.
*/
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
		if (0 == diskcmd) // in order to prevent the assembly access to the disk while it's in the middle of a previous task
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
		writePixel(); //-
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

/*
input arguments:
IORegisterNum - the number of the IO register that is being called.

return argument:
return a string with the name of the corresponding IO registers.
*/
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

/*
writes a data to the monitor array of arrays in the pixel In the X position of monitorx, and the Y position of monitory.
*/
void writePixel() {
	Monitor[monitorx][monitory] = monitordata;
}

/*
initialize the monitor to all black.
*/
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

/*
input arguments:
monitorFileName - a string with the monitor file name.
monitorYuvFileName - a string with the monitorYuv file name that will be used with the yuv software.

The fuction is being used at the end of the simulator operation to write the data of the monitor to two files, each in the requested way.
*/
void writeMonitorToFile(char monitorFileName[], char monitorYuvFileName[])
{
	FILE* monitorFile = fopen(monitorFileName, "w");
	FILE* monitorYuvFile = fopen(monitorYuvFileName, "wb"); // writes in binary

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

/*
The function is used to read the inserted sector from the disk, and load it onto the Ram.
*/
void readFromDisk()
{
	int16_t diskSectorPointer = disksector * SECTOR_SIZE;
	for (int16_t i = 0; i < SECTOR_SIZE; i++)
	{
		Ram[diskbuffer + i] = Disk[diskSectorPointer + i]; // in a growing order
	}
}

/*
The function is used to write onto the inserted sector of the disk, the data of from the Ram in the corresponding place.
*/
void writeToDisk()
{
	int16_t diskSectorPointer = disksector * SECTOR_SIZE;
	for (int16_t i = 0; i < SECTOR_SIZE; i++)
	{
		Disk[diskSectorPointer + i] = Ram[diskbuffer + i]; // in a growing order
	}
}