#include "joypad.hh"

joypad::joypad(_memory* mem){
	joypad_register = &(*mem)[JOYPAD];
	*joypad_register = 0x3F;
	IF = &(*mem)[IF_REGISTER];
	button = 0;
	last_state = 0xF;
}


void joypad::update(uint8_t butt){

  *joypad_register |= 0xF;
	uint8_t tmp = keys_select();

	if(tmp & 1 << 4){ //direction keys
		*joypad_register &= ~(butt & 0xF);
	}

	else if (tmp & 1 << 5){//button keys
		*joypad_register &= ~((butt >> 4) & 0xF);
	}

	if((*joypad_register & 0xF) != 0xF && last_state == 0xF) //high to low transition
		*IF |= BUTTON_INT;
	last_state = *joypad_register & 0xF;
	
}

void joypad::write(uint8_t butt){
	button = butt;
}

uint8_t joypad::keys_select(){

	return (~*joypad_register & 0x30);

}
