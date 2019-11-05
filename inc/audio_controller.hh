#ifndef audio_controllerh
#define audio_controllerh


#include "defines.h"
#include "memory.hh"
#include <cstdint>

#define AUDIO_REGISTERS_ADDR 0xFF10


struct channel{

	uint8_t channel_no;
	int8_t amplitude;
	uint8_t volume;
	const uint8_t* duty;
	uint8_t duty_cycle;
	uint32_t freq;
	uint32_t shadow_freq;
	uint32_t real_freq;
	uint32_t freq_counter;
	uint32_t sweep_counter;
	uint8_t sweep_step;
	uint32_t length;
	uint32_t length_counter;
	uint32_t sample_count;
	uint8_t envelope_steps;
	uint32_t envelope_counter;
	uint8_t curr_wave_pos;
	uint8_t wave_buffer;
	uint8_t prescaler;
	bool* trigg;
	bool len_enable;
	float wave_timer;
	float wave_step;
	bool enable;
	
	channel(): volume(0), real_freq(1024), freq_counter(0), envelope_counter(0), enable(false){}
	bool triggered() { return *trigg; }
	void reset_trigg() { *trigg = false; }

};


class audio_controller{

	
	const uint8_t duty_12_5[8] = {0, 0, 0, 0, 0, 0, 0, 1};
	const uint8_t duty_25[8] = {1, 0, 0, 0, 0, 0, 0, 1};
	const uint8_t duty_50[8] = {1, 0, 0, 0, 0, 1, 1, 1};
	const uint8_t duty_75[8] = {0, 1, 1, 1, 1, 1, 1, 0};
	const uint8_t* duties[4] = {duty_12_5, duty_25, duty_50, duty_75};
		
	uint16_t sampling_freq;

	channel channel1, channel2, channel3, channel4;


	int16_t* sample_buffer;
	uint32_t ready_buff_pos;
	uint16_t buff_size;
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
	void set_sampling_freq(uint16_t freq){ sampling_freq = freq; }
	const int16_t* get_buffer();
	const uint32_t get_buffer_size() { return ready_buff_pos; }
	const int16_t* busy_buffer() { return const_cast<const int16_t*>(sample_buffer); }
};




#endif
