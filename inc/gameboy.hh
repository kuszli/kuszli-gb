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
#include "audio_controller.hh"

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
	audio_controller* _audio_controller;


public:
	gameboy();
	~gameboy();
	void set_buttons(uint8_t butt){button = butt;}
	void run();
	void insert_cart(const std::string& game_name){memory->connect_rom(game_name);}
	void pull_out_cart();
	void reset() {_cpu->reset();}
	void set_debug(bool opt){_lcd_driver->set_oam_viewer(opt);}
	uint8_t* get_display_data() const {return _lcd_driver->screen();}
	uint8_t** oam_debug() const {return _lcd_driver->oam_screen();}
	uint8_t* audio_buffer() const {return _audio_controller->get_audio_buffer();}
	bool audio_buffer_ready() {return _audio_controller->is_buffer_ready();}
	uint8_t* second_audio_buffer() { return _audio_controller->second_audio_buffer(); }
	uint8_t* ready_audio_buffer() { return _audio_controller->ready_buffer(); }
	void reset_audio_buffer_state() { _audio_controller->reset_buffer_state();}
	

};



#endif
