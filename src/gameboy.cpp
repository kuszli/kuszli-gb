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
	_audio_controller = new audio_controller(memory);
}


gameboy::~gameboy(){

	delete memory;
	memory = nullptr;
	delete _cpu;
	_cpu = nullptr;
	delete _timer;
	_timer = nullptr;
	delete _dma;
	_dma = nullptr;
	delete _lcd_driver;
	_lcd_driver = nullptr;
	delete _joypad;
	_joypad = nullptr;
	delete _interrupts;
	_interrupts = nullptr;
	delete _audio_controller;
	_audio_controller = nullptr;

}

void gameboy::run(){

	for(;;){
		cycles = _cpu->execute(_cpu->decode());
		_timer->update(cycles);
		_dma->update(cycles);
		_lcd_driver->update(cycles);
		_joypad->update(button);
		_cpu->handle_interrupts(_interrupts->check());
		_audio_controller->update(cycles);

		if(memory->operator[](0xFF44) == 153){
			break;
		}
	}


}


void gameboy::pull_out_cart(){
	_cpu->reset();
	memory->disconnect_rom();
}
	
		
	
	

