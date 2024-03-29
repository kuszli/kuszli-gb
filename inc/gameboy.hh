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
#include "serial.hh"

class gameboy{

	const uint32_t cycles_per_frame = 70224;
	uint8_t cycles;
	uint8_t button;
	uint32_t cycles_count;

	_gb_type gb_type;
	_memory* memory;
	cpu* _cpu;
	timer* _timer;
	dma* _dma;
	lcd_driver* _lcd_driver;
	joypad* _joypad;
	interrupts* _interrupts;
	audio_controller* _audio_controller;
	serial* _serial;


public:

	gameboy();
	~gameboy();
	_gb_type get_gb_type() { return gb_type; }
	void set_buttons(uint8_t butt){button = butt;}
	void run();
	void insert_cart(const std::string& game_name);
	void pull_out_cart();
	void reset() {_cpu->reset();}
	void set_debug(bool opt){_lcd_driver->set_oam_viewer(opt);}
	uint8_t* get_display_data() const {return _lcd_driver->screen();}
	uint8_t** oam_debug() const {return _lcd_driver->oam_screen();}

	void set_audio_rate(uint32_t frequency){ _audio_controller->set_sampling_freq(frequency); }
	void set_audio_channels(uint8_t channels){ _audio_controller->set_channels_count(channels); }
	void set_audio_buffer_size(uint16_t size){ _audio_controller->set_buffer_size(size); }

	const int16_t* get_audio_buffer() { return _audio_controller->get_buffer(); }
	
};



#endif
