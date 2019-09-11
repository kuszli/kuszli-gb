#include "gameboy.hh"



gameboy::gameboy(){
	memory = new _memory;
	_cpu = new cpu(memory);
	_timer = new timer(memory);
	_lcd_driver = new lcd_driver(memory);
	_interrupts = new interrupts(memory);
}


gameboy::~gameboy(){

	if(memory != nullptr){
		delete[] memory;
		memory = nullptr;
	}
	delete _cpu;
	delete _timer;
	delete _lcd_driver;
	delete _interrupts;
}

void gameboy::run(){

	uint8_t cycles;

	while(1){

		for(int i = 0; i < cycles_per_frame; i += cycles){
			cycles = _cpu->execute(_cpu->decode());
			_timer->update(cycles);
			_lcd_driver->update(cycles);
			_cpu->handle_interrupts(_interrupts->check());
		}

	}

}
	
		
	
	

