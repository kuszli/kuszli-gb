#ifndef gameboyh
#define gameboyh

#include "defines.h"
#include "cpu.hh"
#include "timer.hh"
#include "lcd_driver.hh"
#include "interrupts.hh"
#include "memory.hh"

class gameboy{

	const uint16_t cycles_per_frame = 70224;
	uint8_t cycles;
	uint8_t button;

	_memory* memory;
	cpu* _cpu;
	timer* _timer;
	lcd_driver* _lcd_driver;
	interrupts* _interrupts;


public:
	gameboy();
	~gameboy();
	void set_buttons(uint8_t butt) const;
	void run() const;

};



#endif
