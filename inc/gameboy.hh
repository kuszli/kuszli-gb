#ifndef gameboyh
#define gameboyh

#include <string>
#include <cstdint>
#include "defines.h"
#include "cpu.hh"
#include "timer.hh"
#include "dma.hh"
#include "lcd_driver.hh"
#include "joypad.hh"
#include "interrupts.hh"
#include "memory.hh"

class gameboy{

	const uint32_t cycles_per_frame = 70224;
	uint8_t cycles;
	uint8_t button;

	_memory* memory;
	cpu* _cpu;
	timer* _timer;
	dma* _dma;
	lcd_driver* _lcd_driver;
	joypad* _joypad;
	interrupts* _interrupts;


public:
	gameboy();
	~gameboy();
	void set_buttons(uint8_t butt){button = butt;}
	void run();
	void insert_cart(const std::string& game_name){memory->connect_rom(game_name);}
	uint8_t** get_display_data() const {return _lcd_driver->screen();}
	uint8_t** oam_debug() const {return _lcd_driver->oam_screen();}

};



#endif
