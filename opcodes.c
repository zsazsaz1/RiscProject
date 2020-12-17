#include <stdio.h>
#include <inttypes.h>
#include "singltons.h"



int func() {
	printf("%X", InstructionRam[0]);
	return 1;
}