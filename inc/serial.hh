#ifndef serialhh
#define serialhh

#include <cstdint>
#include "memory.hh"
#include "defines.h"

class serial{

	uint8_t* transfer_data;
	uint8_t* control;
	uint8_t* IF;
	
	bool* trigger;
	uint16_t cycles_to_update;
	uint16_t serial_cycles;
	bool transfer_time;
	uint8_t bits_transfered;

public:

	serial(_memory* mem);
	void update(uint8_t cycles);
	
};


#endif
