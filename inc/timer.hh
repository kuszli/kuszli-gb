#ifndef timerh
#define timerh

#include<cstdint>
#include"defines.h"
#include "memory.hh"

#define TIMER_REGISTERS_MEMORY_ADRRESS 0xFF04

#define DIV  0
#define TIMA 1
#define TMA  2
#define TAC  3

#define TIMER_ENABLE (1 << 2)




class timer{

	const uint16_t divider_speed = 16384;
	const uint16_t divider_cycles = 256;
	
	uint8_t* IF_register;
	uint8_t* timer_registers;
	uint8_t old_TAC;
	uint32_t clock_speed_hz[4]{4096, 262144, 65536, 16384}; //in Hz
	uint16_t clock_speed_cyc[4]{1024, 16, 64, 256}; //in cycles

	uint16_t divider_counter;
	uint16_t timer_counter;

	void count_divider();
	void count_timer();
	void overflow();	
	uint32_t clock_hz(){return clock_speed_hz[ timer_registers[TAC] & 0x3 ]; } //clock select is in first 2 bits of TAC register
	uint16_t clock_cyc(){return clock_speed_cyc[ timer_registers[TAC] & 0x3 ]; }  

public:
	
	timer(_memory& mem);
	void update(const uint8_t cycles);

	
};



#endif
