#include "audio_controller.hh"
#include <algorithm>
#include <iostream>
#include <cstring>


audio_controller::audio_controller(_memory* mem){

	audio_registers = &(*mem)[AUDIO_REGISTERS_ADDR];
	channel1.trigg = &mem->chan1_trigg;
	channel2.trigg = &mem->chan2_trigg;
	channel3.trigg = &mem->chan3_trigg;
	channel4.trigg = &mem->chan4_trigg;

	sample_buffer = new int16_t[16830];
	sampling_freq = 32768;
	curr_buff_pos = 0;
	ready_buff_pos = 0;
	audio_registers[CHANNEL_CONT] &= 0xCC; //initial volume
	counter = 0;

}
 
audio_controller::~audio_controller(){
	delete[] sample_buffer;
	sample_buffer = nullptr;

}



void audio_controller::update(const uint8_t cycles){


	if(audio_registers[AUDIO_CONT] & 1 << 7){

		uint8_t tmp = cycles;
		while(tmp > 0){
			update_channel1();
			update_channel2();
 			update_channel3();
			update_channel4();
			--tmp;
		}
	}

	else{

		channel1.amplitude = 0;
		channel2.amplitude = 0;
 		channel3.amplitude = 0;
		channel4.amplitude = 0;
	}

	counter += cycles;

	if(counter >= 4194304/sampling_freq){

		counter -= 4194304/sampling_freq;

		bool ch1_left_enable = (audio_registers[SOUND_LEVEL] & 1 << 0) ? true : false;
		bool ch1_right_enable = (audio_registers[SOUND_LEVEL] & 1 << 4) ? true : false;
		bool ch2_left_enable = (audio_registers[SOUND_LEVEL] & 1 << 1) ? true : false;
		bool ch2_right_enable = (audio_registers[SOUND_LEVEL] & 1 << 5) ? true : false;
		bool ch3_left_enable = (audio_registers[SOUND_LEVEL] & 1 << 2) ? true : false;
		bool ch3_right_enable = (audio_registers[SOUND_LEVEL] & 1 << 6) ? true : false;
		bool ch4_left_enable = (audio_registers[SOUND_LEVEL] & 1 << 3) ? true : false;
		bool ch4_right_enable = (audio_registers[SOUND_LEVEL] & 1 << 7) ? true : false;
		uint8_t left_vol = (audio_registers[CHANNEL_CONT] & 0x7) + 1;
		uint8_t right_vol = ((audio_registers[CHANNEL_CONT] & 0x70) >> 4) + 1;

		sample_buffer[curr_buff_pos++] = 64 *
		(channel1.amplitude * ch1_left_enable + channel2.amplitude * ch2_left_enable + channel3.amplitude * ch3_left_enable + channel4.amplitude * ch4_left_enable) * left_vol;

		sample_buffer[curr_buff_pos++] = 64 *
		(channel1.amplitude * ch1_right_enable + channel2.amplitude * ch2_right_enable + channel3.amplitude * ch3_right_enable + channel4.amplitude * ch4_right_enable) * right_vol;

		if(curr_buff_pos == 16380)
			curr_buff_pos = 0;
		
	}

}



const int16_t* audio_controller::get_buffer(){ 

	int16_t left_limit = curr_buff_pos - 1638;
	int16_t right_limit = curr_buff_pos + 1638;
	
	if(left_limit < 0)
		left_limit = 16380 + left_limit;
	if(right_limit > 16380)
		right_limit -= 16380;

	if(right_limit < left_limit){
		if(ready_buff_pos > left_limit || ready_buff_pos < right_limit)
			return nullptr;
	}


	else{ 
		if(ready_buff_pos > left_limit && ready_buff_pos < right_limit)
			return nullptr;
	}


	const int16_t* buff = const_cast<const int16_t*>(&sample_buffer[ready_buff_pos]);
	ready_buff_pos += 1638;
	if(ready_buff_pos >= 16380)
		ready_buff_pos = 0;

	return buff;

}

void audio_controller::update_channel1(){


	if(channel1.triggered()){

		audio_registers[AUDIO_CONT] |= 1 << 0;

		if(channel1.length == 0)
			channel1.length = 16384 * 64;
		else
			channel1.length = 16384 * (64 - (audio_registers[S1_LENGTH] & 0x3F));

		channel1.freq = ((audio_registers[S1_CONT] & 0x7) << 8) | audio_registers[S1_FREQ];
		channel1.shadow_freq = channel1.freq;
		channel1.sample_count = 4*(2048 - channel1.freq);
		channel1.freq_counter = 0;



		channel1.envelope_counter = (audio_registers[S1_ENVELOPE] & 0x7) * 65536;

		channel1.volume = ((audio_registers[S1_ENVELOPE] & 0xF0) >> 4);

		channel1.sweep_step = 1 << (audio_registers[S1_SWEEP] & 0x7);
		channel1.sweep_counter = 32768 * (audio_registers[S1_SWEEP] & 0x70) >> 4;

		channel1.duty = duties[ ((audio_registers[S1_LENGTH] & 0xC0) >> 6) ];
		channel1.duty_cycle = 0;
		channel1.enable = true;
		channel1.reset_trigg();
		
		
	}

	if(!channel1.enable){
		channel1.amplitude = 0;
		return;
	}

	if(!(audio_registers[S1_ENVELOPE] & 0xF8)){
		channel1.amplitude = 0;
		return;
	}

	if( ((audio_registers[S1_SWEEP] & 0x70) >> 4) != 0){

		--channel1.sweep_counter;
		
		if(channel1.sweep_counter == 0){
			
			if(audio_registers[S1_SWEEP] & 1 << 3){
				channel1.shadow_freq = channel1.shadow_freq - channel1.shadow_freq/channel1.sweep_step;
				channel1.sample_count = 4*(2048 - channel1.shadow_freq);
				
			}
			else{
				channel1.shadow_freq = channel1.shadow_freq + channel1.shadow_freq/channel1.sweep_step;
				if(channel1.shadow_freq > 2047){
					channel1.enable = false;
					audio_registers[AUDIO_CONT] &= ~(1 << 0);
					channel1.amplitude = 0;
					return;
				}
				else{
					channel1.sample_count = 4*(2048 - channel1.shadow_freq);
				}
			}

			channel1.sweep_counter = 32768 * (audio_registers[S1_SWEEP] & 0x70) >> 4;
		}

	}
	
	if((audio_registers[S1_ENVELOPE] & 0x7) != 0){

		--channel1.envelope_counter;

		if(channel1.envelope_counter == 0){
			
			if(audio_registers[S1_ENVELOPE] & 1 << 3){
				if(channel1.volume < 15)
					++channel1.volume;
			}
	
			else{
				if(channel1.volume > 0)
					--channel1.volume;
			}

			channel1.envelope_counter = (audio_registers[S1_ENVELOPE] & 0x7) * 65536;
		}

	}

	++channel1.freq_counter;
	if(channel1.freq_counter == channel1.sample_count){
		channel1.freq_counter = 0;
		++channel1.duty_cycle;
		if(channel1.duty_cycle == 8)
			channel1.duty_cycle = 0;
	}


	if(!(audio_registers[S1_CONT] & 1 << 6))
		channel1.amplitude = channel1.volume * channel1.duty[channel1.duty_cycle];

	
	else{
		


		if(channel1.length != 0)
			channel1.amplitude = channel1.volume * channel1.duty[channel1.duty_cycle];

		else{
			audio_registers[AUDIO_CONT] &= ~(1 << 0);
			channel1.amplitude = 0;
			channel1.enable = false;
		}

		--channel1.length;
		
	}	
	
}








void audio_controller::update_channel2(){


	if(channel2.triggered()){

		audio_registers[AUDIO_CONT] |= 1 << 1;

		if(channel2.length == 0)
			channel2.length = 16384 * 64;
		else
			channel2.length = 16384 *( 64 - (audio_registers[S2_LENGTH] & 0x3F));

		
		channel2.freq = ((audio_registers[S2_CONT] & 0x7) << 8) | audio_registers[S2_FREQ];
		channel2.sample_count = 4*(2048 - channel2.freq);
		channel2.freq_counter = channel2.freq_counter & 0x3;


		channel2.envelope_counter = (audio_registers[S2_ENVELOPE] & 0x7) * 65536;

		channel2.volume = ((audio_registers[S2_ENVELOPE] & 0xF0) >> 4);

		channel2.duty = duties[ ((audio_registers[S2_LENGTH] & 0xC0) >> 6) ];
		channel2.duty_cycle = 0;
		channel2.enable = true;
		channel2.reset_trigg();
		
	}

	if(!channel2.enable){
		channel2.amplitude = 0;
		return;
	}

	if(!(audio_registers[S2_ENVELOPE] & 0xF8)){
		channel2.amplitude = 0;
		return;
	}
	
	if((audio_registers[S2_ENVELOPE] & 0x7) != 0){

		--channel2.envelope_counter;

		if(channel2.envelope_counter == 0){
			
			if(audio_registers[S2_ENVELOPE] & 1 << 3){
				if(channel2.volume < 15)
					++channel2.volume;
			}
	
			else{
				if(channel2.volume > 0)
					--channel2.volume;
			}

			channel2.envelope_counter = (audio_registers[S2_ENVELOPE] & 0x7) * 65536;
		}

	}


	++channel2.freq_counter;
	if(channel2.freq_counter == channel2.sample_count){
		channel2.freq_counter = 0;
		++channel2.duty_cycle;
		if(channel2.duty_cycle == 8)
			channel2.duty_cycle = 0;
	}



	if(!(audio_registers[S2_CONT] & 1 << 6))
		channel2.amplitude = channel2.volume * channel2.duty[channel2.duty_cycle];

	
	else{
		
		if(channel2.length != 0)
			channel2.amplitude = channel2.volume * channel2.duty[channel2.duty_cycle];
		else{
			audio_registers[AUDIO_CONT] &= ~(1 << 1);
			channel2.amplitude = 0;
			channel2.enable = false;
		}

		--channel2.length;

	}	


}



void audio_controller::update_channel3(){



	if(channel3.triggered()){

		
		channel3.len_enable = (audio_registers[S3_CONT] & 1 << 6) ? true : false;

		audio_registers[AUDIO_CONT] |= 1 << 2;

		if(channel3.length == 0)
			channel3.length = 16384*256;
		else
			channel3.length = 16384*(256 - audio_registers[S3_LENGTH]);
		
		channel3.freq = ((audio_registers[S3_CONT] & 0x7) << 8) | audio_registers[S3_FREQ];
		channel3.sample_count = (2048 - channel3.freq) * 2;
		
		channel3.freq_counter = 0;
		channel3.curr_wave_pos = 0;
		channel3.wave_buffer = audio_registers[WAVE_PATTERN] & 0xF;
		channel3.enable = true;
		channel3.reset_trigg();
		
	}


	if(!channel3.enable){
		channel3.amplitude = 0;
		return;
	}

	if(!(audio_registers[S3_ONOFF] & 1 << 7) ){
		channel3.amplitude = 0;
		return;
	}



	if(channel3.curr_wave_pos % 2 == 0)
		channel3.volume = channel3.wave_buffer >> 4;
	else
		channel3.volume = channel3.wave_buffer & 0xF;
	
	

	uint8_t pattern = (audio_registers[S3_PATTERN] & 0x60) >> 5;

	if( pattern == 0)
		channel3.volume = 0;
	
	else if( pattern >= 2)
		channel3.volume = channel3.volume >> (pattern-1);
	

	++channel3.freq_counter;
	
	if(channel3.freq_counter == channel3.sample_count){

		++channel3.curr_wave_pos;

		if(channel3.curr_wave_pos == WAVE_PATTERN_SIZE*2)
			channel3.curr_wave_pos = 0;


		channel3.wave_buffer = audio_registers[WAVE_PATTERN + channel3.curr_wave_pos/2];

		channel3.freq_counter = 0;
	}

	
	
	if(!channel3.len_enable)
		channel3.amplitude = channel3.volume;
	

	else{
		


		if(channel3.length != 0){
			channel3.amplitude = channel3.volume;
		}

		
		else{
			audio_registers[AUDIO_CONT] &= ~(1 << 2);
			channel3.amplitude = 0;
			channel3.enable = false;
		}

		--channel3.length;

	}

	
}


void audio_controller::update_channel4(){



	if(channel4.triggered()){
		
		
		channel4.len_enable = (audio_registers[S4_CONT] & 1 << 6) ? true : false;
		
		audio_registers[AUDIO_CONT] |= 1 << 3;

		if(channel4.length == 0)
			channel4.length = 16384*64;
		else
			channel4.length = 16384*(64 - (audio_registers[S4_LENGTH] & 0x3F));

		channel4.prescaler = (audio_registers[S4_PCOUNTER] & 0x7);
		if(channel4.prescaler == 0)
			channel4.freq = 1048576/(1 << (((audio_registers[S4_PCOUNTER] & 0xF0) >> 4) + 1));
		else
			channel4.freq = (524288/channel4.prescaler)/(1 << (((audio_registers[S4_PCOUNTER] & 0xF0) >> 4) + 1));
		
		channel4.sample_count = 4194304/channel4.freq;
		channel4.freq_counter = 0;

		channel4.envelope_counter = (audio_registers[S4_ENVELOPE] & 0x7) * 65536;
		channel4.volume = ((audio_registers[S4_ENVELOPE] & 0xF0) >> 4);
		lfsr = 32767;
		channel4.enable = true;
		channel4.reset_trigg();
		
	}


	if(!channel4.enable){
		channel4.amplitude = 0;
		return;
	}

	if(!(audio_registers[S4_ENVELOPE] & 0xF8)){
		channel4.amplitude = 0;
		return;
	}

	if((audio_registers[S4_ENVELOPE] & 0x7) != 0){

		--channel4.envelope_counter;

		if(channel4.envelope_counter == 0){
			
			if(audio_registers[S4_ENVELOPE] & 1 << 3){
				if(channel4.volume < 15)
					++channel4.volume;
			}
	
			else{
				if(channel4.volume > 0)
					--channel4.volume;
			}

			channel4.envelope_counter = (audio_registers[S4_ENVELOPE] & 0x7) * 65536;
		}

	}



	++channel4.freq_counter;
	
	if(channel4.freq_counter == channel4.sample_count){
		
		uint8_t bit = ((lfsr >> 1) ^ lfsr) & 0x1;
		lfsr = lfsr >> 1;
		lfsr |= bit << 14;
		if(audio_registers[S4_PCOUNTER] & 1 << 3){
			lfsr &= ~(1 << 6);
			lfsr |= bit << 6;
		}

		channel4.freq_counter = 0;
	}

	
	
	if(!channel4.len_enable)
		channel4.amplitude = channel4.volume * (~lfsr & 0x1);
		

	else{
		

		if(channel4.length != 0)
			channel4.amplitude = channel4.volume * (~lfsr & 0x1);

		
		else{
			audio_registers[AUDIO_CONT] &= ~(1 << 3);
			channel4.amplitude = 0;
			channel4.enable = false;
		}

		--channel4.length;


	}

	
}


