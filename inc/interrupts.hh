#ifndef interruptsh
#define interruptsh

#include<cstdint>
#include "defines.h"
#include "memory.hh"



class interrupts{
	uint8_t* IF;
	uint8_t* IE;

public:

	interrupts(_memory* mem);
	~interrupts();
	uint8_t check();

};



#endif

