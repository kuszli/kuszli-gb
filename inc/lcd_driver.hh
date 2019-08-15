#ifndef lcd_driverh
#define lcd_driverh

#include<cstdint>
#include "defines.h"

#define LCD_REGS_MEMORY_ADDR 0xFF40

#define LCDC 0 //0xFF40
#define STAT 1 //0xFF41
#define SCY  2 //0xFF42
#define SCX  3 //0xFF43
#define LY   4 //0xFF44
#define LYC  5 //0xFF45
#define DMA  6 //0xFF46
#define BGP  7 //0xFF47
#define OBP0 8 //0xFF48
#define OBP1 9 //0xFF49
#define WY   10 //0xFF4A
#define WX   11 //0xFF4B
#define OAM  0xFE00
#define OAM_SIZE 0xA0


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

	uint16_t mode_counter[4] = {0};
	uint16_t mode_cycles[4] = {80, 172, 204, 4560};
	uint8_t mode_flags[4] = {2,3,0,1};
	uint8_t interrupt_selection_flags[4] = {(1 << 5), 0, (1 << 3), (1 << 4)};

	uint8_t mode;
	uint16_t LY_counter;

	void generate_interrupt(interrupt_type t);
	void inc_LY();
	void switch_mode();


public:

	lcd_driver(uint8_t* mem);
	void update(const uint8_t cycles);
	uint8_t debug(){ return lcd_registers[LY]; }


};






#endif
