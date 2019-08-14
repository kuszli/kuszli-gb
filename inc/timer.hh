#ifndef timerh
#define timerh

#include<cstdint>

#define TIMER_REGISTERS_MEMORY_ADRRESS 0xFF04
#define IF_REGISTER 0xFF0F
#define DIV  0
#define TIMA 1
#define TMA  2
#define TAC  3

#define TIMER_INTERRUPT (1 << 2)
#define TIMER_ENABLE (1 << 2)
#define CLOCK_DIV_1024 0
#define CLOCK_DIV_16   1
#define CLOCK_DIV_64   2
#define CLOCK_DIV_256  3




class timer{
	
	uint8_t* IF_register;
	uint8_t* timer_registers;
	uint32_t clock_speed[4]{4096, 262144, 65536, 16384}; //in Hz
	void overflow();

public:
	
	timer(uint8_t* mem);
	uint8_t& reg(int i){return timer_registers[i]; }
	void count_divider();
	void count_timer();
	uint32_t clock(){return clock_speed[ timer_registers[TAC] & 0x3 ]; } //clock select is in first 2 bits of TAC register
	

};



#endif
