#ifndef joypadh
#define joypadh

#include <cstdint>
#include <iostream>
#include "memory.hh"
#include "defines.h"
#define JOYPAD 0xFF00

class joypad{

	uint8_t* joypad_register;
	uint8_t* IF;

	uint8_t last_state;
	uint8_t button;
	uint8_t keys_select();

public:

	joypad(_memory* mem);
	void update(uint8_t butt);
	void write(uint8_t butt);
	uint8_t debug(){return *joypad_register;}

};

#endif

