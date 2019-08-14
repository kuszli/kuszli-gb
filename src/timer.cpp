#include "timer.hh"

timer::timer(uint8_t* mem){

	timer_registers = &mem[TIMER_REGISTERS_MEMORY_ADRRESS];
	IF_register = &mem[IF_REGISTER];

}

void timer::overflow(){
	*IF_register |= TIMER_INTERRUPT;
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



	



