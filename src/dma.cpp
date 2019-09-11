#include "dma.hh"

dma::dma(_memory* mem){
	memory = mem;
	dma_register = &(*mem)[DMA_ADDR];
	oam = &(*mem)[OAM];
	counter = 0;
}

dma::~dma(){
	delete memory;
	delete dma_register;
	delete oam;
}

void dma::transfer(){

	std::memmove((void*)oam, (void*)&(*memory)[*dma_register * 0x100], OAM_SIZE);	
}


void dma::update(uint8_t cycles){

	if(memory->dma_request){
		counter = 0;
		memory->dma_request = false;
		memory->dma_time = true;
	}

	if(memory->dma_time){
		counter += cycles;
		if(counter >= transfer_time){
			transfer();
			counter = 0;
			memory->dma_time = false;
		}	
	}

}
