
#include "gameboy.hh"
#include "sfml_interface.hh"
#include "cmd_options.hh"
#include <unistd.h>
#include <chrono>
#include <string>

#define VBLANK_IN_MICROSECONDS 16742


int main(int argc, char** argv){

	bool dbg = false;
	std::string rom_name;
	if(!check_options(argc, argv, dbg, rom_name))
		return -1;

	gameboy* gb = new gameboy; 
	gb->insert_cart(rom_name);
	sfml_interface interface(dbg);

	while(interface.is_window_open()){
		
		auto start = std::chrono::steady_clock::now();

		gb->run();
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


