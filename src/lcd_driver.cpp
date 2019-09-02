#include "lcd_driver.hh"

lcd_driver::lcd_driver(_memory& mem){
	lcd_registers = &mem[LCD_REGS_MEMORY_ADDR];
	IF = &mem[IF_REGISTER];

	vram1 = &mem[0x8000];
	vram2 = &mem[0x9000];
	chr_code1 = &mem[0x9800];
	chr_code2 = &mem[0x9C00];
	oam = &mem[OAM];

	compare comp(mem);
	sprites_cont = new std::vector<uint8_t>;
	visible_sprites = new std::priority_queue<uint8_t, std::vector<uint8_t>, compare>(comp, *sprites_cont);

	lcd_registers[STAT] &= 0xFC; //clear mode flags
	lcd_registers[STAT] |= mode_flags[0];
	mode = 0;
	LY_counter = 0;
	oam_search_done = false;
	screen_buffer = new uint8_t*[144];

	for(int i = 0; i < 144; ++i){
		screen_buffer[i] = new uint8_t[160];
	}
}


lcd_driver::~lcd_driver(){
	delete[] screen_buffer;
	delete sprites_cont;
	delete visible_sprites;
}


void lcd_driver::search_oam(){
	if(!(lcd_registers[LCDC] & 1 << 1) || !(lcd_registers[LCDC] & 1 << 7)) // obj off or screen off
		return;

	while(!visible_sprites->empty())
		visible_sprites->pop(); //it should always be empty, but it's different...

	uint8_t sprite_size = lcd_registers[LCDC] &  1 << 2 ? 16 : 8;
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

void lcd_driver::draw_sprite_line(const uint8_t oam_idx, const uint8_t* block_line_data, const uint8_t block){
	if((oam[oam_idx*4+1] == 0) || (oam[oam_idx*4+1] >=160)){
		visible_sprites->pop();
		return;
	}

	uint8_t common_line = find_common_line(oam_idx);
	uint8_t sprite_size = lcd_registers[LCDC] & 1 << 2 ? 16 : 8;
	uint8_t tile;
	uint8_t* palette = oam[oam_idx*4 + 3] & 1 << 4 ? &lcd_registers[OBP1] : &lcd_registers[OBP0];

	uint8_t colorA, colorB, tmp_pal;
	uint8_t sprite_beg, sprite_end, sprite_offset;
	uint8_t sprite_counter = 0;
	uint8_t sx = oam[oam_idx*4 + 1];

	bool h_flip = oam[oam_idx*4 + 3] & 1 << 5 ? true : false;
	bool v_flip = oam[oam_idx*4 + 3] & 1 << 6 ? true : false;

	if(v_flip) 
		common_line = sprite_size - 1 - common_line;

	uint8_t line = common_line % 8;

	if(sprite_size == 16){
		if(common_line < 8)
			tile = oam[oam_idx*4 + 2] & 0xFE;
		else
			tile = oam[oam_idx*4 + 2] | 0x1;
	}
	else
		tile = oam[oam_idx*4 + 2];

	uint8_t* tile_data = &vram1[tile * 16 + line * 2];
	

	if(sx - 8 <= block*8){
		sprite_beg = 0;
		sprite_end = sx - block*8;
		sprite_offset = 8 - (sx - block*8);
	}
	else{
		sprite_beg = sx - 8 - block*8;
		sprite_end = block*8 + 8;
		sprite_offset = 0;
	}

	for(int j = 0; j < 8; ++j){

		if(j >= sprite_beg && j < sprite_end){
			if(h_flip){
				colorA = (tile_data[0] & 1 << (sprite_offset + sprite_counter)) ? 1 : 0;
				colorB = (tile_data[1] & 1 << (sprite_offset + sprite_counter)) ? 2 : 0;
			}
			else{
			colorA = (tile_data[0] & 1 << (7-(sprite_offset + sprite_counter))) ? 1 : 0;
			colorB = (tile_data[1] & 1 << (7-(sprite_offset + sprite_counter))) ? 2 : 0;
			}
			tmp_pal = colorB | colorA;	
			++sprite_counter;
			if(tmp_pal == 0)
				screen_buffer[lcd_registers[LY]][block*8 + j] = lcd_registers[BGP] & 0x3;
			else
				screen_buffer[lcd_registers[LY]][block*8 + j] = (*palette & (0x3 << tmp_pal * 2)) >> tmp_pal*2 ;
		}

		else{
			colorA = (block_line_data[0] & 1 << (7-j)) ? 1 : 0;
			colorB = (block_line_data[1] & 1 << (7-j)) ? 2 : 0;
			tmp_pal = colorB | colorA;	
			screen_buffer[lcd_registers[LY]][block*8 + j] = (lcd_registers[BGP] & (0x3 << tmp_pal*2)) >> tmp_pal*2;
		}

			
	}

	if((sx - 1 < block*8 + 8) || block == 19) //end of sprite or end of line
		visible_sprites->pop();
		
	
}



void lcd_driver::draw_bg_line(const uint8_t* block_line_data, const uint8_t block){

	uint8_t colorA, colorB, tmp_pal;

	for(int j = 0; j < 8; ++j){
		colorA = (block_line_data[0] & 1 << (7-j)) ? 1 : 0;
		colorB = (block_line_data[1] & 1 << (7-j)) ? 2 : 0;
		tmp_pal = colorB | colorA;	
		screen_buffer[lcd_registers[LY]][block*8 + j] = (lcd_registers[BGP] & (0x3 << tmp_pal*2)) >> tmp_pal*2;
		}

}


	
void lcd_driver::draw_line(){

	if(!(lcd_registers[LCDC] & 1 << 7)){ //lcd off
		draw_blank_line();
		return;
	}

	bool sign;
	uint8_t* bg_tile_data;

	if(lcd_registers[LCDC] & 1 << 4){
		bg_tile_data = vram1;
		sign = false;
	}
	else{
		bg_tile_data = vram2;
		sign = true;	
	}

	uint8_t* bg_tile_nums =  (lcd_registers[LCDC] & 1 << 3) ? chr_code2 : chr_code1;
	uint16_t block = (((lcd_registers[SCY] + lcd_registers[LY]) % 255) / 8) * 32 + lcd_registers[SCX]/8;
	uint16_t row = (block/32 + 1)*32;
	uint8_t line = lcd_registers[LY] % 8; //(lcd_registers[SCY] + lcd_registers[LY]) % 8;
	uint8_t* block_line_data;
	uint8_t oam_idx;
	uint8_t oam_x;
	uint8_t oam_y;
	
//	std::cout << "sprites before loop: " << visible_sprites->size() << std::endl;
	for(int i = 0; i < 20; ++i){
		if(sign){
			block_line_data = &bg_tile_data[(int8_t)(bg_tile_nums[block]) * 16 + line * 2];
		}
		else
			block_line_data = &bg_tile_data[bg_tile_nums[block] *16 + line * 2];

		if(!visible_sprites->empty()){
			
			oam_idx = visible_sprites->top();
			oam_x = oam[oam_idx*4+1];
			if(((i*8 <= oam_x - 8) && ((i+1)*8+8 > oam_x - 1)) || (((i-1)*8 <= oam_x - 8) && (i*8 + 8 > oam_x))){
				draw_sprite_line(oam_idx, block_line_data, i);
	//			std::cout << visible_sprites->size() << std::endl;
					
			}
			else
				draw_bg_line(block_line_data, i);
		}

		else
			draw_bg_line(block_line_data, i);

		++block;
		if(block == row)
			block = row - 32;
	//	else
			//block = (block % row);
		
	}
//std::cout << "sprites after loop: " << visible_sprites->size() << std::endl;

}
		


void lcd_driver::generate_interrupt(interrupt_type t){
	
	if(t == vblank)
		*IF |= VBLANK_INT;

	else if(t == mode_x){
		if(lcd_registers[STAT] & interrupt_selection_flags[mode])
			*IF |= LCD_STAT_INT;
	}

	else{ //LY = LYC
		if(lcd_registers[STAT] & LYC_interrupt_select)
			*IF |= LCD_STAT_INT;
	}

	
}


void lcd_driver::inc_LY(){

	++lcd_registers[LY];
	if(lcd_registers[LY] == scanlines_end){
		lcd_registers[LY] = 0;
			}
	
	if(lcd_registers[LY] == lcd_registers[LYC]){
		lcd_registers[STAT] |= LYC_match_flag;
		generate_interrupt(lyc_eq_ly);
	}
		
}

void lcd_driver::switch_mode(){


	switch(mode){

		case 0:{
			oam_search_done = false;
			draw_line();
			mode = 1; //drawing
			break;
		}

		case 1:{
			mode = 2; //hblank
			generate_interrupt(mode_x);
			break;
		}

		case 2:{
			inc_LY();
			if(lcd_registers[LY] == scanlines_until_vblank){ //vblank time!
				LY_counter = 0;
				mode = 3; 
				generate_interrupt(vblank);
				generate_interrupt(mode_x);
			}
			else
				mode = 0; //nope, it's not vblank yet
				break;
		}

		case 3:{
			if(lcd_registers[LY] != 0)
				inc_LY();
			mode = 0;
			generate_interrupt(mode_x);
			break;
		}
		
		default: 
			break;

	}
	lcd_registers[STAT] &= 0xFC; //clear first 2 bits
	lcd_registers[STAT] |= mode_flags[mode]; 
	
}



void lcd_driver::update(uint8_t cycles){

	switch(mode){

		case 0:{
			
			if(oam_search_done == false){
				
				search_oam();
				oam_search_done = true;
			}
			break;
		}
		case 3:{
			//std::cout << (int)lcd_registers[LY] << " " << LY_counter << " " << mode_counter[mode] <<  std::endl;
			LY_counter += cycles;
			if(LY_counter >= cycles_per_scanline){
				inc_LY();
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
			
}


	

