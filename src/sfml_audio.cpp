#include "sfml_audio.hh"
#include <iostream>
#include <cstring>

sfml_audio::sfml_audio(gameboy* g, uint8_t channels, uint32_t sample_rate){

	initialize(channels, sample_rate);
	gb = g;
	stop_request = false;
	latency = 40; //in ms
	audio_buffer_size = sample_rate/((1000/latency)/channels);
	if(audio_buffer_size % 2 != 0)
		audio_buffer_size += 1;

	gb->set_audio_rate(sample_rate);
	gb->set_audio_channels(channels);
	gb->set_audio_buffer_size(audio_buffer_size);

}



bool sfml_audio::onGetData(Chunk& data){

	while((data.samples = gb->get_audio_buffer()) == nullptr){
		if(stop_request)
			return false;
	}

	data.sampleCount = audio_buffer_size;

	return true;
}


