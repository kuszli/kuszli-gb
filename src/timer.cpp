#include "timer.hh"

timer::timer(_memory& mem){

	timer_registers = &mem[TIMER_REGISTERS_MEMORY_ADRRESS];
	IF_register = &mem[IF_REGISTER];
	divider_counter = 0;
	timer_counter = 0;
	old_TAC = 0;
}

void timer::overflow(){
	*IF_register |= TIMER_OV_INT;
	timer_registers[TIMA] = timer_registers[TMA];
}

void timer::count_divider(){
	++timer_registers[DIV];

}

void timer::count_timer(){
	if(timer_registers[TAC] & TIMER_ENABLE){
		++timer_registers[TIMA];
		if(timer_registers[TIMA] == 0)
			overflow();
	}
}


void timer::update(const uint8_t cycles){

	divider_counter += cycles;

	if(divider_counter >= divider_cycles){
		divider_counter = 0;
		count_divider();
	}
	
	if(timer_registers[TAC] != old_TAC)
		timer_counter = 0;

	old_TAC = timer_registers[TAC];

	timer_counter += cycles;
	
	if(timer_counter >= clock_cyc()){
		timer_counter = 0;
		count_timer();
	}

}
	



