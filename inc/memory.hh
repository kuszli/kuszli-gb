#ifndef memoryh
#define memoryh

#include<cstdint>
#include<fstream>
#include<iostream>
#include<string>

class _memory{

uint8_t* memory;
uint8_t* rom_banks;
std::fstream* rom;
uint8_t curr_bank;
uint8_t dummy;
bool mbc;

public:

_memory(const std::string& rom_name);
_memory(){}
~_memory();

uint8_t& operator[](const uint16_t index);
const uint8_t& operator[](const uint16_t index) const;

uint8_t read(const uint16_t index){ return memory[index]; }
void write(const uint16_t index, const uint8_t value);

};



#endif

