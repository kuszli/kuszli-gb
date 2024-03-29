#ifndef lcd_driverh
#define lcd_driverh

#include<cstdint>
#include<iostream>
#include<fstream>
#include<queue>
#include<deque>
#include<functional>

#include "defines.h"
#include "memory.hh"
#include "compare.hh"

#define LCD_REGS_MEMORY_ADDR 0xFF40


struct _sprite{

	uint8_t size;
	uint8_t line;
	uint8_t sx;
	uint8_t tile;
	uint8_t* palette;
	uint8_t* tile_data;
	bool h_flip;
	bool v_flip;
	bool priority_over_bg;
};

enum pixel_type{
	bg,
	obj,
};

struct pixel{
	uint16_t value;
	uint8_t x_coord;
	uint8_t oam_idx;
	uint8_t color_no;
	bool priority_over_obj;
	pixel_type type;
	pixel(uint16_t v, uint8_t no, pixel_type t): value(v), x_coord(0), oam_idx(0), color_no(no), type(t){}
	void create_sprite_px(uint16_t v, uint8_t x, uint8_t id, uint8_t no) { value = v; x_coord = x; oam_idx = id; color_no = no; type = obj; }
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
	const uint8_t cycles_per_sprite = 12;
	const uint8_t LYC_match_flag = 1 << 2;
	const uint8_t LYC_interrupt_select = 1 << 6;

	uint8_t* lcd_registers;
	uint8_t* IF;
	uint8_t** vram1;
	uint8_t** vram2;
	uint8_t** chr_code1;
	uint8_t** chr_code2;
	uint8_t* cgb_vram;
	uint8_t* oam;
	uint8_t* line_buffer;
	uint8_t* screen_buffer;
	uint8_t* cgb_bg_pal;
	uint8_t* cgb_ob_pal;
	uint8_t** oam_debug_buffer;
	std::vector<uint8_t>* sprites_cont;
	std::priority_queue<uint8_t, std::vector<uint8_t>, compare>* visible_sprites;
	std::deque<pixel>* pixel_fifo;
	_sprite sprite;

	int16_t mode_counter[4] = {0};
	uint16_t mode_cycles[4] = {80, 288, 208, 4560};
	uint8_t mode_flags[4] = {2,3,0,1};
	uint8_t interrupt_selection_flags[4] = {(1 << 5), 0, (1 << 3), (1 << 4)};
	uint8_t blank_tile[16] = {0};

	bool dbg;
	bool oam_search_done;
	uint8_t mode;
	uint8_t cycles_to_add;
	uint16_t LY_counter;
	uint8_t old_stat_signal;
	uint8_t curr_px;

	_gb_type gb_type;

	void generate_interrupt(interrupt_type t);
	void check_for_interrupts();
	void inc_LY();
	void switch_mode();
	uint8_t find_common_line(const uint8_t oam_index);
	void draw_blank_line();
	void draw_line();
	void draw_sprite_line(const uint8_t oam_idx, const uint8_t* block_line_data, const uint8_t block, const uint8_t blocks_to_draw);
	void draw_bg_line(const uint8_t* block_line_data, const uint8_t block, const uint8_t blocks_to_draw);
	void draw_pixel(const uint16_t color);
	uint8_t get_pixel_mono(const uint8_t* pal, const uint8_t color);
	uint16_t get_pixel_rgb(const uint8_t* pal, const uint8_t color);
	uint8_t get_palette_index(const uint8_t* tile_data, const uint8_t px);
	uint8_t get_palette_index_rev(const uint8_t* tile_data, const uint8_t px);
	void search_oam();
 	void fill_fifo_bgwin(const uint8_t* const* bg_map, const uint8_t* const* bg_data, uint16_t block, const bool windowing);
	void fill_fifo_oam(const uint8_t oam_idx, const uint8_t shift);
	void update_sprite(const uint8_t oam_idx);
	void update_mode();
	void debug_draw_oam();


public:

	lcd_driver(_memory* mem);
	~lcd_driver();
	void set_gb_type(_gb_type t) { gb_type = t; }
	void update(const uint8_t cycles);
	uint8_t debug(){ return lcd_registers[LY]; }
	void set_oam_viewer(bool opt){ dbg = opt; }
	uint8_t* const screen(){ return screen_buffer; }
	uint8_t** const oam_screen(){ return oam_debug_buffer; }

};


#endif
