#ifndef lcd_driverh
#define lcd_driverh

#include<cstdint>
#include<iostream>
#include<fstream>
#include<queue>
#include<functional>

#include "defines.h"
#include "memory.hh"
#include "compare.hh"

#define LCD_REGS_MEMORY_ADDR 0xFF40


struct _sprite{

	uint8_t size;
	uint8_t beg;
	uint8_t end;
	uint8_t offset;
	uint8_t sx;
	uint8_t tile;
	uint8_t* palette;
	uint8_t* tile_data;
	bool h_flip;
	bool v_flip;

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

	bool oam_search_done;
	uint8_t* lcd_registers;
	uint8_t* IF;
	uint8_t* vram1;
	uint8_t* vram2;
	uint8_t* chr_code1;
	uint8_t* chr_code2;
	uint8_t* oam;
	uint8_t* line_buffer;
	uint8_t** screen_buffer;
	std::vector<uint8_t>* sprites_cont;
	std::priority_queue<uint8_t, std::vector<uint8_t>, compare>* visible_sprites;
	_sprite sprite;

	uint16_t mode_counter[4] = {0};
	uint16_t mode_cycles[4] = {80, 172, 204, 4560};
	uint8_t mode_flags[4] = {2,3,0,1};
	uint8_t interrupt_selection_flags[4] = {(1 << 5), 0, (1 << 3), (1 << 4)};

	uint8_t mode;
	uint16_t LY_counter;


	void generate_interrupt(interrupt_type t);
	void inc_LY();
	void switch_mode();

	uint8_t find_common_line(const uint8_t oam_index);
	
	uint8_t curr_px;
	void draw_blank_line();
	void draw_line();
	void draw_sprite_line(const uint8_t oam_idx, const uint8_t* block_line_data, const uint8_t block, const uint8_t blocks_to_draw);
	void draw_bg_line(const uint8_t* block_line_data, const uint8_t block, const uint8_t blocks_to_draw);
	void draw_pixel(const uint8_t* pal, const uint8_t color);
	uint8_t get_color(const uint8_t* tile_data, const uint8_t px);
	uint8_t get_color_rev(const uint8_t* tile_data, const uint8_t px);
	void search_oam();
 
	void update_sprite(const uint8_t oam_idx, const uint8_t block);
	

//	bool compare(uint8_t A, uint8_t B){ return oam[4*A+1] > oam[4*B+1]; }

public:

	lcd_driver(_memory& mem);
	~lcd_driver();
	void update(const uint8_t cycles);
	uint8_t debug(){ return lcd_registers[LY]; }
	uint8_t** const screen(){ return screen_buffer; }


};






#endif
