#include "timer.hh"

timer::timer(_memory* mem){

	timer_registers = &(*mem)[TIMER_REGISTERS_MEMORY_ADRRESS];
	IF_register = &(*mem)[IF_REGISTER];
	system_counter = &(*mem)[0xFF03];
	*system_counter = 0;
	divider_counter = 0;
	timer_counter = 0;
	old_TAC = 0;
	overflow_flag = false;
}


void timer::overflow(){
	*IF_register |= TIMER_OV_INT;
	timer_registers[TIMA] = timer_registers[TMA];
}

void timer::count_divider(){
	++timer_registers[DIV];

}

void timer::count_timer(){

		++timer_registers[TIMA];
		if(timer_registers[TIMA] == 0){
			overflow();
		}

}


void timer::update(const uint8_t cycles){
	

	if(*system_counter > (uint8_t)(*system_counter + cycles)) //overflow
		count_divider();
	*system_counter += cycles;

	
	if(timer_registers[TAC] != old_TAC)
		timer_counter = 0;

	old_TAC = timer_registers[TAC];

	if(!(timer_registers[TAC] & TIMER_ENABLE))
		return;

	timer_counter += cycles;
	
	if(timer_counter >= clock_cyc()){

		while(timer_counter >= clock_cyc()){
			timer_counter -= clock_cyc();
			count_timer();
			
		}
	
		//timer_counter = 0;

	}
	

}
	



