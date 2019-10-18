#include "dma.hh"

dma::dma(_memory* mem){
	memory = mem;
	dma_register = &(*mem)[DMA_ADDR];
	oam = &(*mem)[OAM];
	counter = 0;
	hdma_transfer = false;
	transfer_size = 0;
	hdma_counter = 0;

}

dma::~dma(){
	memory = nullptr;
	dma_register = nullptr;
	oam = nullptr;
}

void dma::transfer(){

	std::memmove((void*)oam, (void*)&(*memory)[*dma_register * 0x100], OAM_SIZE);	
}

void dma::transfer_cgb(){





	if(memory->hblank_dma_time){
		
		std::memmove((void*)&(*memory)[0x8000 + hdma_dest], (void*)&(*memory)[hdma_src], 16);

		hdma_dest += 16;
		hdma_src += 16;

		--hdma_len;
		(*memory)[0xFF55] = hdma_len;

		if(hdma_len == 0xFF)
			memory->hblank_dma_time = false;
	
	}

	else{	
		std::memmove((void*)&(*memory)[0x8000 + hdma_dest], (void*)&(*memory)[hdma_src], 16*(hdma_len + 1));
		(*memory)[0xFF55] = 0xFF;
		hdma_src += 16*(hdma_len + 1);
		hdma_dest += 16*(hdma_len + 1);

	}
}


void dma::update(uint8_t cycles){

	if(memory->dma_request){
		counter = 0;
		memory->dma_request = false;
		memory->dma_time = true;
	}

	if(memory->hdma_request){

		memory->hdma_request = false;

		hdma_len = (*memory)[0xFF55] & 0x7F;

		if(memory->hdma_src_trigger)
			hdma_src = ( ((*memory)[0xFF51] << 8) | (*memory)[0xFF52] ) & 0xFFF0;
		if(memory->hdma_dest_trigger)	
			hdma_dest = ( ((*memory)[0xFF53] << 8) | (*memory)[0xFF54] ) & 0x1FF0;

		if((*memory)[0xFF55] & 1 << 7){ //hblank hdma mode
			(*memory)[0xFF55] &= ~(1 << 7);
			memory->hblank_dma_time = true;	
		}

		else{ //general hdma
			transfer_cgb();
			hdma_transfer = true;
			transfer_size = hdma_len + 1;
		}
	}


	if(memory->dma_time){
		counter += cycles;
		if(counter >= transfer_time){
			transfer();
			counter = 0;
			memory->dma_time = false;
		}	
	}

	if(memory->hblank_dma_time){

		if( ((*memory)[0xFF41] & 0x3) == 0 && last_lcd_mode != 0){ //hblank mode
			transfer_cgb();
			hdma_transfer = true;
			transfer_size = 1;
		}
	
		last_lcd_mode = (*memory)[0xFF41] & 0x3;
	}



}
