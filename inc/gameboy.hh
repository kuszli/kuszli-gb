#ifndef gameboyh
#define gameboyh

#include "defines.h"
#include "cpu.hh"
#include "timer.hh"
#include "lcd_driver.hh"
#include "interrupts.hh"

class gameboy{

	const uint16_t cycles_per_frame = 70224;

	uint8_t *memory;
	cpu* _cpu;
	timer* _timer;
	lcd_driver* _lcd_driver;
	interrupts* _interrupts;


public:
	gameboy();
	~gameboy();
	void run();

};














#endif
