#include "dma.hh"

dma::dma(_memory &mem){
	memory = mem;
	dma_register = &mem[DMA];
	oam = &mem[OAM];
	counter = 0;
}

void dma::transfer(){
	if(*dma_register < 0x80)
		return;
	else
		std::memmove((void*)oam, (void*)&memory[*dma_register * 0x100], OAM_SIZE);

}


void dma::update(uint8_t cycles){

	counter += cycles;
	if(counter >= transfer_time){
		counter = 0;
		transfer();
	}



}
