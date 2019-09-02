#include "memory.hh"

_memory::_memory(const std::string& rom_name){
	memory = new uint8_t[65536];
	rom_banks = new uint8_t[16*65536]; //64 banks
	curr_bank = 0;

	rom = new std::fstream(rom_name.c_str(), std::ios::in | std::ios::binary);

	rom->seekg(0, rom->end);
	unsigned int length = rom->tellg();
	rom->seekg(0, rom->beg);
	std::cout << std::hex << length << std::endl;
	if(length <= 0x8000){ //32 KByte
		rom->read((char*)memory, length);
		mbc = false;
	}
	else{
		rom->read((char*)memory, 0x8000);
		rom->seekg(0, rom->beg);
		rom->read((char*)rom_banks, length);
		mbc = true;
	}

}

_memory::~_memory(){

	if(memory != nullptr){
		delete[] memory;
		memory = nullptr;
	}

	if(rom_banks != nullptr){
		delete[] rom_banks;
		rom_banks = nullptr;
	}

	if(rom != nullptr){
		rom->close();
		rom = nullptr;
	}
}

uint8_t& _memory::operator[](const uint16_t index){ 
	if((index >= 0x4000 && index < 0x8000) && mbc) //rom bank 1 area
		return rom_banks[0x4000 * curr_bank + index - 0x4000];
	else
		return memory[index]; 
}

const uint8_t& _memory::operator[](const uint16_t index) const {
	if((index >= 0x4000 && index < 0x8000) && mbc) //rom bank 1 area
		return rom_banks[0x4000 * curr_bank + index - 0x4000];
	else
		return memory[index]; 
}

void _memory::write(const uint16_t index, const uint8_t value){

	if(index >= 0x2000 && index <= 0x3FFF){
		curr_bank = value & 0x1F;
		if(value & 0xF == 0)
			curr_bank |= 0x1;
	}

	if(index >= 0x4000 && index <= 0x5FFF){
		curr_bank |= value << 5;
	}



	if(index < 0x8000){
		dummy = value;
	}

	else if(index >= 0xC000 && index < 0xDE00){
		memory[index] = value;
		memory[0xE000 + index - 0xC000] = value;
	}

	else if(index >= 0xE000 && index < 0xFE00){
		memory[index] = value;
		memory[0xC000 + index - 0xE000] = value;
	}
	
	else
		memory[index] = value;

}


