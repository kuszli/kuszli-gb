#ifndef dmah
#define dmah

#include<cstdint>
#include<cstring>
#include "memory.hh"
#include "defines.h"
#include<iostream>

#define DMA_ADDR 0xFF46


class dma{

	const uint16_t transfer_time = 640;
	uint16_t counter;
	bool dma_time;
	bool hblank_dma_time;
	uint16_t hdma_src;
	uint16_t hdma_dest;
	uint8_t hdma_len;
	uint8_t hdma_mode;
	uint8_t last_lcd_mode; //for hblank dma
	uint16_t hdma_counter;

	_memory* memory;
	uint8_t* dma_register;
	uint8_t* oam;

	void transfer();
	void transfer_cgb();

public:
	
	dma(_memory* mem);
	~dma();
	bool hdma_transfer;
	uint8_t transfer_size;
	void update(uint8_t cycles);

};



#endif
