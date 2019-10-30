#include "rtc.hh"

rtc::rtc(uint8_t* regs){
	rtc_registers = regs;
	init = false;
}

void rtc::latch(){
	


void rtc::trigger(){
	clock = std::chrono::steady_clock::now();
}
