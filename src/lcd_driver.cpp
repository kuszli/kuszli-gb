#include "lcd_driver.hh"

lcd_driver::lcd_driver(_memory& mem){
	lcd_registers = &mem[LCD_REGS_MEMORY_ADDR];
	IF = &mem[IF_REGISTER];

	vram1 = &mem[0x8000];
	vram2 = &mem[0x9000];
	chr_code1 = &mem[0x9800];
	chr_code2 = &mem[0x9C00];

	lcd_registers[STAT] |= mode_flags[0];
	mode = 0;
	LY_counter = 0;
	line_buffer = new uint8_t[160];
	screen_buffer = new uint8_t*[144];

	for(int i = 0; i < 144; ++i){
		screen_buffer[i] = new uint8_t[160];
	}
}


lcd_driver::~lcd_driver(){
	delete[] line_buffer;
	delete[] screen_buffer;
}

void lcd_driver::draw_blank_line(){
	for(int i = 0; i < 160; ++i){
		screen_buffer[lcd_registers[LY]][i] = 0;
	}
}

	
void lcd_driver::draw_line(){

	if(!(lcd_registers[LCDC] & 1 << 7)){
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
	uint16_t block = (lcd_registers[SCY]/8 + lcd_registers[LY]/8) * 32 + lcd_registers[SCX]/8;
	uint16_t row = (block/32 + 1)*32;
	uint8_t line = lcd_registers[LY] % 8;
	uint8_t* block_line_data;
	uint8_t colorA,colorB; //color bits
	uint8_t tmp_pal;

	for(int i = 0; i < 20; ++i){
		if(sign)
			block_line_data = &bg_tile_data[(int16_t)(bg_tile_nums[block] *16 + line * 2)];
		
		else
			block_line_data = &bg_tile_data[bg_tile_nums[block] *16 + line * 2];

		for(int j = 0; j < 8; ++j){
			colorA = (block_line_data[0] & 1 << (7-j)) ? 1 : 0;
			colorB = (block_line_data[1] & 1 << (7-j)) ? 2 : 0;
			tmp_pal = colorB | colorA;	
			screen_buffer[lcd_registers[LY]][i*8 + j] = (lcd_registers[BGP] & (0x3 << tmp_pal*2)) >> tmp_pal*2;
		}
		++block;
		block = block % row;
		
	}

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


	

