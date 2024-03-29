#include "memory.hh"
#include <cstring>

_memory::_memory(){

	memory = new uint8_t[65536];
	wram_bank = vram_bank = 0;
	rom_connected = false;
	dummy = 0xFF;
	dma_time = false;
	dma_request = false;
	hblank_dma_time = false;
	hdma_request = false;
	hdma_transfer = false;
	gdma_transfer = 0;
	hdma_src_trigger = false;
	hdma_dest_trigger = false;
	serial_trigg = false;
	rom = nullptr;
	external_ram = nullptr;
	rom_banks = nullptr;
	rtc_registers = nullptr;
	_rtc = nullptr;
	cgb_wram = new uint8_t[0x8000];
	cgb_vram = new uint8_t[0x2000];
	bg_palette_ram = new uint8_t[64];
	ob_palette_ram = new uint8_t[64];
	ram_enable = false;
	ex_ram = false;
	chan1_trigg = chan2_trigg = chan3_trigg = chan4_trigg = false;
	mbc_type = none;
	gb_type = dmg;
}

_memory::~_memory(){

	save_ram();

	delete[] memory;
	memory = nullptr;

	delete[] rom_banks;
	rom_banks = nullptr;
	
	if(rom != nullptr){
		rom->close();
		rom = nullptr;
	}

	delete[] external_ram;
	external_ram = nullptr;
	
	delete[] rtc_registers;
	rtc_registers = nullptr;

	delete[] cgb_wram;
	cgb_wram = nullptr;

	delete[] cgb_vram;
	cgb_vram = nullptr;

	delete[] bg_palette_ram;
	bg_palette_ram = nullptr;

	delete[] ob_palette_ram;
	ob_palette_ram = nullptr;
	

}

void _memory::connect_rom(const std::string& rom_name){

	bank_select1 = 1;
	bank_select2 = 0;
	bank_select3 = 0;


	mode_select = 0;

	rtc_registers = new uint8_t[5];
	_rtc = new rtc(rtc_registers);
	_rom_name = rom_name;

	rom = new std::fstream(rom_name.c_str(), std::ios::in | std::ios::binary);
	

	if(!rom->is_open())
		throw std::runtime_error("Error opening game file.");

	rom->seekg(0, rom->end);
	unsigned int length = rom->tellg();
	rom->seekg(0, rom->beg);

	rom->read((char*)memory, 0x8000);
	rom->seekg(0, rom->beg);

	if(memory[0x148] <= 0x7)
		rom_banks = new uint8_t[length];
	
	else
		throw std::runtime_error("Unsupported rom size.");

	rom->read((char*)rom_banks, length);
	
	if(memory[0x143] == 0x80)
		gb_type = cgb_compatible;
	else if(memory[0x143] == 0xC0)
		gb_type = cgb_only;
	else
		gb_type = dmg;


	memory[0xFF4F] = 0;
	memory[0xFF4D] = 0;
	memory[0xFF55] = 0xFF;

	if(memory[0x147] > 0 && memory[0x147] <=3)
		mbc_type = mbc1;
	else if(memory[0x147] == 5 || memory[0x147] == 6)
		mbc_type = mbc2;
	else if(memory[0x147] > 0xE && memory[0x147] <= 0x13)
		mbc_type = mbc3;
	else if(memory[0x147] >= 0x19 && memory[0x147] < 0x1F)
		mbc_type = mbc5;
	else
		mbc_type = none;

	rom_size = length;
	ram_type = memory[0x149];

	if(ram_type !=0){
		if(ram_type > 4)
			throw std::runtime_error("Unsupported ram size.");
		uint32_t ram_types[4] = {2048, 8192, 32768, 131072};
		ram_size = ram_types[ram_type-1];
		external_ram = new uint8_t[ram_size];
		ex_ram = true;
	}
	else{
		ex_ram = false;
		ram_size = 0;
		}


	available_rom_banks = rom_size / 0x4000;
	available_ram_banks = ram_type < 3 ? 1 : ram_size / 0x2000;
	rom_connected = true;

	load_ram();


}


void _memory::disconnect_rom(){
	
	save_ram();

	std::memset((void*)memory, 0, 0x8000);
	delete rtc_registers;
	rtc_registers = nullptr;

	if(rom != nullptr){
		rom->close();
		rom = nullptr;
	}

	delete rom_banks;
	rom_banks = nullptr;

	delete external_ram;
	external_ram = nullptr;

	mbc_type = none;
	ex_ram = false;

	dma_time = false;
	dma_request = false;

	wram_bank = vram_bank = 0;
}

uint16_t _memory::curr_rom_bank() const{

	if(mbc_type == mbc1)
		return ((bank_select2 << 5) | bank_select1 ) % available_rom_banks;

	else if(mbc_type == mbc5)
		return ((bank_select2 << 8) | bank_select1) % available_rom_banks;
	
	else
		return bank_select1 % available_rom_banks;
	
}

uint8_t _memory::curr_ram_bank() const{

	if(mbc_type == mbc1){

		if(mode_select == 1)
			return bank_select2 % available_ram_banks;
		else
			return 0;
	}
	
	else if(mbc_type == mbc5)
		return bank_select3 % available_ram_banks;

	else
		return bank_select2 % available_ram_banks;

}




uint8_t& _memory::operator[](const uint16_t index){ 

	if(index <= 0x3FFF){

		if(mbc_type == mbc1){
			if(mode_select == 0)
				return memory[index];
			else
				return rom_banks[0x4000 * ((bank_select2 << 5) % available_rom_banks) + index];
		}
		else
			return memory[index];			
	}
	
	else if((index >= 0x4000 && index < 0x8000) && mbc_type != none){ //switchable rom bank area
		return rom_banks[0x4000 * curr_rom_bank() + index - 0x4000];
	}

	else if(index >= 0x8000 && index < 0xA000){
		if(vram_bank == 1)
			return cgb_vram[index - 0x8000];
		else
			return memory[index];
	}

	else if((index >= 0xA000 && index < 0xC000)){ //external ram area

		if(ram_enable && ex_ram){

			if(mbc_type == mbc3 && rtc_register >= 8)
				return rtc_registers[rtc_register - 8];

			else if(mbc_type == mbc2)
				return external_ram[(index - 0xA000) % 0x200];

			else
				return external_ram[curr_ram_bank() * 0x2000 + index - 0xA000];			
		}
		else
			return dummy;		
	}

	else if(index >= 0xD000 && index < 0xE000){ //switchable internal ram area
	
		if(gb_type != dmg && wram_bank >= 2)
			return cgb_wram[(wram_bank - 2) * 0x1000 + (index - 0xD000)];
		else
			return memory[index];

	}


	else if(index == 0xFF69 && gb_type != dmg)
		return bg_palette_ram[memory[0xFF68] & 0x3F];
	

	else if(index == 0xFF6B && gb_type != dmg)
		return ob_palette_ram[memory[0xFF6A] & 0x3F];

	else
		return memory[index]; 
}

const uint8_t& _memory::operator[](const uint16_t index) const {

	return this->operator[](index);
}

void _memory::write(const uint16_t index, const uint8_t value){

	if(dma_time && index < 0xFF80)
		return;

	if(index < 0x8000)
		write_to_mbc(index, value);

	else if(index >= 0x8000 && index < 0xA000){ //vram
		if(vram_bank == 0)
			memory[index] = value;
		else
			cgb_vram[index - 0x8000] = value;

	}

	else if((index >= 0xA000 && index < 0xC000))
		write_to_ex_ram(index, value);
	

	else if(index >= 0xD000 && index < 0xE000){
		if(gb_type != dmg && wram_bank > 1)
			cgb_wram[ (wram_bank - 2) * 0x1000 + (index - 0xD000) ] = value;
		
		else	
			memory[index] = value;
		//memory[0xE000 + index - 0xC000] = value;
	}

//	else if(index >= 0xE000 && index < 0xFE00){ //echo ram
//		memory[index] = value;
//		memory[0xC000 + index - 0xE000] = value;

//	}

	else if(index >= 0xFE00 && index < 0xFEA0){ //oam ram
		if((memory[0xFF41] & 0x3) < 0x2)
			memory[index] = value;
	}

	else if(index >= 0xFEA0 && index <= 0xFFFF)
		write_to_hram(index, value);

	else
		memory[index] = value;
}


void _memory::write_to_ex_ram(const uint16_t index, const uint8_t value){

	if(ram_enable && ex_ram){
		if(mbc_type == mbc3 && rtc_register >= 8)
			_rtc->write(rtc_register - 8, value);

		else if(mbc_type == mbc2)
			external_ram[(index - 0xA000) % 0x200] = value & 0xF;

		else
			external_ram[curr_ram_bank() * 0x2000 + index - 0xA000] = value;
		}

}

void _memory::write_to_hram(const uint16_t index, const uint8_t value){

	
	if(index == 0xFF00)
		memory[index] = value | 0xCF;

	if(index == 0xFF02){

		if(value & 1 << 7)
			serial_trigg = true;

		if(gb_type == dmg)
			memory[index] = value & ~(1 << 1);

		else
			memory[index] = value;

	}

	else if(index == 0xFF03 || index == 0xFF04){
		memory[0xFF03] = 0;
		memory[0xFF04] = 0;
	}

	else if(index == 0xFF14){
		if(value & 1 << 7)
			chan1_trigg = true;
		memory[index] = value;
	}

	else if(index == 0xFF19){
		if(value & 1 << 7)
			chan2_trigg = true;
		memory[index] = value;
	}

	else if(index == 0xFF1A){
		if(!(value & 1 << 7))
			memory[0xFF26] &= ~(1 << 2);
		else
			memory[0xFF26] |= 1 << 2;

		memory[index] = value;
	}

	else if(index == 0xFF1E){
		if(value & 1 << 7)
			chan3_trigg = true;
		memory[index] = value;
	}


	else if(index == 0xFF23){
		if(value & 1 << 7)
			chan4_trigg = true;
		memory[index] = value;
	}


	else if(index == 0xFF40){
		if(!(value & 1 << 7)){
			memory[0xFF44] = 0; //LY
			memory[0xFF41] &= ~0x3; //STAT
			}
		memory[index] = value;
	}

	else if(index == 0xFF41)
		memory[index] = value | 1 << 7;

	else if(index == 0xFF44)
		memory[index] = 0;
	
	else if(index == 0xFF46){

		dma_request = true;
		memory[index] = value;
	}


	else if(index == 0xFF4D){
		memory[index] = value & 0x7F | memory[index] & 1 << 7;
	}

	else if(index == 0xFF4F){

		if(gb_type != dmg){
			vram_bank = value & 0x1;
			memory[index] = value & 0x1;
		}
		else
			memory[index] = 0;
	}

	else if(index == 0xFF51 || index == 0xFF52){

		if(gb_type != dmg)
			hdma_src_trigger = true;
		memory[index] = value;
	
	}

	else if(index == 0xFF53 || index == 0xFF54){

		if(gb_type != dmg)
			hdma_dest_trigger = true;
		memory[index] = value;

	}

	else if(index == 0xFF55){
		
		if(gb_type != dmg)
			hdma_request = true;

		if(hblank_dma_time && !(value & 1 << 7)){
			hblank_dma_time = false;
			hdma_request = false;
			memory[index] = value | 1 << 7;
		}

		else
			memory[index] = value;
	}



	else if(index == 0xFF69){
		
		uint8_t idx = memory[0xFF68] & 0x3F;
		bg_palette_ram[idx] = value;
		memory[index] = value;
		if(memory[0xFF68] & 1 << 7)
			memory[0xFF68] = 0x80 | ((idx + 1) & 0x3F);
	
	}

	else if(index == 0xFF6B){

		uint8_t idx = memory[0xFF6A] & 0x3F;
		ob_palette_ram[idx] = value;
		memory[index] = value;
		if(memory[0xFF6A] & 1 << 7)
			memory[0xFF6A] = 0x80 | ((idx + 1) & 0x3F);
	
	}


	else if(index == 0xFF70){

		if(gb_type != dmg)
			wram_bank = value & 0x7;
		
		memory[index] = value;

	}

	else
		memory[index] = value;

}

void _memory::write_to_mbc(const uint16_t index, const uint8_t value){
	
	switch(mbc_type){

		case mbc1:{
			write_to_mbc1(index, value);
			break;
		}

		case mbc2:{
			write_to_mbc2(index, value);
			break;
		}

		case mbc3:{
			write_to_mbc3(index, value);
			break;
		}			

		case mbc5:{
			write_to_mbc5(index, value);
			break;
		}

		default:
			break;
	}

}


void _memory::write_to_mbc1(const uint16_t index, const uint8_t value){

	if(index <= 0x1FFF){
		if((value & 0xF) == 0xA)
			ram_enable = true;
		else
			ram_enable = false;
	}

	else if(index >= 0x2000 && index <= 0x3FFF){
		bank_select1 = (value & 0x1F);
		if((value & 0x1F) == 0)
			bank_select1 = 1;
	}

	else if(index >= 0x4000 && index <= 0x5FFF){
		bank_select2 = (value & 0x3);
	}

	else if(index >= 0x6000 && index <= 0x7FFF){
		if(value & 0x1)
			mode_select = 1;
		else
			mode_select = 0;
	}

}


void _memory::write_to_mbc2(const uint16_t index, const uint8_t value){

	if(index <= 0x3FFF){
		if(index & 1 << 8){
			bank_select1 = (value & 0xF);
			if(bank_select1 == 0)
				bank_select1 = 1;
		}
		else{
			if((value & 0xF) == 0xA)
				ram_enable = true;
			else
				ram_enable = false;
		}
	}

}


void _memory::write_to_mbc3(const uint16_t index, const uint8_t value){


	if(index <= 0x1FFF){
		if((value & 0xF) == 0xA)
			ram_enable = true;
		else
			ram_enable = false;
	}

	else if(index >= 0x2000 && index <= 0x3FFF){
		bank_select1 = (value & 0x7F);
		if((value & 0x7F) == 0)
			bank_select1 |= 1;
	}

	else if(index >= 0x4000 && index <= 0x5FFF){
		if( (value & 0xF) >= 0x8 && (value & 0xF) <= 0xC)
			rtc_register = value & 0xF;
		else{
			bank_select2 = (value & 0x3);
			rtc_register = 0;
		}
	}

	else if(index >= 0x6000 && index <= 0x7FFF){
		if(value == 0)
			latch = 1;
		else if(value == 1){
			if(latch){
				_rtc->latch();
				latch = 0;
			}
		}
				
	}

}


void _memory::write_to_mbc5(const uint16_t index, const uint8_t value){

	if(index <= 0x1FFF){
		if(value == 0xA)
			ram_enable = true;
		else
			ram_enable = false;
	}

	else if(index >= 0x2000 && index <= 0x2FFF){
		bank_select1 = value;
	}

	else if(index >= 0x3000 && index <= 0x3FFF){
		bank_select2 = (value & 0x1);
	}

	else if(index >= 0x4000 && index <= 0x5FFF)
		bank_select3 = (value & 0xF);
	
}


void _memory::save_ram(){

	if(!ex_ram)
		return;

	std::string save_name;
	std::size_t dot = _rom_name.find_last_of('.');

	save_name = _rom_name.substr(0, dot);
	save_name += ".sav";

	if(mbc_type == mbc3){
		std::string rtc_name = _rom_name.substr(0, dot) + ".rtc";
		
		std::fstream rtc(rtc_name.c_str(), std::ios::out | std::ios::binary);
		if(!rtc.is_open())
			throw std::runtime_error("RTC file creating error.");

		rtc.write((char*)rtc_registers, 5);
		int32_t* last_time_point = new int32_t;
		*last_time_point = _rtc->get_last_time_point();
		rtc.write((char*)last_time_point, sizeof(int32_t));
		rtc.close();
		delete last_time_point;
	}

	std::fstream save(save_name.c_str(), std::ios::out | std::ios::binary);
	if(!save.is_open())
		throw std::runtime_error("Save file creating error.");

	save.write((char*)external_ram, ram_size);
	save.close();

}

void _memory::load_ram(){

	if(!ex_ram)
		return;

	std::string save_name;
	std::size_t dot = _rom_name.find_last_of('.');

	save_name = _rom_name.substr(0, dot);
	save_name += ".sav";

	if(mbc_type == mbc3){
		std::string rtc_name = _rom_name.substr(0, dot) + ".rtc";
		
		std::fstream rtc(rtc_name.c_str(), std::ios::in | std::ios::binary);
		if(!rtc.is_open())
			return;

		rtc.seekg(0, rtc.end);
		unsigned int length = rtc.tellg();
		rtc.seekg(0, rtc.beg);

		if(length != 9)
			return;

		rtc.read((char*)rtc_registers, 5);
		int32_t* last_time_point = new int32_t;
		rtc.read((char*)last_time_point, sizeof(int32_t));
		_rtc->load_last_time_point(*last_time_point);
		rtc.close();
		delete last_time_point;
	}

	std::fstream save(save_name.c_str(), std::ios::in | std::ios::binary);
	if(!save.is_open())
		return;

	save.seekg(0, save.end);
	unsigned int length = save.tellg();
	save.seekg(0, save.beg);

	if(length != ram_size)
		return;

	save.read((char*)external_ram, ram_size);
	save.close();
}




