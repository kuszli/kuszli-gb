#ifndef memoryh
#define memoryh

#include<cstdint>
#include<fstream>
#include<iostream>
#include<string>
#include<cmath>

enum _mbc_type{
	none,
	mbc1,
	mbc2,
	mbc3,
	mbc5,
};


enum _gb_type{
	dmg,
	cgb_compatible,
	cgb_only,
	sgb,
	sgb2,
};

class _memory{

	uint8_t* memory;
	uint8_t* rom_banks;
	uint8_t* external_ram;
	uint8_t* cgb_wram;
	uint8_t* cgb_vram;
	uint8_t wram_bank;
	uint8_t vram_bank;

	std::fstream* rom;

	std::string _rom_name;
	uint8_t latch;
	uint8_t dummy;
	uint8_t ram_type;	
	bool rom_connected;
	bool mbc;
	bool ex_ram;
	bool ram_enable;
	uint8_t mode_select;
	uint16_t available_rom_banks;
	uint8_t available_ram_banks;
	_mbc_type mbc_type;
	_gb_type gb_type;
	uint32_t rom_size;
	uint32_t ram_size;
	uint8_t bank_select1;
	uint8_t bank_select2;
	uint8_t bank_select3;
	uint8_t rtc_register;
	uint8_t* rtc_registers;

	uint16_t curr_rom_bank() const;
	uint8_t curr_ram_bank() const;
	void write_to_ex_ram(const uint16_t index, const uint8_t value);
	void write_to_hram(const uint16_t index, const uint8_t value);
	void write_to_mbc(const uint16_t index, const uint8_t value);
	void write_to_mbc1(const uint16_t index, const uint8_t value);
	void write_to_mbc2(const uint16_t index, const uint8_t value);
	void write_to_mbc3(const uint16_t index, const uint8_t value);
	void write_to_mbc5(const uint16_t index, const uint8_t value);


public:

	_memory();
	~_memory();

	bool dma_request;
	bool dma_time;
	bool chan1_trigg;
	bool chan2_trigg;
	bool chan3_trigg;
	bool chan4_trigg;

	uint8_t& operator[](const uint16_t index);
	const uint8_t& operator[](const uint16_t index) const;
	void connect_rom(const std::string&);
	void disconnect_rom();
	uint8_t read(const uint16_t index){ return memory[index];}
	void write(const uint16_t index, const uint8_t value);
	void save_ram();
	void load_ram();

};


#endif

