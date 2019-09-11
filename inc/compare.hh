#ifndef compareh
#define compareh

#include<cstdint>
#include "defines.h"

class compare{

	uint8_t* oam_addr;

public:

	compare(_memory* mem){oam_addr = &(*mem)[OAM];}
	bool operator ()(uint8_t a, uint8_t b){ return oam_addr[4*a+1] > oam_addr[4*b+1]; }

};
#endif
