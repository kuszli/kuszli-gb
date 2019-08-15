#include "lcd_driver.hh"

lcd_driver::lcd_driver(uint8_t* mem){
	lcd_registers = &mem[LCD_REGS_MEMORY_ADDR];
	IF = &mem[IF_REGISTER];
	lcd_registers[STAT] |= mode_flags[0];
	mode = 0;
	LY_counter = 0;
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

	if(mode == 2){
			inc_LY();
			if(lcd_registers[LY] == scanlines_until_vblank){ //vblank
				mode = 3;
				generate_interrupt(vblank);
			}
			else
				mode = 0; 
		}

	else{
		mode == 3 ? mode = 0 : ++mode; //switch to next mode
		generate_interrupt(mode_x);	
	}

	lcd_registers[STAT] |= mode_flags[mode];

}

	

void lcd_driver::update(uint8_t cycles){

	mode_counter[mode] += cycles;

	if(mode_counter[mode] >= mode_cycles[mode]){

		mode_counter[mode] = 0;	
		switch_mode();

	}


	switch(mode){
		case 3:{

			LY_counter += cycles;
			if(LY_counter >= cycles_per_scanline){
				inc_LY();
				LY_counter = 0;
			}
		}
		
		default: ;
	}
			
}


	

