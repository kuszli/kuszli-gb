#ifndef rtchh
#define rtchh

#include <chrono>
#include <cstdint>

class rtc{

	int32_t last_time_point;
	uint8_t*	rtc_registers;
	bool init;

public:
	rtc(uint8_t* regs);
	void latch();
	void trigger();

};



#endif
