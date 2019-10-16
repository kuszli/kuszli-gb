#include "gameboy.hh"



gameboy::gameboy(): button(0), cycles(0), cycles_count(0), gb_type(dmg)
{
	memory = new _memory;
	_cpu = new cpu(memory);
	_timer = new timer(memory);
	_dma = new dma(memory);
	_lcd_driver = new lcd_driver(memory);
	_joypad = new joypad(memory);
	_interrupts = new interrupts(memory);
	_audio_controller = new audio_controller(memory);
	_serial = new serial(memory);
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
	delete _serial;
	_serial = nullptr;

}

void gameboy::insert_cart(const std::string &game_name){

	memory->connect_rom(game_name);
	gb_type = memory->get_gb_type();
	_lcd_driver->set_gb_type(gb_type);

}

void gameboy::run(){

	for(;;){

		if(_dma->hdma_transfer){
			cycles = 4 + 32*_cpu->speed();
			--_dma->transfer_size;
			if(_dma->transfer_size == 0)
				_dma->hdma_transfer = false;
		}
		else

		cycles = _cpu->execute(_cpu->decode());
		_timer->update(cycles);
		_dma->update(cycles);
		_lcd_driver->update(cycles/_cpu->speed());
		_audio_controller->update(cycles/_cpu->speed());
		_serial->update(cycles);
		_joypad->update(button);
		_cpu->handle_interrupts(_interrupts->check());
		//std::cout << *_cpu->regs16[1] << std::endl;
		cycles_count += cycles;
		if(memory->operator[](0xFF44) == 153){
			cycles_count = 0;
			break;
		}
	}


}


void gameboy::pull_out_cart(){
	_cpu->reset();
	memory->disconnect_rom();
}
	
		
	
	

