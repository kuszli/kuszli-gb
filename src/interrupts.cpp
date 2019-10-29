#include "interrupts.hh"

interrupts::interrupts(_memory* mem){
	IF = &(*mem)[IF_REGISTER];
	IE = &(*mem)[IE_REGISTER];

}

interrupts::~interrupts(){
	IF = nullptr;
	IE = nullptr;
}

uint8_t interrupts::check(){
	uint8_t interrupt = 0xFF;
	for(uint8_t i = 0; i<5; ++i){
		if((*IE & 1 << i) && (*IF & 1 << i)){
			interrupt = i;
			break;
		}
	}

	return interrupt;
}
