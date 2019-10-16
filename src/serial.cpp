#include "serial.hh"

serial::serial(_memory* mem){

	transfer_data = &(*mem)[0xFF01];
	control = &(*mem)[0xFF02];
	IF = &(*mem)[IF_REGISTER];
	trigger = &mem->serial_trigg;
	serial_cycles = 0;
	bits_transfered = 0;
	transfer_time = false;
}

void serial::update(uint8_t cycles){


	if(*trigger){
		*trigger = false;
		transfer_time = true;
		cycles_to_update = *control & 1 << 1 ?  16 : 512;
	}
	
	if(transfer_time){
		serial_cycles += cycles;
		if(serial_cycles >= cycles_to_update){
			serial_cycles -= cycles_to_update;
			if(*control & 1 << 0){
				*transfer_data = *transfer_data << 1;
				++bits_transfered;
			}
		}
		if(bits_transfered == 8){
			bits_transfered = 0;
			transfer_time = false;
			*control &= ~(1 << 7);
			*IF |= SERIAL_INT;
			serial_cycles = 0;
		}
	}

	
		
}


