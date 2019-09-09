#include "lcd_driver.hh"

lcd_driver::lcd_driver(_memory* mem){
	lcd_registers = &(*mem)[LCD_REGS_MEMORY_ADDR];
	IF = &(*mem)[IF_REGISTER];

	vram1 = &(*mem)[0x8000];
	vram2 = &(*mem)[0x9000];
	chr_code1 = &(*mem)[0x9800];
	chr_code2 = &(*mem)[0x9C00];
	oam = &(*mem)[OAM];

	compare comp(mem);
	sprites_cont = new std::vector<uint8_t>;
	visible_sprites = new std::priority_queue<uint8_t, std::vector<uint8_t>, compare>(comp, *sprites_cont);
	pixel_fifo = new std::deque<uint8_t>;

	lcd_registers[STAT] &= 0xFC; //clear mode flags
	lcd_registers[STAT] |= mode_flags[0];
	mode = 0;
	LY_counter = 0;
	old_stat_signal = 0;
	oam_search_done = false;
	screen_buffer = new uint8_t*[144];

	for(int i = 0; i < 144; ++i){
		screen_buffer[i] = new uint8_t[160];
	}

	oam_debug_buffer = new uint8_t*[128];

	for(int i = 0; i < 144; ++i){
		oam_debug_buffer[i] = new uint8_t[40];
	}
}


lcd_driver::~lcd_driver(){
	delete[] screen_buffer;
	delete[] oam_debug_buffer;
	delete sprites_cont;
	delete visible_sprites;
}

void lcd_driver::update_sprite(const uint8_t oam_idx){

	sprite.size = lcd_registers[LCDC] & 1 << 2 ? 16 : 8;
	sprite.sx = oam[oam_idx*4+1];
	sprite.h_flip = oam[oam_idx*4 + 3] & 1 << 5 ? true : false;
	sprite.v_flip = oam[oam_idx*4 + 3] & 1 << 6 ? true : false;
	sprite.priority_over_bg = oam[oam_idx*4 + 3] & 1 << 7 ? false : true;
	uint8_t common_line = find_common_line(oam_idx);

	if(sprite.v_flip) 
		common_line = sprite.size - 1 - common_line;

	sprite.line = common_line % 8;

	if(sprite.size == 16){
		if(common_line < 8)
			sprite.tile = oam[oam_idx*4 + 2] & 0xFE;
		else
			sprite.tile = oam[oam_idx*4 + 2] | 0x1;
	}
	else
		sprite.tile = oam[oam_idx*4 + 2];


	sprite.palette = oam[oam_idx*4 + 3] & 1 << 4 ? &lcd_registers[OBP1] : &lcd_registers[OBP0];

	sprite.tile_data = &vram1[sprite.tile * 16 + sprite.line * 2];


}

void inline lcd_driver::draw_pixel(const uint8_t color){
	screen_buffer[lcd_registers[LY]][curr_px++] = color;
}

uint8_t inline lcd_driver::get_pixel(const uint8_t* pal, const uint8_t color){
	return (*pal & (0x3 << color * 2)) >> color*2;
}

uint8_t lcd_driver::get_color(const uint8_t* tile_data, const uint8_t px){
	uint8_t colorA, colorB;

	colorA = (tile_data[0] & 1 << (7-px)) ? 1 : 0;
	colorB = (tile_data[1] & 1 << (7-px)) ? 2 : 0;

	return (colorB | colorA);
}


uint8_t lcd_driver::get_color_rev(const uint8_t* tile_data, const uint8_t px){
	uint8_t colorA, colorB;

	colorA = (tile_data[0] & 1 << (px)) ? 1 : 0;
	colorB = (tile_data[1] & 1 << (px)) ? 2 : 0;

	return (colorB | colorA);
}



void lcd_driver::fill_fifo_bgwin(const uint8_t* bg_map, const uint8_t* bg_data, uint16_t block, const bool windowing){
		
	const uint8_t* block_line_data;
	uint8_t pal;

	uint8_t line;  
	if(windowing)
		line = (lcd_registers[LY] - lcd_registers[WY] ) % 8;
	else
		line = (lcd_registers[SCY] +lcd_registers[LY]) % 8;

	if(bg_data == vram2)
		block_line_data = &bg_data[(int8_t)(bg_map[block]) * 16 + line * 2];
	else
		block_line_data = &bg_data[bg_map[block] * 16 + line * 2];


	for(uint8_t i = 0; i < 8; ++i){
		pal = get_color(block_line_data, i);
		pixel_fifo->push_back(get_pixel(&lcd_registers[BGP], pal));
	}
}




void lcd_driver::fill_fifo_oam(const uint8_t oam_idx, const uint8_t shift = 0){
	
	update_sprite(oam_idx);

	uint8_t pal;

	for(uint8_t i = shift; i < 8; ++i){

		if(sprite.h_flip)
			pal = get_color_rev(sprite.tile_data, i);
		else
			pal = get_color(sprite.tile_data, i);

		if(sprite.priority_over_bg){ 
			if(pal != 0){
				if( (pixel_fifo->at(i-shift) <= 0x3) || ((pixel_fifo->at(i-shift) & 0xFC) >> 2) > oam_idx ) //this pixel is bg pixel or sprite pixel with lower priority
					pixel_fifo->at(i-shift) = get_pixel(sprite.palette, pal) | (oam_idx << 2);
			}
		}
		else{
			if((pixel_fifo->at(i-shift) & 0x3) == (lcd_registers[BGP] & 0x3))
				pixel_fifo->at(i-shift) = get_pixel(sprite.palette, pal) | (oam_idx << 2);

		}
	}

	visible_sprites->pop();
}



void lcd_driver::search_oam(){

	while(!visible_sprites->empty())
		visible_sprites->pop(); //it should always be empty, but it's different...

	if(!(lcd_registers[LCDC] & 1 << 1)) // obj off
		return;
	
	uint8_t sprite_size = lcd_registers[LCDC] & 1 << 2 ? 16 : 8;
	uint8_t counter = 0;
	

	for(uint8_t i = 0; i < OAM_SIZE; i+=4){
		if((lcd_registers[LY] >= (oam[i] - 16)) && (lcd_registers[LY] < (oam[i] + sprite_size - 16))){
			visible_sprites->push(i/4);
			++counter;
			if(counter == 10)
				break;
		}
	}

	

}
			

uint8_t lcd_driver::find_common_line(const uint8_t oam_index){
	int16_t top_line = oam[oam_index*4] - 16;
	uint8_t sprite_size = lcd_registers[LCDC] & 1 << 2 ? 16 : 8;
	uint8_t common_line = 0;

	for(uint8_t i = 0; i < sprite_size; ++i){
		if( lcd_registers[LY] == top_line + i ){
			common_line = i;
			break;
		}
	}

	return common_line;

}



void lcd_driver::draw_blank_line(){
	for(int i = 0; i < 160; ++i){
		screen_buffer[lcd_registers[LY]][i] = 0;
	}
}


	
void lcd_driver::draw_line(){
	
	mode_counter[mode] += (10 - visible_sprites->size()) * cycles_per_sprite;
	cycles_to_add = visible_sprites->size() * cycles_per_sprite;

	bool windowing = false;
	uint8_t* bg_tile_data = (lcd_registers[LCDC] & 1 << 4) ? vram1 : vram2;
	uint8_t* bg_tile_nums =  (lcd_registers[LCDC] & 1 << 3) ? chr_code2 : chr_code1;
	uint8_t* window_tile_nums = (lcd_registers[LCDC] & 1 << 6) ? chr_code2 : chr_code1;
	uint16_t block = ( ((lcd_registers[SCY] + lcd_registers[LY]) % 256) / 8) * 32 + lcd_registers[SCX]/8;
	uint16_t row = (block/32 + 1)*32;
	uint8_t line =  (lcd_registers[SCY] +lcd_registers[LY]) % 8;
	uint8_t* block_line_data;
	uint8_t* curr_nums;
	uint8_t oam_idx;
	uint8_t oam_x;
	uint8_t x_off = lcd_registers[SCX] % 8;
	uint8_t length = 32;
	curr_px = 0;
	curr_nums = bg_tile_nums;
	pixel_fifo->clear();

	fill_fifo_bgwin(curr_nums, bg_tile_data, block, windowing);
	
	for(uint8_t i = 0; i < x_off; ++i){
		pixel_fifo->pop_front();
	}

	while(curr_px < 160){


		if(pixel_fifo->size() < 9){ //fetch
			++block;
			if(block == row)
				block = row - 32;
			fill_fifo_bgwin(curr_nums, bg_tile_data, block, windowing);
		}

		if(!windowing){
			if(lcd_registers[LCDC] & 1 << 5){ //window enable
				if((curr_px >= lcd_registers[WX] - 7) && (lcd_registers[WY] <= lcd_registers[LY])){
					pixel_fifo->clear();
					curr_nums = window_tile_nums;
					block = ((lcd_registers[LY] - lcd_registers[WY])/8) * 32;
					windowing = true;
					fill_fifo_bgwin(curr_nums, bg_tile_data, block, windowing);

				}

			}

		}
	  
		if(!visible_sprites->empty()){ //there are objs to draw	
			oam_idx = visible_sprites->top();
			oam_x = oam[oam_idx*4+1];
			while(oam_x < 8 && !visible_sprites->empty()){ //draw sprites partially hidden
				fill_fifo_oam(oam_idx, 8 - oam_x);
				oam_idx = visible_sprites->top();
				oam_x = oam[oam_idx*4+1];
			}
			while(curr_px == oam_x - 8 && !visible_sprites->empty() ){ //draw entire sprites
				fill_fifo_oam(oam_idx);
				oam_idx = visible_sprites->top();
				oam_x = oam[oam_idx*4+1];
			}
			
		}

		draw_pixel(pixel_fifo->front() & 0x3); //push
		pixel_fifo->pop_front();
		

	}

}
		

void lcd_driver::debug_draw_oam(){

	if(lcd_registers[LY] >= 128)
		return;

	uint8_t sprite_size = lcd_registers[LCDC] & 1 << 2 ? 16 : 8;
	uint8_t base = (lcd_registers[LY] / 16) * 5;
	uint8_t block = (lcd_registers[LY] / 8) % 2 == 0 ? 0 : 1;
		
	uint8_t* curr_pal;
	uint8_t* tile;
	uint8_t color;

	for(uint8_t i = 0; i < 5; ++i){
		curr_pal = oam[(base + i)*4 + 3]  & 1 << 4 ? &lcd_registers[OBP1] : &lcd_registers[OBP0];
		if(block == 1 && sprite_size == 8)
			tile = blank_tile;
		else
			tile = &vram1[ (oam[(base + i)*4 + 2] + block) * 16 + (lcd_registers[LY]%8)*2];

		for(uint8_t j = 0; j < 8; ++j){
			color = get_color(tile, j);
			oam_debug_buffer[lcd_registers[LY]][i*8 + j] = get_pixel(curr_pal, color);
		}
	}

}
	

void lcd_driver::check_for_interrupts(){

	uint8_t new_stat_signal = 0;

	if(lcd_registers[LY] == lcd_registers[LYC]){
		lcd_registers[STAT] |= LYC_match_flag;
 		if(lcd_registers[STAT] & LYC_interrupt_select)
			new_stat_signal |= 1;	
	}
	else
		lcd_registers[STAT] &= ~LYC_match_flag;

	if(lcd_registers[STAT] & interrupt_selection_flags[mode]) 
		new_stat_signal |= 2;

	if(old_stat_signal == 0 && new_stat_signal != 0)
		generate_interrupt(mode_x);

	old_stat_signal = new_stat_signal;

}



void lcd_driver::generate_interrupt(interrupt_type t){
	
	if(t == vblank)
		*IF |= VBLANK_INT;

	else if(t == mode_x || t == lyc_eq_ly)
		*IF |= LCD_STAT_INT;
	
}


void lcd_driver::inc_LY(){

	++lcd_registers[LY];
	if(lcd_registers[LY] == scanlines_end){
		lcd_registers[LY] = 0;
		}

}

void lcd_driver::switch_mode(){


	switch(mode){

		case 0:{
			oam_search_done = false;
			mode = 1; //drawing
			draw_line();
			debug_draw_oam();
			break;
		}

		case 1:{
			mode = 2; //hblank
			mode_counter[mode] += cycles_to_add;
			cycles_to_add = 0;
			break;
		}

		case 2:{
			inc_LY();
			if(lcd_registers[LY] == scanlines_until_vblank){ //vblank time!
				LY_counter = 0;
				mode = 3; 
				generate_interrupt(vblank);
			}
			else
				mode = 0; //nope, it's not vblank yet
			break;
		}

		case 3:{
			if(lcd_registers[LY] != 0 )
				lcd_registers[LY] = 0;
			mode = 0;
			break;
		}
		
		default: 
			break;

	}
	lcd_registers[STAT] &= 0xFC; //clear first 2 bits
	lcd_registers[STAT] |= mode_flags[mode]; 
	
}



void lcd_driver::update(uint8_t cycles){

	if(!(lcd_registers[LCDC] & 1 << 7)){
		return;
	}

	switch(mode){

		case 0:{
			
			if(oam_search_done == false){	
				search_oam();
				oam_search_done = true;
			}
			break;
		}
		case 3:{
			LY_counter += cycles;
			if(LY_counter >= cycles_per_scanline){
				inc_LY();
				if(lcd_registers[LY] == 153)
					LY_counter = 452;
				else
					LY_counter = 0;
			}
			break;
		}
		
		default:
			break;
	}


	mode_counter[mode] += cycles;

	if(mode_counter[mode] >= mode_cycles[mode]){
		mode_counter[mode] = 0;	
		switch_mode();

	}

	check_for_interrupts();
			
}


	

