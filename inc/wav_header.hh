#ifndef wav_headerh
#define wav_headerh

#include <cstdint>

class wav_header{

	uint8_t ChunkID[4];
	uint8_t ChunkSize[4];
	uint8_t Format[4];
	uint8_t Subchunk1ID[4];
	uint8_t Subchunk1Size[4];
	uint8_t AudioFormat[2];
	uint8_t NumChannels[2];
	uint8_t SampleRate[4];
	uint8_t ByteRate[4];
	uint8_t BlockAlign[2];
	uint8_t BitsPerSample[2];
	uint8_t Subchunk2ID[4];
	uint8_t Subchunk2Size[4];

public:

	wav_header();
	wav_header(const uint8_t channels, const uint32_t sample_rate, const uint8_t bits_per_sample);
	void set_channels(const uint8_t channels);
	void set_sample_rate(const uint32_t sample_rate);
	void set_bits_per_sample(const uint8_t bits_per_sample);

	uint8_t channels();
	uint8_t sample_rate();
	uint8_t bits_per_sample();

	void append_to_buffer(int16_t* const buffer, const uint32_t size);
		


};




#endif
