#include <iostream>
#include "cmd_options.hh"


bool check_options(int argc, char** argv, bool& dbg, std::string& rom_name){

	if(argc < 2 || argc > 3){
	std::cerr << "Wrong number of parameters. Usage: kuszli-gb \"rom_name.gb\" [-d]\n";
	return false;
	}

	if(argc == 3){

		if(argv[1][0] == '-'){
			if(argv[1][1] == 'd'){
				dbg = true;
				rom_name = argv[2];
			}
			else{
				std::cerr << "Unknown option: " << argv[1][1] << "Usage: kuszli-gb \"rom_name.gb\" [-d]\n";
				return false;
			}
		}
		else{
			if(argv[2][1] == 'd'){
				dbg = true;
				rom_name = argv[1];
			}
			else{
				std::cerr << "Unknown option: " << argv[2][1] << ". Usage: kuszli-gb \"rom_name.gb\" [-d]\n";
				return false;
			}
		}

	}
	else
		rom_name = argv[1];

	return true;


}
