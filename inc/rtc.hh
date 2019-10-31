#ifndef rtchh
#define rtchh

#include <chrono>
#include <cstdint>

#define SECONDS 0
#define MINUTES 1
#define HOURS 2
#define DAYS1 3
#define DAYS2 4

class rtc{

	int32_t last_time_point;
	uint8_t*	rtc_registers;

	int32_t current_time();
	void inc_mins(uint32_t mins);
	void inc_hrs(uint32_t hrs);
	void inc_days(uint16_t days);

public:

	rtc(uint8_t* regs);
	void latch();
	void trigger();
	void write(uint8_t idx, uint8_t value);
	int32_t get_last_time_point(){ return last_time_point; }
	void load_last_time_point(int32_t value){ last_time_point = value; }
	

};



#endif
