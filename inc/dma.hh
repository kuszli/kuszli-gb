#ifndef dmah
#define dmah

#include<cstdint>
#include<cstring>
#include "memory.hh"
#include "defines.h"


class dma{

	const uint16_t transfer_time = 640;
	uint16_t counter;

	_memory memory;
	uint8_t* dma_register;
	uint8_t* oam;

	void transfer();

public:
	
	dma(_memory &mem);
	void update(uint8_t cycles);




};



#endif
