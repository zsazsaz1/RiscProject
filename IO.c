#include <stdio.h>
#include <inttypes.h>

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

int32_t getIORegister(int8_t IORegisterNum)
{
	switch (IORegisterNum)
	{
	case 0:
		return (irqEnable & 0b001) ? 1 : 0;
	case 1:
		return (irqEnable & 0b010) ? 1 : 0;
	case 2:
		return (irqEnable & 0b100) ? 1 : 0;
	case 3:
		return (irqStatus & 0b001) ? 1 : 0;
	case 4:
		return (irqStatus & 0b010) ? 1 : 0;
	case 5:
		return (irqStatus & 0b100) ? 1 : 0;
	case 6:
		return irqhandler & 0x3FF;
	case 7:
		return irqreturn & 0x3FF;
	case 8:
		return clks;
	case 9:
		return leds;
	case 10:
		return reserved;
	case 11:
		return timerenable & 1;
	case 12:
		return timercurrent;
	case 13:
		return timermax;
	case 14:
		return diskcmd & 0b11;
	case 15:
		return disksector & 0xBF;
	case 16:
		return diskbuffer & 0xFFF;
	case 17:
		return diskstatus & 0b1;
	case 18:
		return monitorcmd & 0b1;
	case 19:
		return monitorx & 0x1FF;
	case 20:
		return monitory & 0x1FF;
	case 21:
		return monitordata;
	default:
		printf("Attempt to access unrecognized IO register");
		exit(1);
		break;

	}
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
}

