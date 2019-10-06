
#include "gameboy.hh"
#include "sfml_interface.hh"
#include "cmd_options.hh"
#include <unistd.h>
#include <chrono>
#include <string>
#include <SFML/Audio.hpp>
#include "wav_header.hh"
#include <stdlib.h>
#include <fstream>
#include <cstring>

#define VBLANK_IN_MICROSECONDS 16742


int main(int argc, char** argv){

	unsigned char frame_count = 0;
	bool dbg = false;
	std::string rom_name;
	if(!check_options(argc, argv, dbg, rom_name))
		return -1;

	gameboy* gb = new gameboy; 
	gb->insert_cart(rom_name);
	gb->set_debug(dbg);
	sfml_interface interface(dbg);

	wav_header head(2, 32768, 8);
	
	head.append_to_buffer(gb->audio_buffer(), 8192);	
	head.append_to_buffer(gb->second_audio_buffer(), 8192);

	sf::SoundBuffer buffer;
	sf::Sound sound;

	
	while(interface.is_window_open()){
		
		auto start = std::chrono::steady_clock::now();

		gb->run();

		if(gb->audio_buffer_ready()){
			gb->reset_audio_buffer_state();
			buffer.loadFromMemory(gb->ready_audio_buffer(), 8192);
			sound.setBuffer(buffer);
			sound.play();
		}
		
		interface.display(gb->get_display_data());


		if(dbg) 
			interface.show_oam(gb->oam_debug());

		interface.check_events();
		gb->set_buttons(interface.get_key());

		
		auto end = std::chrono::steady_clock::now();

		if(VBLANK_IN_MICROSECONDS - std::chrono::duration_cast<std::chrono::microseconds>(end-start).count() > 0)
			usleep(VBLANK_IN_MICROSECONDS - std::chrono::duration_cast<std::chrono::microseconds>(end-start).count()); 
		

	}
	
	delete gb;

}


