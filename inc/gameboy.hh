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

	_gb_type gb_type;
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
	_gb_type get_gb_type() { std::cout << gb_type << std::endl; return gb_type; }
	void set_buttons(uint8_t butt){button = butt;}
	void run();
	void insert_cart(const std::string& game_name);
	void pull_out_cart();
	void reset() {_cpu->reset();}
	void set_debug(bool opt){_lcd_driver->set_oam_viewer(opt);}
	uint8_t* get_display_data() const {return _lcd_driver->screen();}
	uint8_t** oam_debug() const {return _lcd_driver->oam_screen();}
	int16_t* audio_buffer() const {return _audio_controller->get_audio_buffer();}
	bool audio_buffer_ready() {return _audio_controller->is_buffer_ready();}
	int16_t* second_audio_buffer() { return _audio_controller->second_audio_buffer(); }
	int16_t* ready_audio_buffer() { return _audio_controller->ready_buffer(); }
	void reset_audio_buffer_state() { _audio_controller->reset_buffer_state();}
	void set_audio_callback_fun(void (*fp)(int16_t*)) { _audio_controller->set_callback_function(fp); }
	

};



#endif
