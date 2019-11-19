#ifndef sfml_audio_h
#define sfml_audio_h


#include<SFML/Audio.hpp>
#include <cstdint>
#include "gameboy.hh"

class sfml_audio: public sf::SoundStream{

	gameboy* gb;
	bool stop_request;
	uint16_t audio_buffer_size;
	uint16_t latency; //in ms
	virtual bool onGetData(Chunk& data);
	virtual void onSeek(sf::Time timeOffset){}

public:

	sfml_audio(gameboy* g, uint8_t channels, uint32_t sample_rate);
	void request_stop() { stop_request = true; }
	
};




#endif
