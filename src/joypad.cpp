#include "joypad.hh"

joypad::joypad(_memory &mem){
	joypad_register = &mem[JOYPAD];
	*joypad_register = 0x3F;
	button = 0;
}


void joypad::update(uint8_t butt){
//	std::cout << std::hex << (int) *joypad_register << std::endl;
  *joypad_register |= 0xF;
	uint8_t tmp = keys_select();

	if(tmp & 1 << 4){ //direction keys
		*joypad_register &= ~(butt & 0xF);
	}

	else if (tmp & 1 << 5){//button keys
		*joypad_register &= ~((butt >> 4) & 0xF);
	}


}

void joypad::write(uint8_t butt){
	button = butt;
}

uint8_t joypad::keys_select(){

	return (~*joypad_register & 0x30);

}
