#ifndef memoryh
#define memoryh

#include<cstdint>
#include<fstream>
#include<iostream>

class _memory{

uint8_t* memory;
uint8_t rom_bank;
std::fstream* rom;

public:

_memory();
~_memory();
uint8_t& operator[](const uint16_t index);
const uint8_t operator[](const uint16_t index) const;

uint8_t read(const uint16_t index){ return memory[index]; }
void write(const uint16_t index, const uint8_t value);

};



#endif

