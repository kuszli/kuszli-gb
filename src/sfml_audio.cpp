#include "sfml_audio.hh"
#include <iostream>
#include <cstring>

sfml_audio::sfml_audio(gameboy* g, uint8_t channels, uint32_t sample_rate){

	initialize(channels, sample_rate);
	gb = g;
	stop_request = false;

}



bool sfml_audio::onGetData(Chunk& data){

	while(gb->get_audio_buffer_size() == 0){
		if(stop_request)
			break;
	}

	data.samples = gb->get_audio_buffer();
	data.sampleCount = gb->get_audio_buffer_size();

	while(data.samples == gb->get_busy_audio_buffer()){
		if(stop_request)
			break;
	}

	return true;
}


