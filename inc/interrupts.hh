#ifndef interruptsh
#define interruptsh

#include<cstdint>
#include "defines.h"

#define IE_REGISTER 0xFFFF


class interrupts{
	uint8_t* IF;
	uint8_t* IE;
	

public:

	interrupts(uint8_t* mem);
	uint8_t check();

};



#endif

