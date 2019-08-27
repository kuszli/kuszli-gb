#ifndef lcd_driverh
#define lcd_driverh

#include<cstdint>
#include<iostream>
#include<fstream>
#include "defines.h"
#include "memory.hh"

#define LCD_REGS_MEMORY_ADDR 0xFF40


struct pixel{
	uint8_t r;
	uint8_t g;
	uint8_t b;
};

enum interrupt_type{
	vblank,
	mode_x,
	lyc_eq_ly,
};
 
class lcd_driver{

	const uint16_t cycles_per_scanline = 456;
	const uint8_t scanlines_until_vblank = 144;
	const uint8_t scanlines_end = 154;	
	const uint8_t LYC_match_flag = 1 << 2;
	const uint8_t LYC_interrupt_select = 1 << 6;

	uint8_t* lcd_registers;
	uint8_t* IF;
	uint8_t* vram1;
	uint8_t* vram2;
	uint8_t* chr_code1;
	uint8_t* chr_code2;
	uint8_t* line_buffer;
	uint8_t** screen_buffer;

	uint16_t mode_counter[4] = {0};
	uint16_t mode_cycles[4] = {80, 172, 204, 4560};
	uint8_t mode_flags[4] = {2,3,0,1};
	uint8_t interrupt_selection_flags[4] = {(1 << 5), 0, (1 << 3), (1 << 4)};

	uint8_t mode;
	uint16_t LY_counter;


	void generate_interrupt(interrupt_type t);
	void inc_LY();
	void switch_mode();

	uint16_t get_bg_origin();
	
	void draw_line();
	void draw_blank_line();

public:

	lcd_driver(_memory& mem);
	~lcd_driver();
	void update(const uint8_t cycles);
	uint8_t debug(){ return lcd_registers[LY]; }
	uint8_t** const screen(){ return screen_buffer; }


};






#endif
