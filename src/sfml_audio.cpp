#include "sfml_audio.hh"
#include <iostream>
#include <cstring>

sfml_audio::sfml_audio(gameboy* g, uint8_t channels, uint32_t sample_rate){

	initialize(channels, sample_rate);
	gb = g;
	stop_request = false;

}



bool sfml_audio::onGetData(Chunk& data){

	while((data.samples = gb->get_audio_buffer()) == nullptr){
		if(stop_request)
			return false;
	}
//	data.samples = gb->get_audio_buffer();
	data.sampleCount = 2204;//1638;

	return true;
}


