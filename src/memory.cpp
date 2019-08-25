#include "memory.hh"

_memory::_memory(){
	memory = new uint8_t[65536];
	rom_bank = 0;

	rom = new std::fstream("src/rom.gb", std::ios::in | std::ios::binary);
	//rom->open();

	rom->seekg(0, rom->end);
	unsigned int length = rom->tellg();
	rom->seekg(0, rom->beg);

	if(length < 0x8000) //32 KByte
		rom->read((char*)memory, length);
	else
		rom->read((char*)memory, 0x8000);

}

_memory::~_memory(){

	if(memory != nullptr){
		delete[] memory;
		memory = nullptr;
	}

	if(rom != nullptr){
		rom->close();
		rom = nullptr;
	}
}


void _memory::write(const uint16_t index, const uint8_t value){
	if(index < 0x8000){
		rom_bank = value;
	}
	
	else
		memory[index] = value;
}


