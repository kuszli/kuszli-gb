#ifndef audio_controllerh
#define audio_controllerh


#include "defines.h"
#include "memory.hh"
#include <cstdint>

#define AUDIO_REGISTERS_ADDR 0xFF10


struct channel{

	int8_t amplitude;
	uint8_t volume;
	const uint8_t* duty;
	uint8_t duty_cycle;
	uint32_t freq;
	uint32_t shadow_freq;
	uint32_t freq_counter;
	uint32_t sweep_counter;
	uint8_t sweep_step;
	uint32_t length;
	uint32_t sample_count;
	uint32_t envelope_counter;
	uint8_t curr_wave_pos;
	uint8_t wave_buffer;
	uint8_t prescaler;
	bool* trigg;
	bool len_enable;
	bool enable;
	
	channel(): volume(0), freq_counter(0), envelope_counter(0), enable(false){}
	bool triggered() { return *trigg; }
	void reset_trigg() { *trigg = false; }

};


class audio_controller{

	
	const uint8_t duty_12_5[8] = {0, 0, 0, 0, 0, 0, 0, 1};
	const uint8_t duty_25[8] = {1, 0, 0, 0, 0, 0, 0, 1};
	const uint8_t duty_50[8] = {1, 0, 0, 0, 0, 1, 1, 1};
	const uint8_t duty_75[8] = {0, 1, 1, 1, 1, 1, 1, 0};
	const uint8_t* duties[4] = {duty_12_5, duty_25, duty_50, duty_75};
		
	uint32_t sampling_freq;
	uint8_t channels_count;

	channel channel1, channel2, channel3, channel4;

	
	int16_t* sample_buffer;
	uint32_t ready_buff_pos;
	uint16_t buff_size;
	uint8_t buff_count;
	uint32_t curr_buff_pos;

	uint16_t lfsr;

	uint16_t counter;
	uint8_t* audio_registers;

	void update_channel1();
	void update_channel2();
	void update_channel3();
	void update_channel4();


public:

	audio_controller(_memory* mem);
	~audio_controller();
	void update(const uint8_t cycles);
	void set_sampling_freq(uint32_t freq){ sampling_freq = freq; }
	void set_channels_count(uint8_t count){ channels_count = count > 1 ? 2 : 1; }
	void set_buffer_size(uint16_t buffer_size);
	const int16_t* get_buffer();

};




#endif
