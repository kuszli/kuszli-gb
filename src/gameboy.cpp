#include "gameboy.hh"



gameboy::gameboy(): button(0), cycles(0)
{
	memory = new _memory;
	_cpu = new cpu(memory);
	_timer = new timer(memory);
	_dma = new dma(memory);
	_lcd_driver = new lcd_driver(memory);
	_joypad = new joypad(memory);
	_interrupts = new interrupts(memory);
}


gameboy::~gameboy(){

	delete[] memory;
	memory = nullptr;

	delete _cpu;
	delete _timer;
	delete _dma;
	delete _lcd_driver;
	delete _joypad;
	delete _interrupts;
}

void gameboy::run(){

	for(int i = 0; i < cycles_per_frame; i += cycles){
		cycles = _cpu->execute(_cpu->decode());
		_timer->update(cycles);
		_dma->update(cycles);
		_lcd_driver->update(cycles);
		_joypad->update(button);
		_cpu->handle_interrupts(_interrupts->check());
	}


}
	
		
	
	

