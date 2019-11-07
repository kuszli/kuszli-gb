#include "gameboy.hh"
#include "sfml_interface.hh"
#include "sfml_audio.hh"
#include <string>
#include <stdlib.h>
#include <fstream>
#include <cstring>


int main(int argc, char** argv){

	if(argc < 2){	
		std::cerr << "Usage: kuszli-gb \"rom_name\"\n";
		return -1;
	}
	std::string rom_name = argv[1];

	gameboy* gb = new gameboy; 
	gb->insert_cart(rom_name);

	sfml_interface interface(gb);
	sfml_audio audio(gb, 2, 32768);
	audio.play();

	interface.event_loop();
	audio.request_stop();
	delete gb;

}


