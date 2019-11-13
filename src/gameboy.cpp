#include "gameboy.hh"
#include <fstream>

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
	deb = false;
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
	rom_name = game_name;

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

		if(deb)
			debug();
		
		if(memory->operator[](0xFF44) == 153)
			break;

	
	}


}


void gameboy::pull_out_cart(){
	_cpu->reset();
	memory->disconnect_rom();
}
	
	
void gameboy::save_state(){

	std::string save_state_name;
	std::size_t dot = rom_name.find_last_of('.');

	save_state_name = rom_name.substr(0, dot);
	save_state_name += ".ss";

	std::fstream* save_state_file = new std::fstream(save_state_name.c_str(), std::ios::out | std::ios::binary);
	if(!save_state_file->is_open()){
		throw std::runtime_error("Save state file creating error.");
	}
	_cpu->save_state(save_state_file);
	memory->save_state(save_state_file);
	save_state_file->close();
	

}	
	
void gameboy::load_state(){

	std::string save_state_name;
	std::size_t dot = rom_name.find_last_of('.');

	save_state_name = rom_name.substr(0, dot);
	save_state_name += ".ss";

	std::fstream* save_state_file = new std::fstream(save_state_name.c_str(), std::ios::in | std::ios::binary);
	if(!save_state_file->is_open())
		return;

	save_state_file->seekg(0, save_state_file->end);
	unsigned int length = save_state_file->tellg();
	save_state_file->seekg(0, save_state_file->beg);

	unsigned int predicted_size = 16 + memory->save_state_size();

	if(length != predicted_size){
		save_state_file->close();
		return;
	}
	
	_cpu->load_state(save_state_file);
	memory->load_state(save_state_file);
	save_state_file->close();


}

void gameboy::debug(){

	static std::string command;
	_cpu->print_regs();

	std::getline(std::cin, command);
	if(command == "exit" || command == "e"){
		deb = false;
	}
	
}
