#include<iostream>

#include "cpu.hh"
#include "timer.hh"
#include "lcd_driver.hh"
#include "interrupts.hh"
#include "memory.hh"
#include "joypad.hh"
#include "dma.hh"
#include "sfml_interface.hh"
#include<unistd.h>
#include<cstdlib>
#include<sys/time.h>
#include<chrono>
#include<string>
#include <SFML/Graphics.hpp>


#define INSTRUCTIONS_PER_VBLANK 70224
#define VBLANK_IN_MICROSECONDS 16742


int main(int argc, char** argv){

if(argc < 2 || argc > 3){
	std::cout << "Wrong number of parameters. Usage: kuszli-gb \"rom_name.gb\" [-d]\n";
	return -1;
	}

bool dbg = false;
std::string rom_name;

if(argc == 3){

	if(argv[1][0] == '-'){
		if(argv[1][1] == 'd'){
			dbg = true;
			rom_name = argv[2];
		}
		else{
			std::cout << "Unknown option: " << argv[1][1] << "Usage: kuszli-gb \"rom_name.gb\" [-d]\n";
			return -1;
		}
	}
	else{
		if(argv[2][1] == 'd'){
			dbg = true;
			rom_name = argv[1];
		}
		else{
			std::cout << "Unknown option: " << argv[2][1] << ". Usage: kuszli-gb \"rom_name.gb\" [-d]\n";
			return -1;
		}
	}

}
else
	rom_name = argv[1];

uint8_t cycles;

_memory* memory = new _memory(rom_name);
cpu* _cpu = new cpu(memory);
timer* _timer = new timer(memory);
lcd_driver*	_lcd_driver = new lcd_driver(memory);
interrupts*	_interrupts = new interrupts(memory);
dma* _dma = new dma(memory);
joypad* _joypad = new joypad(memory);

sfml_interface interface(dbg);


while(interface.is_window_open()){

auto start = std::chrono::steady_clock::now();


for(int i = 0; i < INSTRUCTIONS_PER_VBLANK; i+=cycles){
	cycles = _cpu->execute(_cpu->decode());
	_timer->update(cycles);
	_dma->update(cycles);
	_lcd_driver->update(cycles);
	_joypad->update(interface.get_key());
	_cpu->handle_interrupts(_interrupts->check());

}

interface.display(_lcd_driver->screen());
if(dbg) 
	interface.show_oam(_lcd_driver->oam_screen());

interface.check_events();

auto end = std::chrono::steady_clock::now();

if(VBLANK_IN_MICROSECONDS - std::chrono::duration_cast<std::chrono::microseconds>(end-start).count() > 0)
	usleep(VBLANK_IN_MICROSECONDS - std::chrono::duration_cast<std::chrono::microseconds>(end-start).count()); 

}

memory->save_ram();

}


