#include "wav_header.hh"
#include <cstring>

wav_header::wav_header(){

	const char* riff = "RIFF";
	const char* wave = "WAVE";
	const char* fmt = "fmt ";
	const char* data = "data";

	std::memcpy(ChunkID, riff, 4);
	std::memcpy(Format, wave, 4);
	std::memcpy(Subchunk1ID, fmt, 4);
	std::memcpy(Subchunk2ID, data, 4);

	std::memset(Subchunk1Size, 0, 4);
	Subchunk1Size[0] = 16;

	AudioFormat[0] = 1;
	AudioFormat[1] = 0;


}


wav_header::wav_header(const uint8_t channels, const uint32_t sample_rate, const uint8_t bits_per_sample): wav_header(){

	NumChannels[0] = channels;
	NumChannels[1] = 0;

	*(uint32_t *)SampleRate = sample_rate;

	*(uint32_t *)ByteRate = sample_rate * channels * bits_per_sample/8;


	BlockAlign[0] = channels * bits_per_sample/8;
	BlockAlign[1] = 0;

	BitsPerSample[0] = bits_per_sample;
	BitsPerSample[1] = 0;
	
}



void wav_header::set_channels(const uint8_t channels){

	NumChannels[0] = channels;
	NumChannels[1] = 0;

}
	

void wav_header::set_sample_rate(const uint32_t sample_rate){

	*(uint32_t *)SampleRate = sample_rate;

}

void wav_header::set_bits_per_sample(const uint8_t bits_per_sample){

	BitsPerSample[0] = bits_per_sample;
	BitsPerSample[1] = 0;

}


void wav_header::append_to_buffer(int16_t* const buffer, const uint32_t size){

	if(size <= sizeof(wav_header))
		return;

	*(uint32_t *)ChunkSize = size - 8;

	*(uint32_t *)Subchunk2Size = size - 44;

	std::memcpy(buffer, (void*)this, sizeof(wav_header));


}
