#include "rtc.hh"
#include <iostream>

rtc::rtc(uint8_t* regs): rtc_registers(regs){}

inline int32_t rtc::current_time(){

	using std::chrono::system_clock;
	return system_clock::now().time_since_epoch().count() * system_clock::period::num/system_clock::period::den;

}

void rtc::inc_mins(uint32_t mins){

	if(rtc_registers[MINUTES] + mins >= 60)
		inc_hrs((rtc_registers[MINUTES] + mins)/60);
	
	rtc_registers[MINUTES] = (rtc_registers[MINUTES] + mins) % 60;
}

void rtc::inc_hrs(uint32_t hrs){

	if(rtc_registers[HOURS] + hrs >= 24)
		inc_days((rtc_registers[HOURS] + hrs)/24);
	
	rtc_registers[HOURS] = (rtc_registers[HOURS] + hrs) % 24;
}

void rtc::inc_days(uint16_t days){

	if(rtc_registers[DAYS1] + days > 255){

		if(rtc_registers[DAYS2] & 1 << 0){
			rtc_registers[DAYS2] |= 1 << 7; //day counter overflow
			rtc_registers[DAYS2] &= ~(1 << 0);
		}
		else
			rtc_registers[DAYS2] |= 1 << 0;
	}
	
	rtc_registers[DAYS1] = (rtc_registers[DAYS1] + days) % 256;

}

void rtc::latch(){

	int32_t seconds_to_add = current_time() - last_time_point;
	last_time_point = current_time();
	
	if(rtc_registers[SECONDS] + seconds_to_add >= 60)
		inc_mins((rtc_registers[SECONDS] + seconds_to_add)/60);

	rtc_registers[SECONDS] = (rtc_registers[SECONDS] + seconds_to_add) % 60;

}


void rtc::write(uint8_t idx, uint8_t value){

	rtc_registers[idx] = value;
	if(!(rtc_registers[DAYS2] & 1 << 6))
		last_time_point = current_time();
}



