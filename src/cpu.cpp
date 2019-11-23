
#include"cpu.hh"
#include "opcodes_init_list.h"


cpu::cpu(_memory* mem){

	memory = mem;
	IME = false;
	EI_scheduled = false;
	_state = running;

	init_registers();

	opcode_array = new (uint8_t (cpu::*[256])()){ OP_ARRAY };

	CB_array = new (uint8_t (cpu::*[256])()){ CB_ARRAY };

	cpu_speed = 1;
	interrupt_start = false;

	regs = new uint8_t*[8]{&A,&F,&B,&C,&D,&E,&H,&L};
	regs16 = new uint16_t*[2]{&SP,&PC};

}

cpu::~cpu(){
	delete[] opcode_array;
	opcode_array = nullptr;
	delete[] CB_array;
	CB_array = nullptr;
}

void cpu::init_registers(){
	
	PC = 0x100;
	SP = 0xFFFE;
	A = 0x11;
	F = 0xB0;
	B = 0;
	C = 0x13;
	D = 0;
	E = 0xD8;
	H = 1;
	L = 0x4D;
	(*memory)[0xFF40] = 0x91;

}



bool cpu::handle_interrupts(uint8_t id){

	if(id == 0xFF)
		return false;
	
	if(_state == halted)
		_state = running;


	if(IME == true){
		memory->write(IF_REGISTER, (*memory)[IF_REGISTER] & ~(1 << id));
		IME = false;
		memory->write(--SP, PC >> 8);
		memory->write(--SP, PC & 0xFF); 
		PC = interrupt_address[id];
		interrupt_start = true;
		return true;
	}
	else
		return false;
	
		
}


uint8_t cpu::decode(){

	if(EI_scheduled){
		IME = true;
		EI_scheduled = false;
	}

	if(_state == halted)
		return 0;
	
	else
		return (*memory)[PC++];
}

uint8_t cpu::execute(const uint8_t opcode){

	if(interrupt_start){
		--PC;
		interrupt_start = false;
		return 20;
	}

	return (this->*opcode_array[opcode])();
}


void cpu::reset(){

	IME = false;
	EI_scheduled = false;
	_state = running;
	init_registers();

}
	
	
//*********generic cpu::instructions*********************************************

uint8_t cpu::NOP(){ return 4; }

uint8_t cpu::LD_REG16_d16(uint8_t &reg_high, uint8_t &reg_low){
	reg_low = (*memory)[PC++];
	reg_high = (*memory)[PC++];
	return 12;
}

uint8_t cpu::LD_REG8_d8(uint8_t &reg){
	reg = (*memory)[PC++];
	return 8;
}


uint8_t cpu::LD_REG16_A(const uint16_t reg){
	memory->write(reg, A);
	return 8;
}

uint8_t cpu::INC_REG16(uint8_t &reg_high, uint8_t &reg_low){
	++reg_low;
	if(reg_low==0)
		++reg_high;
	return 8;
}

uint8_t cpu::INC_REG8(uint8_t &reg){
	if( (reg & 0xF) == 0xF)
		F |= HALF_CARRY_FLAG;
	else 
		F &= ~HALF_CARRY_FLAG;

	++reg;

	if(reg == 0)
		F |= ZERO_FLAG;
	else 
		F &= ~ZERO_FLAG;

// clear subtract flag
	F &= ~SUBTRACT_FLAG;
	return 4;
}



uint8_t cpu::cpu::DEC_REG16(uint8_t &reg_high, uint8_t &reg_low){
	--reg_low;
	if(reg_low == 0xFF)
		--reg_high;
	return 8;
}

uint8_t cpu::DEC_REG8(uint8_t &reg){
	if( (reg & 0xF) == 0)
		F |= HALF_CARRY_FLAG;
	else 
		F &= ~HALF_CARRY_FLAG;
	--reg;

	if(reg == 0)
		F |= ZERO_FLAG;
	else 
		F &= ~ZERO_FLAG;
//set subtract flag
	F |= SUBTRACT_FLAG;
	return 4;
}

uint8_t cpu::RLCA(){
	if(A & 1<<7)
		F |= CARRY_FLAG;
	else
		F &= ~CARRY_FLAG;

//shift left 1 bit and set new bit 0 to old bit 7
	A = (A << 1) | ((A & 1<<7)>>7);

	F &= ~ZERO_FLAG;
	F &= ~SUBTRACT_FLAG;
	F &= ~HALF_CARRY_FLAG;
	return 4;
}

uint8_t cpu::LD_a16_SP(){
	uint8_t low = (*memory)[PC++];
	uint8_t high = (*memory)[PC++];
	uint16_t addr = (high << 8) | low;
	memory->write(addr, SP & 0xFF); 
	memory->write(addr+1, SP >> 8); 
	return 20;
}

uint8_t cpu::ADD_HL_REG16(const uint16_t reg){
	uint32_t res = HL + reg;

	if(res & 1<<16)
		F |= CARRY_FLAG;
	else 
		F &= ~CARRY_FLAG;

	if( ((HL & 0x0FFF) + (reg & 0x0FFF)) & 1<<12)
		F |= HALF_CARRY_FLAG;
	else 
		F &= ~HALF_CARRY_FLAG;
	
	F &= ~SUBTRACT_FLAG;
	
	H = (res & 0xFF00) >> 8;
	L = (res & 0x00FF);
	return 8;
}




uint8_t cpu::LD_A_ADDR(const uint16_t reg_addr){
	A = (*memory)[reg_addr];
	return 8;
}

uint8_t cpu::RRCA(){
	if(A & 1<<0)
		F |= CARRY_FLAG;
	else
		F &= ~CARRY_FLAG;

//shift right 1 bit and set new bit 7 to old bit 0
	A = (A >> 1) | ((A & 1<<0)<<7);

	F &= ~ZERO_FLAG;
	F &= ~SUBTRACT_FLAG;
	F &= ~HALF_CARRY_FLAG;
	return 4;
}

uint8_t cpu::STOP(){
	//_state = stopped;
	if((*memory)[0xFF4D] & 1 << 0){
		cpu_speed = (*memory)[0xFF4D] & 1 << 7 ? 1 : 2;
		(*memory)[0xFF4D] = (cpu_speed - 1) << 7;
	}

	PC++;
	return 4;
}

uint8_t cpu::LD_ADDR_A(const uint16_t reg_addr){
	memory->write(reg_addr, A);
	return 8;
}

uint8_t cpu::RLA(){
	uint8_t tmpA = A;

//shift left by 1 byte and set new bit 0 to CARRY_FLAG
	A = (A << 1) | (F & CARRY_FLAG)>>4;

	if(tmpA & 1<<7)
		F |= CARRY_FLAG;
	else
		F &= ~CARRY_FLAG;



	F &= ~ZERO_FLAG;
	F &= ~SUBTRACT_FLAG;
	F &= ~HALF_CARRY_FLAG;
	return 4;
}

uint8_t cpu::JR_r8(){
	PC+=(int8_t)(*memory)[PC] + 1;
	return 12;
}

uint8_t cpu::RRA(){
	uint8_t tmpA = A;
	

//shift right 1 bit and set new bit 7 to CARRY_FLAG
	A = (A >> 1) | ((F & CARRY_FLAG)<<3);


	if(tmpA & 1<<0)
		F |= CARRY_FLAG;
	else
		F &= ~CARRY_FLAG;



	F &= ~ZERO_FLAG;
	F &= ~SUBTRACT_FLAG;
	F &= ~HALF_CARRY_FLAG;
	return 4;
}


uint8_t cpu::JR_NZ_r8(){
	if(F & ZERO_FLAG){
		++PC;
		return 8;
	}
	else{
		PC+=(int8_t)(*memory)[PC] + 1;
		return 12;
	}
}

uint8_t cpu::LD_HLINC_A(){
	memory->write(HL, A);
	INC_REG16(H,L); //this might need some workaround
	return 8;
}

uint8_t cpu::LD_HLDEC_A(){
	memory->write(HL, A);
	DEC_REG16(H,L); //this might need some workaround
	return 8;
}

uint8_t cpu::DAA(){
	int8_t value_to_add = 0;
	
	if(!(F & SUBTRACT_FLAG)){ //last operation was an addition
		if( ((A & 0xF0) > 0x90) || (F & CARRY_FLAG) || (((A & 0xF0) >= 0x90) && ((A & 0xF) > 0x9)) ){
			value_to_add |= 0x60;
			F |= CARRY_FLAG;
		}
		if( ((A & 0xF) > 0x9) || (F & HALF_CARRY_FLAG) )
			value_to_add |= 0x6;
	}

	else{ //last operation was a subtraction
		if(F & HALF_CARRY_FLAG)
			value_to_add -= 0x6;
		if(F & CARRY_FLAG)
			value_to_add -= 0x60;
	}

	A += value_to_add;

	if(A == 0)
		F |= ZERO_FLAG;
	else
		F &= ~ZERO_FLAG;

	F &= ~HALF_CARRY_FLAG;
	return 4; 

}




uint8_t cpu::JR_Z_r8(){
	if((F & ZERO_FLAG)==0){
		++PC;
		return 8;
	}
	else{
		PC+=(int8_t)(*memory)[PC] + 1;
		return 12;
	}
}

uint8_t cpu::LD_A_HLINC(){
	A = (*memory)[HL];
	INC_REG16(H,L); //this might need some workaround
	return 8;
}

uint8_t cpu::LD_A_HLDEC(){
	A = (*memory)[HL];
	DEC_REG16(H,L); //this might need some workaround
	return 8;
}

uint8_t cpu::CPL(){
	A = ~A;
	F |= SUBTRACT_FLAG;
	F |= HALF_CARRY_FLAG;
	return 4;
}

	
uint8_t cpu::JR_NC_r8(){
	if(F & CARRY_FLAG){
		++PC;
		return 8;
	}
	else{
		PC+=(int8_t)(*memory)[PC] + 1;
		return 12;
	}
}

uint8_t cpu::LD_SP_d16(){
	uint8_t reg_low = (*memory)[PC++];
	uint8_t reg_high = (*memory)[PC++];
	SP = (reg_high) << 8 | reg_low;
	return 12;
}


uint8_t cpu::INC_SP(){
	++SP;
	return 8;
}

uint8_t cpu::DEC_SP(){
	--SP;
	return 8;
}

uint8_t cpu::INC_HLADDR(){
	if( ((*memory)[HL] & 0xF) == 0xF)
		F |= HALF_CARRY_FLAG;
	else 
		F &= ~HALF_CARRY_FLAG;

	++(*memory)[HL];

	if((*memory)[HL] == 0)
		F |= ZERO_FLAG;
	else 
		F &= ~ZERO_FLAG;

// clear subtract flag
	F &= ~SUBTRACT_FLAG;
	return 12;
}


uint8_t cpu::DEC_HLADDR(){
	if( ((*memory)[HL] & 0xF) == 0)
		F |= HALF_CARRY_FLAG;
	else 
		F &= ~HALF_CARRY_FLAG;
	--(*memory)[HL];

	if((*memory)[HL] == 0)
		F |= ZERO_FLAG;
	else 
		F &= ~ZERO_FLAG;
//set subtract flag
	F |= SUBTRACT_FLAG;
	return 12;
}



uint8_t cpu::LD_HLADDR_d8(){
	memory->write(HL, (*memory)[PC++]);
	return 12;
}

uint8_t cpu::SCF(){
	F |= CARRY_FLAG;
	F &= ~HALF_CARRY_FLAG;
	F &= ~SUBTRACT_FLAG;
	return 4;
}

uint8_t cpu::JR_C_r8(){
	if((F & CARRY_FLAG)==0){
		++PC;
		return 8;
	}
	else{
		PC+=(int8_t)(*memory)[PC] + 1;
		return 12;
	}
}

uint8_t cpu::CCF(){
	if(F & CARRY_FLAG)
		F &= ~CARRY_FLAG;
	else
		F |= CARRY_FLAG;

	F &= ~HALF_CARRY_FLAG;
	F &= ~SUBTRACT_FLAG;
	return 4;
}

uint8_t cpu::LD_REG8_REG8(uint8_t &reg1, const uint8_t reg2){
	reg1 = reg2;
	return 4;
}


uint8_t cpu::LD_HLADDR_REG8(const uint8_t reg){
	memory->write(HL, reg);
	return 8;
}

uint8_t cpu::LD_REG8_HLADDR(uint8_t &reg){
	reg = (*memory)[HL];
	return 8;
}

uint8_t cpu::HALT(){

	if( !IME && (((*memory)[IE_REGISTER] & (*memory)[IF_REGISTER]) & 0x1F) )
		--PC;

	else
		_state = halted;
	
	return 4;
}

uint8_t cpu::ADD_A_REG8(const uint8_t reg){
	uint16_t res = A+reg;
	if( ((A & 0xF) + (reg & 0xF)) & 1 << 4)
		F |= HALF_CARRY_FLAG;
	else 
		F &= ~HALF_CARRY_FLAG;

	if(res & 1 << 8)
		F |= CARRY_FLAG;
	else 
		F &= ~CARRY_FLAG;

	A+=reg;
	if(A == 0)
		F |= ZERO_FLAG;
	else 
		F &= ~ZERO_FLAG;

	F &= ~SUBTRACT_FLAG;
	return 4;
}


uint8_t cpu::ADD_A_HLADDR(){
	uint16_t res = A+(*memory)[HL];
	if( ((A & 0xF) + ((*memory)[HL] & 0xF)) & 1 << 4)
		F |= HALF_CARRY_FLAG;
	else 
		F &= ~HALF_CARRY_FLAG;

	if(res & 1 << 8)
		F |= CARRY_FLAG;
	else 
		F &= ~CARRY_FLAG;

	A+=(*memory)[HL];
	if(A == 0)
		F |= ZERO_FLAG;
	else 
		F &= ~ZERO_FLAG;

	F &= ~SUBTRACT_FLAG;
	return 8;
}



uint8_t cpu::ADC_A_REG8(const uint8_t reg){
	uint8_t flag = (F & CARRY_FLAG)? 1:0;

	uint16_t res = A+reg+flag;

	if( ((A & 0xF) + (reg & 0xF) + flag) & 1 << 4)
		F |= HALF_CARRY_FLAG;
	else
		F &= ~HALF_CARRY_FLAG;

	if(res & 1 << 8)
		F |= CARRY_FLAG;
	else
		F &= ~CARRY_FLAG;

	A+=reg+flag;

	if(A == 0)
		F |= ZERO_FLAG;
	else 
		F &= ~ZERO_FLAG;

	F &= ~SUBTRACT_FLAG;
	return 4;
	
}

uint8_t cpu::ADC_A_HLADDR(){
	uint8_t flag = (F & CARRY_FLAG)? 1:0;

	uint16_t res = A+(*memory)[HL]+flag;

	if( ((A & 0xF) + ((*memory)[HL] & 0xF) + flag) & 1 << 4)
		F |= HALF_CARRY_FLAG;
	else
		F &= ~HALF_CARRY_FLAG;

	if(res & 1 << 8)
		F |= CARRY_FLAG;
	else
		F &= ~CARRY_FLAG;

	A+=(*memory)[HL]+flag;

	if(A == 0)
		F |= ZERO_FLAG;
	else 
		F &= ~ZERO_FLAG;

	F &= ~SUBTRACT_FLAG;

	return 8;
	
}



uint8_t cpu::SUB_REG8(const uint8_t reg){
	if(reg > A)
		F |= CARRY_FLAG;
	else
		F &= ~CARRY_FLAG;

	if((reg & 0xF) > (A & 0xF))
		F |= HALF_CARRY_FLAG;
	else
		F &= ~HALF_CARRY_FLAG;	

	A-=reg;

	if(A == 0)
		F |= ZERO_FLAG;
	else
		F &= ~ZERO_FLAG;

	F |= SUBTRACT_FLAG;

	return 4;
}



uint8_t cpu::SUB_HLADDR(){
	if((*memory)[HL] > A)
		F |= CARRY_FLAG;
	else
		F &= ~CARRY_FLAG;

	if(((*memory)[HL] & 0xF) > (A & 0xF))
		F |= HALF_CARRY_FLAG;
	else
		F &= ~HALF_CARRY_FLAG;	

	A-=(*memory)[HL];

	if(A == 0)
		F |= ZERO_FLAG;
	else
		F &= ~ZERO_FLAG;

	F |= SUBTRACT_FLAG;

	return 8;
}

uint8_t cpu::SBC_REG8(const uint8_t reg){
	uint8_t flag = (F & CARRY_FLAG)? 1:0;

	if(reg + flag > A)
		F |= CARRY_FLAG;
	else
		F &= ~CARRY_FLAG;

	if((reg & 0xF) + flag > (A & 0xF))
		F |= HALF_CARRY_FLAG;
	else
		F &= ~HALF_CARRY_FLAG;	

	A-=(reg+flag);

	if(A == 0)
		F |= ZERO_FLAG;
	else
		F &= ~ZERO_FLAG;

	F |= SUBTRACT_FLAG;

	return 4;

}


uint8_t cpu::SBC_HLADDR(){
	uint8_t flag = (F & CARRY_FLAG)? 1:0;

	if((*memory)[HL] + flag > A)
		F |= CARRY_FLAG;
	else
		F &= ~CARRY_FLAG;

	if(((*memory)[HL] & 0xF) + flag > (A & 0xF))
		F |= HALF_CARRY_FLAG;
	else
		F &= ~HALF_CARRY_FLAG;	

	A-=((*memory)[HL]+flag);

	if(A == 0)
		F |= ZERO_FLAG;
	else
		F &= ~ZERO_FLAG;

	F |= SUBTRACT_FLAG;

	return 8;

}

uint8_t cpu::AND_REG8(const uint8_t reg){
	A &= reg;
	if(A==0)
		F |= ZERO_FLAG;
	else
		F &= ~ZERO_FLAG;

	F |= HALF_CARRY_FLAG;
	F &= ~SUBTRACT_FLAG;
	F &= ~CARRY_FLAG;

	return 4;
}


uint8_t cpu::AND_HLADDR(){
	A &= (*memory)[HL];
	if(A==0)
		F |= ZERO_FLAG;
	else
		F &= ~ZERO_FLAG;

	F |= HALF_CARRY_FLAG;
	F &= ~SUBTRACT_FLAG;
	F &= ~CARRY_FLAG;

	return 8;
}


uint8_t cpu::XOR_REG8(const uint8_t reg){
	A ^= reg;
	if(A == 0)
		F |= ZERO_FLAG;
	else
		F &= ~ZERO_FLAG;

	F &= ~CARRY_FLAG;
	F &= ~HALF_CARRY_FLAG;
	F &= ~SUBTRACT_FLAG;

	return 4;
}


uint8_t cpu::XOR_HLADDR(){
	A ^= (*memory)[HL];
	if(A == 0)
		F |= ZERO_FLAG;
	else
		F &= ~ZERO_FLAG;

	F &= ~CARRY_FLAG;
	F &= ~HALF_CARRY_FLAG;
	F &= ~SUBTRACT_FLAG;

	return 8;
}
	
uint8_t cpu::OR_REG8(const uint8_t reg){
	A |= reg;
	if(A == 0)
		F |= ZERO_FLAG;
	else
		F &= ~ZERO_FLAG;

	F &= ~CARRY_FLAG;
	F &= ~HALF_CARRY_FLAG;
	F &= ~SUBTRACT_FLAG;

	return 4;
}


uint8_t cpu::OR_HLADDR(){
	A |= (*memory)[HL];
	if(A == 0)
		F |= ZERO_FLAG;
	else
		F &= ~ZERO_FLAG;

	F &= ~CARRY_FLAG;
	F &= ~HALF_CARRY_FLAG;
	F &= ~SUBTRACT_FLAG;

	return 8;
}


uint8_t cpu::CP_REG8(const uint8_t reg){
	if(reg > A)
		F |= CARRY_FLAG;
	else
		F &= ~CARRY_FLAG;

	if((reg & 0xF) > (A & 0xF))
		F |= HALF_CARRY_FLAG;
	else
		F &= ~HALF_CARRY_FLAG;	


	if(A == reg)
		F |= ZERO_FLAG;
	else
		F &= ~ZERO_FLAG;

	F |= SUBTRACT_FLAG;


	return 4;
}



uint8_t cpu::CP_HLADDR(){
	if((*memory)[HL] > A)
		F |= CARRY_FLAG;
	else
		F &= ~CARRY_FLAG;

	if(((*memory)[HL] & 0xF) > (A & 0xF))
		F |= HALF_CARRY_FLAG;
	else
		F &= ~HALF_CARRY_FLAG;	


	if(A == (*memory)[HL])
		F |= ZERO_FLAG;
	else
		F &= ~ZERO_FLAG;

	F |= SUBTRACT_FLAG;


	return 8;
}


uint8_t cpu::RET_NZ(){

	if(F & ZERO_FLAG)
		return 8;
	else{
		uint8_t tmp = (*memory)[SP++];
		PC = ((*memory)[SP++]) << 8 | tmp;
		return 20;
	}
}


uint8_t cpu::RET_NC(){

	if(F & CARRY_FLAG)
		return 8;
	else{
		uint8_t tmp = (*memory)[SP++];
		PC = ((*memory)[SP++]) << 8 | tmp;
		return 20;
	}
}


uint8_t cpu::POP(uint8_t &reg_high, uint8_t &reg_low){
	reg_low = (*memory)[SP++];
	reg_high = (*memory)[SP++];
	F &= 0xF0;
	return 12;
}

uint8_t cpu::JP_NZ(){
	uint8_t tmpl = (*memory)[PC++];
	uint8_t tmph = (*memory)[PC++];
	if(F & ZERO_FLAG)
		return 12;
	else{
		PC = (tmph << 8) | tmpl;
		return 16;
	}
}



uint8_t cpu::JP_NC(){
	uint8_t tmpl = (*memory)[PC++];
	uint8_t tmph = (*memory)[PC++];
	if(F & CARRY_FLAG)
		return 12;
	else{
		PC = (tmph << 8) | tmpl;
		return 16;
	}
}

uint8_t cpu::JP(){
	uint8_t tmpl = (*memory)[PC++];
	uint8_t tmph = (*memory)[PC++];
	PC = (tmph << 8) | tmpl;
	return 16;	
}

uint8_t cpu::CALL_NZ(){
	uint8_t tmpl = (*memory)[PC++];
	uint8_t tmph = (*memory)[PC++];
	if(F & ZERO_FLAG)
		return 12;
	else{
		uint16_t pctmp = PC;//+1;
		memory->write(--SP, pctmp >> 8);
		memory->write(--SP, pctmp & 0xFF);
		PC = (tmph << 8) | tmpl;
		return 24;
	}

}

uint8_t cpu::CALL_NC(){
	uint8_t tmpl = (*memory)[PC++];
	uint8_t tmph = (*memory)[PC++];
	if(F & CARRY_FLAG)
		return 12;
	else{
		uint16_t pctmp = PC;//+1;
		memory->write(--SP, pctmp >> 8);
		memory->write(--SP, pctmp & 0xFF);
		PC = (tmph << 8) | tmpl;
		return 24;
	}

}

uint8_t cpu::PUSH(const uint16_t reg){
	memory->write(--SP, reg >> 8);
	memory->write(--SP, reg & 0xFF);
	return 16;
}


uint8_t cpu::ADD_A_d8(){
	uint8_t tmp = (*memory)[PC++];
	uint16_t res = A+tmp;
	if( ((A & 0xF) + (tmp & 0xF)) & 1 << 4)
		F |= HALF_CARRY_FLAG;
	else 
		F &= ~HALF_CARRY_FLAG;

	if(res & 1 << 8)
		F |= CARRY_FLAG;
	else 
		F &= ~CARRY_FLAG;

	A+=tmp;
	if(A == 0)
		F |= ZERO_FLAG;
	else 
		F &= ~ZERO_FLAG;

	F &= ~SUBTRACT_FLAG;
	return 8;
}


uint8_t cpu::SUB_A_d8(){
	uint8_t tmp = (*memory)[PC++];
	if(tmp > A)
		F |= CARRY_FLAG;
	else
		F &= ~CARRY_FLAG;

	if((tmp & 0xF) > (A & 0xF))
		F |= HALF_CARRY_FLAG;
	else
		F &= ~HALF_CARRY_FLAG;	

	A-=tmp;

	if(A == 0)
		F |= ZERO_FLAG;
	else
		F &= ~ZERO_FLAG;

	F |= SUBTRACT_FLAG;

	return 8;
}

uint8_t cpu::AND_A_d8(){
	uint8_t tmp = (*memory)[PC++];
	A &= tmp;
	if(A==0)
		F |= ZERO_FLAG;
	else
		F &= ~ZERO_FLAG;

	F |= HALF_CARRY_FLAG;
	F &= ~SUBTRACT_FLAG;
	F &= ~CARRY_FLAG;

	return 8;
}

uint8_t cpu::OR_A_d8(){
	uint8_t tmp = (*memory)[PC++];
	A |= tmp;
	if(A == 0)
		F |= ZERO_FLAG;
	else
		F &= ~ZERO_FLAG;

	F &= ~CARRY_FLAG;
	F &= ~HALF_CARRY_FLAG;
	F &= ~SUBTRACT_FLAG;

	return 8;
}

uint8_t cpu::RST(uint8_t address){
	PUSH(PC);
	PC = address;
	return 16;
}


uint8_t cpu::RET_Z(){

	if((F & ZERO_FLAG)==0)
		return 8;
	else{
		uint8_t tmp = (*memory)[SP++];
		PC = ((*memory)[SP++]) << 8 | tmp;
		return 20;
	}
}

uint8_t cpu::RET_C(){

	if((F & CARRY_FLAG)==0)
		return 8;
	else{
		uint8_t tmp = (*memory)[SP++];
		PC = ((*memory)[SP++]) << 8 | tmp;
		return 20;
	}
}

uint8_t cpu::RET(){
		uint8_t tmp = (*memory)[SP++];
		PC = ((*memory)[SP++]) << 8 | tmp;
		return 16;
}


uint8_t cpu::RETI(){
	uint8_t tmp = (*memory)[SP++];
	PC = ((*memory)[SP++]) << 8 | tmp;
	IME = true;
	return 16;
}

uint8_t cpu::JP_Z(){
	uint8_t tmpl = (*memory)[PC++];
	uint8_t tmph = (*memory)[PC++];
	if((F & ZERO_FLAG)==0)
		return 12;
	else{
		PC = (tmph << 8) | tmpl;
		return 16;
	}
}

uint8_t cpu::JP_C(){
	uint8_t tmpl = (*memory)[PC++];
	uint8_t tmph = (*memory)[PC++];
	if((F & CARRY_FLAG)==0)
		return 12;
	else{
		PC = (tmph << 8) | tmpl;
		return 16;
	}
}

uint8_t cpu::JP_HLADDR(){
	PC = HL;
	return 4;
}


uint8_t cpu::PREFIX_CB(){
return (this->*CB_array[(*memory)[PC++]])();

}

uint8_t cpu::CALL_Z(){

	uint8_t tmpl = (*memory)[PC++];
	uint8_t tmph = (*memory)[PC++];
	if((F & ZERO_FLAG)==0)
		return 12;
	else{
		uint16_t pctmp = PC;//+1;
		memory->write(--SP, pctmp >> 8);
		memory->write(--SP, pctmp & 0xFF);
		PC = (tmph << 8) | tmpl;
		return 24;
	}
}


uint8_t cpu::CALL_C(){
	uint8_t tmpl = (*memory)[PC++];
	uint8_t tmph = (*memory)[PC++];
	if((F & CARRY_FLAG)==0)
		return 12;
	else{
		uint16_t pctmp = PC;//+1;
		memory->write(--SP, pctmp >> 8);
		memory->write(--SP, pctmp & 0xFF);
		PC = (tmph << 8) | tmpl;
		return 24;
	}
}

uint8_t cpu::CALL(){
	uint8_t tmpl = (*memory)[PC++];
	uint8_t tmph = (*memory)[PC++];

	uint16_t pctmp = PC;
	memory->write(--SP, pctmp >> 8);
	memory->write(--SP, pctmp & 0xFF);
	PC = (tmph << 8) | tmpl;
	return 24;
}


uint8_t cpu::ADC_A_d8(){
	uint8_t flag = (F & CARRY_FLAG)? 1:0;
	uint8_t val = (*memory)[PC++];
	uint16_t res = A+val+flag;

	if( ((A & 0xF) + (val & 0xF) + flag) & 1 << 4)
		F |= HALF_CARRY_FLAG;
	else
		F &= ~HALF_CARRY_FLAG;

	if(res & 1 << 8)
		F |= CARRY_FLAG;
	else
		F &= ~CARRY_FLAG;

	A+=val+flag;

	if(A == 0)
		F |= ZERO_FLAG;
	else 
		F &= ~ZERO_FLAG;

	F &= ~SUBTRACT_FLAG;

	return 8;
	
}

uint8_t cpu::SBC_A_d8(){
	uint8_t flag = (F & CARRY_FLAG)? 1:0;
	uint8_t val = (*memory)[PC++];

	if(val + flag > A)
		F |= CARRY_FLAG;
	else
		F &= ~CARRY_FLAG;

	if((val & 0xF)+flag > (A & 0xF))
		F |= HALF_CARRY_FLAG;
	else
		F &= ~HALF_CARRY_FLAG;	

	A-=(val+flag);

	if(A == 0)
		F |= ZERO_FLAG;
	else
		F &= ~ZERO_FLAG;

	F |= SUBTRACT_FLAG;

	return 8;

}

uint8_t cpu::XOR_A_d8(){
	uint8_t val = (*memory)[PC++];
	A ^= val;
	if(A == 0)
		F |= ZERO_FLAG;
	else
		F &= ~ZERO_FLAG;

	F &= ~CARRY_FLAG;
	F &= ~HALF_CARRY_FLAG;
	F &= ~SUBTRACT_FLAG;

	return 8;
}

uint8_t cpu::CP_A_d8(){
	uint8_t val = (*memory)[PC++];

	if(val > A)
		F |= CARRY_FLAG;
	else
		F &= ~CARRY_FLAG;

	if((val & 0xF) > (A & 0xF))
		F |= HALF_CARRY_FLAG;
	else
		F &= ~HALF_CARRY_FLAG;	


	if(A == val)
		F |= ZERO_FLAG;
	else
		F &= ~ZERO_FLAG;

	F |= SUBTRACT_FLAG;

	return 8;
}

uint8_t cpu::LDH_a8_A(){
	uint8_t val = (*memory)[PC++];
	memory->write(0xFF00 + val, A);
	return 12;
}

uint8_t cpu::LDH_A_a8(){
	uint8_t val = (*memory)[PC++];
	A = (*memory)[0xFF00 + val];
	return 12;
}


uint8_t cpu::LD_FFC_A(){
	memory->write(0xFF00 + C, A);
	return 8;
}

uint8_t cpu::LD_A_FFC(){
	A = (*memory)[0xFF00 + C];
	return 8;
}

uint8_t cpu::DI(){
	EI_scheduled = false;
	IME = false;
	return 4;
}

uint8_t cpu::ADD_SP_r8(){
	uint8_t val = (*memory)[PC++];
	if( ((SP & 0xF) + (val & 0xF)) & 1 << 4)
		F |= HALF_CARRY_FLAG;
	else
		F &= ~HALF_CARRY_FLAG;

	uint16_t res = (SP & 0xFF) + val;

	if(res & 1 << 8)
		F |= CARRY_FLAG;
	else
		F &= ~CARRY_FLAG;
	
	SP += (int8_t)val;

	F &= ~ZERO_FLAG;
	F &= ~SUBTRACT_FLAG;
	return 16;
}

uint8_t cpu::LD_HL_SPr8(){
	uint16_t SP_tmp = SP;
	ADD_SP_r8();
	H = ((SP & 0xFF00)>>8);
	L = (SP & 0xFF);
	SP = SP_tmp;
	return 12;
}

uint8_t cpu::LD_SP_HL(){
	SP = HL;
	return 8;
}

uint8_t cpu::LD_ADDR_A(){
	uint8_t tmpl = (*memory)[PC++];
	uint8_t tmph = (*memory)[PC++];
	memory->write((tmph << 8) | tmpl, A);
	return 16;
}


uint8_t cpu::LD_A_ADDR(){
	uint8_t tmpl = (*memory)[PC++];
	uint8_t tmph = (*memory)[PC++];
	A = (*memory)[(tmph << 8) | tmpl];
	return 16;
}

uint8_t cpu::EI(){
	EI_scheduled = true;
	return 4;
}

uint8_t cpu::ERR(){
	throw std::runtime_error("Illegal opcode.");
	return 0;
}


uint8_t cpu::RLC(uint8_t &reg){

	if(reg  & 1 << 7)
		F |= CARRY_FLAG;
	else
		F &= ~CARRY_FLAG;

	reg = (reg << 1);

	if(F & CARRY_FLAG)
		reg |= 1 << 0;
	

	if(reg == 0)
		F |= ZERO_FLAG;
	else
		F &= ~ZERO_FLAG;

	F &= ~SUBTRACT_FLAG;

	F &= ~HALF_CARRY_FLAG;
	
	return 8;
}


uint8_t cpu::RLC_HLADDR(){

	uint8_t tmp = (*memory)[HL];

	if(tmp  & 1 << 7)
		F |= CARRY_FLAG;
	else
		F &= ~CARRY_FLAG;

	memory->write(HL, tmp << 1);
	if(F & CARRY_FLAG)
		memory->write(HL, (*memory)[HL] | 1 << 0);

	if((*memory)[HL] == 0)
		F |= ZERO_FLAG;
	else
		F &= ~ZERO_FLAG;

	F &= ~SUBTRACT_FLAG;

	F &= ~HALF_CARRY_FLAG;
	
	return 16;
}

uint8_t cpu::RRC(uint8_t &reg){

	if(reg  & 1 << 0)
		F |= CARRY_FLAG;
	else
		F &= ~CARRY_FLAG;

	reg = (reg >> 1);
	if(F & CARRY_FLAG)
		reg |= 1 << 7;
	

	if(reg == 0)
		F |= ZERO_FLAG;
	else
		F &= ~ZERO_FLAG;

	F &= ~SUBTRACT_FLAG;

	F &= ~HALF_CARRY_FLAG;
	
	return 8;
}

uint8_t cpu::RRC_HLADDR(){

	uint8_t tmp = (*memory)[HL];

	if(tmp  & 1 << 0)
		F |= CARRY_FLAG;
	else
		F &= ~CARRY_FLAG;

	memory->write(HL, tmp >> 1);
	if(F & CARRY_FLAG)
		memory->write(HL, (*memory)[HL] | 1 << 7);
	

	if((*memory)[HL] == 0)
		F |= ZERO_FLAG;
	else
		F &= ~ZERO_FLAG;

	F &= ~SUBTRACT_FLAG;

	F &= ~HALF_CARRY_FLAG;
	
	return 16;
}

uint8_t cpu::RL(uint8_t &reg){

	uint8_t bit = (F & CARRY_FLAG)? 1:0; 

	if(reg  & 1 << 7)
		F |= CARRY_FLAG;
	else
		F &= ~CARRY_FLAG;

	reg = (reg << 1);
	if(bit == 1)
		reg |= 1 << 0;
	

	if(reg == 0)
		F |= ZERO_FLAG;
	else
		F &= ~ZERO_FLAG;

	F &= ~SUBTRACT_FLAG;

	F &= ~HALF_CARRY_FLAG;
	
	return 8;
}

uint8_t cpu::RL_HLADDR(){

	uint8_t tmp = (*memory)[HL];
	uint8_t bit = (F & CARRY_FLAG)? 1:0;

	if(tmp  & 1 << 7)
		F |= CARRY_FLAG;
	else
		F &= ~CARRY_FLAG;

	memory->write(HL, tmp << 1);
	if(bit == 1)
		memory->write(HL, (*memory)[HL] | 1 << 0);

	if((*memory)[HL] == 0)
		F |= ZERO_FLAG;
	else
		F &= ~ZERO_FLAG;

	F &= ~SUBTRACT_FLAG;

	F &= ~HALF_CARRY_FLAG;
	
	return 16;
}


uint8_t cpu::RR(uint8_t &reg){

	uint8_t bit = (F & CARRY_FLAG)? 1:0; 

	if(reg  & 1 << 0)
		F |= CARRY_FLAG;
	else
		F &= ~CARRY_FLAG;

	reg = (reg >> 1);
	if(bit == 1)
		reg |= 1 << 7;
	

	if(reg == 0)
		F |= ZERO_FLAG;
	else
		F &= ~ZERO_FLAG;

	F &= ~SUBTRACT_FLAG;

	F &= ~HALF_CARRY_FLAG;
	
	return 8;
}


uint8_t cpu::RR_HLADDR(){

	uint8_t tmp = (*memory)[HL];
	uint8_t bit = (F & CARRY_FLAG)? 1:0;

	if(tmp  & 1 << 0)
		F |= CARRY_FLAG;
	else
		F &= ~CARRY_FLAG;

	memory->write(HL, tmp >> 1);
	if(bit == 1)
		memory->write(HL, (*memory)[HL] | 1 << 7);

	if((*memory)[HL] == 0)
		F |= ZERO_FLAG;
	else
		F &= ~ZERO_FLAG;

	F &= ~SUBTRACT_FLAG;

	F &= ~HALF_CARRY_FLAG;
	
	return 16;
}

uint8_t cpu::SLA(uint8_t &reg){

	if(reg  & 1 << 7)
		F |= CARRY_FLAG;
	else
		F &= ~CARRY_FLAG;

	reg = (reg << 1);

	if(reg == 0)
		F |= ZERO_FLAG;
	else
		F &= ~ZERO_FLAG;

	F &= ~SUBTRACT_FLAG;

	F &= ~HALF_CARRY_FLAG;
	
	return 8;
}

uint8_t cpu::SLA_HLADDR(){

	uint8_t tmp = (*memory)[HL];

	if(tmp  & 1 << 7)
		F |= CARRY_FLAG;
	else
		F &= ~CARRY_FLAG;

	memory->write(HL, tmp << 1);

	if((*memory)[HL] == 0)
		F |= ZERO_FLAG;
	else
		F &= ~ZERO_FLAG;

	F &= ~SUBTRACT_FLAG;

	F &= ~HALF_CARRY_FLAG;
	
	return 16;
}

uint8_t cpu::SRA(uint8_t &reg){

	uint8_t bit = (reg & 1 << 7)? 1:0;
	if(reg  & 1 << 0)
		F |= CARRY_FLAG;
	else
		F &= ~CARRY_FLAG;

	reg = (reg >> 1);
	if(bit == 1)
		reg |= (1 << 7);

	if(reg == 0)
		F |= ZERO_FLAG;
	else
		F &= ~ZERO_FLAG;

	F &= ~SUBTRACT_FLAG;

	F &= ~HALF_CARRY_FLAG;
	
	return 8;
}

uint8_t cpu::SRA_HLADDR(){

	uint8_t tmp = (*memory)[HL];
	uint8_t bit = (tmp & 1 << 7)? 1:0;

	if(tmp  & 1 << 0)
		F |= CARRY_FLAG;
	else
		F &= ~CARRY_FLAG;

	memory->write(HL, tmp >> 1);
	if(bit == 1)
		memory->write(HL, (*memory)[HL] | (1 << 7));

	if((*memory)[HL] == 0)
		F |= ZERO_FLAG;
	else
		F &= ~ZERO_FLAG;

	F &= ~SUBTRACT_FLAG;

	F &= ~HALF_CARRY_FLAG;
	
	return 16;
}

uint8_t cpu::SWAP(uint8_t &reg){

	uint8_t tmpl = reg & 0xF;
	uint8_t tmph = reg & 0xF0;
	reg = (tmpl << 4) | (tmph >> 4);

	if(reg == 0)
		F |= ZERO_FLAG;
	else
		F &= ~ZERO_FLAG;

	F &= ~SUBTRACT_FLAG;
	F &= ~CARRY_FLAG;
	F &= ~HALF_CARRY_FLAG;

	return 8;
}

uint8_t cpu::SWAP_HLADDR(){

	uint8_t tmpl = (*memory)[HL] & 0xF;
	uint8_t tmph = (*memory)[HL] & 0xF0;
	memory->write(HL, (tmpl << 4) | (tmph >> 4));

	if((*memory)[HL] == 0)
		F |= ZERO_FLAG;
	else
		F &= ~ZERO_FLAG;

	F &= ~SUBTRACT_FLAG;
	F &= ~CARRY_FLAG;
	F &= ~HALF_CARRY_FLAG;

	return 16;
}

uint8_t cpu::SRL(uint8_t &reg){

	if(reg  & 1 << 0)
		F |= CARRY_FLAG;
	else
		F &= ~CARRY_FLAG;

	reg = (reg >> 1);

	if(reg == 0)
		F |= ZERO_FLAG;
	else
		F &= ~ZERO_FLAG;

	F &= ~SUBTRACT_FLAG;

	F &= ~HALF_CARRY_FLAG;
	
	return 8;
}
	

uint8_t cpu::SRL_HLADDR(){

	uint8_t tmp = (*memory)[HL];

	if(tmp  & 1 << 0)
		F |= CARRY_FLAG;
	else
		F &= ~CARRY_FLAG;

	memory->write(HL, tmp >> 1);

	if((*memory)[HL] == 0)
		F |= ZERO_FLAG;
	else
		F &= ~ZERO_FLAG;

	F &= ~SUBTRACT_FLAG;

	F &= ~HALF_CARRY_FLAG;
	
	return 16;
}

uint8_t cpu::BIT(const uint8_t bit, const uint8_t reg){

	if(reg & 1 << bit)
		F &= ~ZERO_FLAG;
	else
		F |= ZERO_FLAG;

	F &= ~SUBTRACT_FLAG;
	F |= HALF_CARRY_FLAG;

	return 8;
}

	

uint8_t cpu::BIT_HLADDR(const uint8_t bit){

	if((*memory)[HL] & 1 << bit)
		F &= ~ZERO_FLAG;
	else
		F |= ZERO_FLAG;

	F &= ~SUBTRACT_FLAG;
	F |= HALF_CARRY_FLAG;

	return 12; 
}

uint8_t cpu::RES(const uint8_t bit, uint8_t &reg){
	reg &= ~(1 << bit);
	return 8;
}

	
uint8_t cpu::RES_HLADDR(const uint8_t bit){
	memory->write(HL, (*memory)[HL] & ~(1 << bit));
	return 16;
}

uint8_t cpu::SET(const uint8_t bit, uint8_t &reg){
	reg |= (1 << bit);
	return 8;
}
	
uint8_t cpu::SET_HLADDR(const uint8_t bit){
	memory->write(HL, (*memory)[HL] | (1 << bit));
	return 16;
}


//*************************************************************************


//********instruction set**************************************************



uint8_t cpu::inst00(){
return NOP();
}

uint8_t cpu::inst01(){
return LD_REG16_d16(B,C);
}

uint8_t cpu::inst02(){
return LD_REG16_A(BC);
}

uint8_t cpu::inst03(){
return INC_REG16(B,C);
}

uint8_t cpu::inst04(){
return INC_REG8(B);
}

uint8_t cpu::inst05(){
return DEC_REG8(B);
}

uint8_t cpu::inst06(){
return LD_REG8_d8(B);
}

uint8_t cpu::inst07(){
return RLCA();
}

uint8_t cpu::inst08(){
return LD_a16_SP();
}

uint8_t cpu::inst09(){
return ADD_HL_REG16(BC);
}

uint8_t cpu::inst0A(){
return LD_A_ADDR(BC);
}

uint8_t cpu::inst0B(){
return DEC_REG16(B,C);
}

uint8_t cpu::inst0C(){
return INC_REG8(C);
}

uint8_t cpu::inst0D(){
return DEC_REG8(C);
}

uint8_t cpu::inst0E(){
return LD_REG8_d8(C);
}

uint8_t cpu::inst0F(){
return RRCA();
}

uint8_t cpu::inst10(){
return STOP();
}

uint8_t cpu::inst11(){
return LD_REG16_d16(D,E);
}

uint8_t cpu::inst12(){
return LD_REG16_A(DE);
}

uint8_t cpu::inst13(){
return INC_REG16(D,E);
}

uint8_t cpu::inst14(){
return INC_REG8(D);
}

uint8_t cpu::inst15(){
return DEC_REG8(D);
}

uint8_t cpu::inst16(){
return LD_REG8_d8(D);
}

uint8_t cpu::inst17(){
return RLA();
}

uint8_t cpu::inst18(){
return JR_r8();
}

uint8_t cpu::inst19(){
return ADD_HL_REG16(DE);
}

uint8_t cpu::inst1A(){
return LD_A_ADDR(DE);
}

uint8_t cpu::inst1B(){
return DEC_REG16(D,E);
}

uint8_t cpu::inst1C(){
return INC_REG8(E);
}

uint8_t cpu::inst1D(){
return DEC_REG8(E);
}

uint8_t cpu::inst1E(){
return LD_REG8_d8(E);
}

uint8_t cpu::inst1F(){
return RRA();
}

uint8_t cpu::inst20(){
return JR_NZ_r8();
}

uint8_t cpu::inst21(){
return LD_REG16_d16(H,L);
}

uint8_t cpu::inst22(){
return LD_HLINC_A();
}

uint8_t cpu::inst23(){
return INC_REG16(H,L);
}

uint8_t cpu::inst24(){
return INC_REG8(H);
}

uint8_t cpu::inst25(){
return DEC_REG8(H);
}

uint8_t cpu::inst26(){
return LD_REG8_d8(H);
}

uint8_t cpu::inst27(){
return DAA();
}

uint8_t cpu::inst28(){
return JR_Z_r8();
}

uint8_t cpu::inst29(){
return ADD_HL_REG16(HL);
}

uint8_t cpu::inst2A(){
return LD_A_HLINC();
}

uint8_t cpu::inst2B(){
return DEC_REG16(H,L);
}

uint8_t cpu::inst2C(){
return INC_REG8(L);
}

uint8_t cpu::inst2D(){
return DEC_REG8(L);
}

uint8_t cpu::inst2E(){
return LD_REG8_d8(L);
}

uint8_t cpu::inst2F(){
return CPL();
}

uint8_t cpu::inst30(){
return JR_NC_r8();
}

uint8_t cpu::inst31(){
return LD_SP_d16();
}

uint8_t cpu::inst32(){
return LD_HLDEC_A();
}

uint8_t cpu::inst33(){
return INC_SP();
}

uint8_t cpu::inst34(){
return INC_HLADDR();
}

uint8_t cpu::inst35(){
return DEC_HLADDR();
}

uint8_t cpu::inst36(){
return LD_HLADDR_d8();
}

uint8_t cpu::inst37(){
return SCF();
}

uint8_t cpu::inst38(){
return JR_C_r8();
}

uint8_t cpu::inst39(){
return ADD_HL_REG16(SP);
}

uint8_t cpu::inst3A(){
return LD_A_HLDEC();
}

uint8_t cpu::inst3B(){
return DEC_SP();
}

uint8_t cpu::inst3C(){
return INC_REG8(A);
}

uint8_t cpu::inst3D(){
return DEC_REG8(A);
}

uint8_t cpu::inst3E(){
return LD_REG8_d8(A);
}

uint8_t cpu::inst3F(){
return CCF();
}

uint8_t cpu::inst40(){
return LD_REG8_REG8(B,B);
}

uint8_t cpu::inst41(){
return LD_REG8_REG8(B,C);
}

uint8_t cpu::inst42(){
return LD_REG8_REG8(B,D);
}

uint8_t cpu::inst43(){
return LD_REG8_REG8(B,E);
}

uint8_t cpu::inst44(){
return LD_REG8_REG8(B,H);
}

uint8_t cpu::inst45(){
return LD_REG8_REG8(B,L);
}

uint8_t cpu::inst46(){
return LD_REG8_HLADDR(B);
}

uint8_t cpu::inst47(){
return LD_REG8_REG8(B,A);
}

uint8_t cpu::inst48(){
return LD_REG8_REG8(C,B);
}

uint8_t cpu::inst49(){
return LD_REG8_REG8(C,C);
}

uint8_t cpu::inst4A(){
return LD_REG8_REG8(C,D);
}

uint8_t cpu::inst4B(){
return LD_REG8_REG8(C,E);
}

uint8_t cpu::inst4C(){
return LD_REG8_REG8(C,H);
}

uint8_t cpu::inst4D(){
return LD_REG8_REG8(C,L);
}

uint8_t cpu::inst4E(){
return LD_REG8_HLADDR(C);
}

uint8_t cpu::inst4F(){
return LD_REG8_REG8(C,A);
}

uint8_t cpu::inst50(){
return LD_REG8_REG8(D,B);
}

uint8_t cpu::inst51(){
return LD_REG8_REG8(D,C);
}

uint8_t cpu::inst52(){
return LD_REG8_REG8(D,D);
}

uint8_t cpu::inst53(){
return LD_REG8_REG8(D,E);
}

uint8_t cpu::inst54(){
return LD_REG8_REG8(D,H);
}

uint8_t cpu::inst55(){
return LD_REG8_REG8(D,L);
}

uint8_t cpu::inst56(){
return LD_REG8_HLADDR(D);
}

uint8_t cpu::inst57(){
return LD_REG8_REG8(D,A);
}

uint8_t cpu::inst58(){
return LD_REG8_REG8(E,B);
}

uint8_t cpu::inst59(){
return LD_REG8_REG8(E,C);
}

uint8_t cpu::inst5A(){
return LD_REG8_REG8(E,D);
}

uint8_t cpu::inst5B(){
return LD_REG8_REG8(E,E);
}

uint8_t cpu::inst5C(){
return LD_REG8_REG8(E,H);
}

uint8_t cpu::inst5D(){
return LD_REG8_REG8(E,L);
}

uint8_t cpu::inst5E(){
return LD_REG8_HLADDR(E);
}

uint8_t cpu::inst5F(){
return LD_REG8_REG8(E,A);
}

uint8_t cpu::inst60(){
return LD_REG8_REG8(H,B);
}

uint8_t cpu::inst61(){
return LD_REG8_REG8(H,C);
}

uint8_t cpu::inst62(){
return LD_REG8_REG8(H,D);
}

uint8_t cpu::inst63(){
return LD_REG8_REG8(H,E);
}

uint8_t cpu::inst64(){
return LD_REG8_REG8(H,H);
}

uint8_t cpu::inst65(){
return LD_REG8_REG8(H,L);
}

uint8_t cpu::inst66(){
return LD_REG8_HLADDR(H);
}

uint8_t cpu::inst67(){
return LD_REG8_REG8(H,A);
}

uint8_t cpu::inst68(){
return LD_REG8_REG8(L,B);
}

uint8_t cpu::inst69(){
return LD_REG8_REG8(L,C);
}

uint8_t cpu::inst6A(){
return LD_REG8_REG8(L,D);
}

uint8_t cpu::inst6B(){
return LD_REG8_REG8(L,E);
}

uint8_t cpu::inst6C(){
return LD_REG8_REG8(L,H);
}

uint8_t cpu::inst6D(){
return LD_REG8_REG8(L,L);
}

uint8_t cpu::inst6E(){
return LD_REG8_HLADDR(L);
}

uint8_t cpu::inst6F(){
return LD_REG8_REG8(L,A);
}

uint8_t cpu::inst70(){
return LD_HLADDR_REG8(B);
}

uint8_t cpu::inst71(){
return LD_HLADDR_REG8(C);
}

uint8_t cpu::inst72(){
return LD_HLADDR_REG8(D);
}

uint8_t cpu::inst73(){
return LD_HLADDR_REG8(E);
}

uint8_t cpu::inst74(){
return LD_HLADDR_REG8(H);
}

uint8_t cpu::inst75(){
return LD_HLADDR_REG8(L);
}

uint8_t cpu::inst76(){
return HALT();
}

uint8_t cpu::inst77(){
return LD_HLADDR_REG8(A);
}

uint8_t cpu::inst78(){
return LD_REG8_REG8(A,B);
}

uint8_t cpu::inst79(){
return LD_REG8_REG8(A,C);
}

uint8_t cpu::inst7A(){
return LD_REG8_REG8(A,D);
}

uint8_t cpu::inst7B(){
return LD_REG8_REG8(A,E);
}

uint8_t cpu::inst7C(){
return LD_REG8_REG8(A,H);
}

uint8_t cpu::inst7D(){
return LD_REG8_REG8(A,L);
}

uint8_t cpu::inst7E(){
return LD_REG8_HLADDR(A);
}

uint8_t cpu::inst7F(){
return LD_REG8_REG8(A,A);
}

uint8_t cpu::inst80(){
return ADD_A_REG8(B);
}

uint8_t cpu::inst81(){
return ADD_A_REG8(C);
}

uint8_t cpu::inst82(){
return ADD_A_REG8(D);
}

uint8_t cpu::inst83(){
return ADD_A_REG8(E);
}

uint8_t cpu::inst84(){
return ADD_A_REG8(H);
}

uint8_t cpu::inst85(){
return ADD_A_REG8(L);
}

uint8_t cpu::inst86(){
return ADD_A_HLADDR();
}

uint8_t cpu::inst87(){
return ADD_A_REG8(A);
}

uint8_t cpu::inst88(){
return ADC_A_REG8(B);
}

uint8_t cpu::inst89(){
return ADC_A_REG8(C);
}

uint8_t cpu::inst8A(){
return ADC_A_REG8(D);
}

uint8_t cpu::inst8B(){
return ADC_A_REG8(E);
}

uint8_t cpu::inst8C(){
return ADC_A_REG8(H);
}

uint8_t cpu::inst8D(){
return ADC_A_REG8(L);
}

uint8_t cpu::inst8E(){
return ADC_A_HLADDR();
}

uint8_t cpu::inst8F(){
return ADC_A_REG8(A);
}

uint8_t cpu::inst90(){
return SUB_REG8(B);
}

uint8_t cpu::inst91(){
return SUB_REG8(C);
}

uint8_t cpu::inst92(){
return SUB_REG8(D);
}

uint8_t cpu::inst93(){
return SUB_REG8(E);
}

uint8_t cpu::inst94(){
return SUB_REG8(H);
}

uint8_t cpu::inst95(){
return SUB_REG8(L);
}

uint8_t cpu::inst96(){
return SUB_HLADDR();
}

uint8_t cpu::inst97(){
return SUB_REG8(A);
}

uint8_t cpu::inst98(){
return SBC_REG8(B);
}

uint8_t cpu::inst99(){
return SBC_REG8(C);
}

uint8_t cpu::inst9A(){
return SBC_REG8(D);
}

uint8_t cpu::inst9B(){
return SBC_REG8(E);
}

uint8_t cpu::inst9C(){
return SBC_REG8(H);
}

uint8_t cpu::inst9D(){
return SBC_REG8(L);
}

uint8_t cpu::inst9E(){
return SBC_HLADDR();
}

uint8_t cpu::inst9F(){
return SBC_REG8(A);
}

uint8_t cpu::instA0(){
return AND_REG8(B);
}

uint8_t cpu::instA1(){
return AND_REG8(C);
}

uint8_t cpu::instA2(){
return AND_REG8(D);
}

uint8_t cpu::instA3(){
return AND_REG8(E);
}

uint8_t cpu::instA4(){
return AND_REG8(H);
}

uint8_t cpu::instA5(){
return AND_REG8(L);
}

uint8_t cpu::instA6(){
return AND_HLADDR();
}

uint8_t cpu::instA7(){
return AND_REG8(A);
}

uint8_t cpu::instA8(){
return XOR_REG8(B);
}

uint8_t cpu::instA9(){
return XOR_REG8(C);
}

uint8_t cpu::instAA(){
return XOR_REG8(D);
}

uint8_t cpu::instAB(){
return XOR_REG8(E);
}

uint8_t cpu::instAC(){
return XOR_REG8(H);
}

uint8_t cpu::instAD(){
return XOR_REG8(L);
}

uint8_t cpu::instAE(){
return XOR_HLADDR();
}

uint8_t cpu::instAF(){
return XOR_REG8(A);
}

uint8_t cpu::instB0(){
return OR_REG8(B);
}

uint8_t cpu::instB1(){
return OR_REG8(C);
}

uint8_t cpu::instB2(){
return OR_REG8(D);
}

uint8_t cpu::instB3(){
return OR_REG8(E);
}

uint8_t cpu::instB4(){
return OR_REG8(H);
}

uint8_t cpu::instB5(){
return OR_REG8(L);
}

uint8_t cpu::instB6(){
return OR_HLADDR();
}

uint8_t cpu::instB7(){
return OR_REG8(A);
}

uint8_t cpu::instB8(){
return CP_REG8(B);
}

uint8_t cpu::instB9(){
return CP_REG8(C);
}

uint8_t cpu::instBA(){
return CP_REG8(D);
}

uint8_t cpu::instBB(){
return CP_REG8(E);
}

uint8_t cpu::instBC(){
return CP_REG8(H);
}

uint8_t cpu::instBD(){
return CP_REG8(L);
}

uint8_t cpu::instBE(){
return CP_HLADDR();
}

uint8_t cpu::instBF(){
return CP_REG8(A);
}

uint8_t cpu::instC0(){
return RET_NZ();
}

uint8_t cpu::instC1(){
return POP(B,C);
}

uint8_t cpu::instC2(){
return JP_NZ();
}

uint8_t cpu::instC3(){
return JP();
}

uint8_t cpu::instC4(){
return CALL_NZ();
}

uint8_t cpu::instC5(){
return PUSH(BC);
}

uint8_t cpu::instC6(){
return ADD_A_d8();
}

uint8_t cpu::instC7(){
return RST(0x0);
}

uint8_t cpu::instC8(){
return RET_Z();
}

uint8_t cpu::instC9(){
return RET();
}

uint8_t cpu::instCA(){
return JP_Z();
}

uint8_t cpu::instCB(){
return PREFIX_CB();
}

uint8_t cpu::instCC(){
return CALL_Z();
}

uint8_t cpu::instCD(){
return CALL();
}

uint8_t cpu::instCE(){
return ADC_A_d8();
}

uint8_t cpu::instCF(){
return RST(0x8);
}

uint8_t cpu::instD0(){
return RET_NC();
}

uint8_t cpu::instD1(){
return POP(D,E);
}

uint8_t cpu::instD2(){
return JP_NC();
}

uint8_t cpu::instD3(){
return ERR();
}

uint8_t cpu::instD4(){
return CALL_NC();
}

uint8_t cpu::instD5(){
return PUSH(DE);
}

uint8_t cpu::instD6(){
return SUB_A_d8();
}

uint8_t cpu::instD7(){
return RST(0x10);
}

uint8_t cpu::instD8(){
return RET_C();
}

uint8_t cpu::instD9(){
return RETI();
}

uint8_t cpu::instDA(){
return JP_C();
}

uint8_t cpu::instDB(){
return ERR();
}

uint8_t cpu::instDC(){
return CALL_C();
}

uint8_t cpu::instDD(){
return ERR();
}

uint8_t cpu::instDE(){
return SBC_A_d8();
}

uint8_t cpu::instDF(){
return RST(0x18);
}

uint8_t cpu::instE0(){
return LDH_a8_A();
}

uint8_t cpu::instE1(){
return POP(H,L);
}

uint8_t cpu::instE2(){
return LD_FFC_A();
}

uint8_t cpu::instE3(){
return ERR();
}

uint8_t cpu::instE4(){
return ERR();
}

uint8_t cpu::instE5(){
return PUSH(HL);
}

uint8_t cpu::instE6(){
return AND_A_d8();
}

uint8_t cpu::instE7(){
return RST(0x20);
}

uint8_t cpu::instE8(){
return ADD_SP_r8();
}

uint8_t cpu::instE9(){
return JP_HLADDR();
}

uint8_t cpu::instEA(){
return LD_ADDR_A();
}

uint8_t cpu::instEB(){
return ERR();
}

uint8_t cpu::instEC(){
return ERR();
}

uint8_t cpu::instED(){
return ERR();
}

uint8_t cpu::instEE(){
return XOR_A_d8();
}

uint8_t cpu::instEF(){
return RST(0x28);
}

uint8_t cpu::instF0(){
return LDH_A_a8();
}

uint8_t cpu::instF1(){
return POP(A,F);
}

uint8_t cpu::instF2(){
return LD_A_FFC();
}

uint8_t cpu::instF3(){
return DI();
}

uint8_t cpu::instF4(){
return ERR();
}

uint8_t cpu::instF5(){
return PUSH(AF);
}

uint8_t cpu::instF6(){
return OR_A_d8();
}

uint8_t cpu::instF7(){
return RST(0x30);
}

uint8_t cpu::instF8(){
return LD_HL_SPr8();
}

uint8_t cpu::instF9(){
return LD_SP_HL();
}

uint8_t cpu::instFA(){
return LD_A_ADDR();
}

uint8_t cpu::instFB(){
return EI();
}

uint8_t cpu::instFC(){
return ERR();
}

uint8_t cpu::instFD(){
return ERR();
}

uint8_t cpu::instFE(){
return CP_A_d8();
}

uint8_t cpu::instFF(){
return RST(0x38);
}

//****************************************************************
//**************Prefix CB*****************************************

uint8_t cpu::CBinst00(){
return RLC(B);
}

uint8_t cpu::CBinst01(){
return RLC(C);
}

uint8_t cpu::CBinst02(){
return RLC(D);
}

uint8_t cpu::CBinst03(){
return RLC(E);
}

uint8_t cpu::CBinst04(){
return RLC(H);
}

uint8_t cpu::CBinst05(){
return RLC(L);
}

uint8_t cpu::CBinst06(){
return RLC_HLADDR();
}

uint8_t cpu::CBinst07(){
return RLC(A);
}

uint8_t cpu::CBinst08(){
return RRC(B);
}

uint8_t cpu::CBinst09(){
return RRC(C);
}

uint8_t cpu::CBinst0A(){
return RRC(D);
}

uint8_t cpu::CBinst0B(){
return RRC(E);
}

uint8_t cpu::CBinst0C(){
return RRC(H);
}

uint8_t cpu::CBinst0D(){
return RRC(L);
}

uint8_t cpu::CBinst0E(){
return RRC_HLADDR();
}

uint8_t cpu::CBinst0F(){
return RRC(A);
}

uint8_t cpu::CBinst10(){
return RL(B);
}

uint8_t cpu::CBinst11(){
return RL(C);
}

uint8_t cpu::CBinst12(){
return RL(D);
}

uint8_t cpu::CBinst13(){
return RL(E);
}

uint8_t cpu::CBinst14(){
return RL(H);
}

uint8_t cpu::CBinst15(){
return RL(L);
}

uint8_t cpu::CBinst16(){
return RL_HLADDR();
}

uint8_t cpu::CBinst17(){
return RL(A);
}

uint8_t cpu::CBinst18(){
return RR(B);
}

uint8_t cpu::CBinst19(){
return RR(C);
}

uint8_t cpu::CBinst1A(){
return RR(D);
}

uint8_t cpu::CBinst1B(){
return RR(E);
}

uint8_t cpu::CBinst1C(){
return RR(H);
}

uint8_t cpu::CBinst1D(){
return RR(L);
}

uint8_t cpu::CBinst1E(){
return RR_HLADDR();
}

uint8_t cpu::CBinst1F(){
return RR(A);
}

uint8_t cpu::CBinst20(){
return SLA(B);
}

uint8_t cpu::CBinst21(){
return SLA(C);
}

uint8_t cpu::CBinst22(){
return SLA(D);
}

uint8_t cpu::CBinst23(){
return SLA(E);
}

uint8_t cpu::CBinst24(){
return SLA(H);
}

uint8_t cpu::CBinst25(){
return SLA(L);
}

uint8_t cpu::CBinst26(){
return SLA_HLADDR();
}

uint8_t cpu::CBinst27(){
return SLA(A);
}

uint8_t cpu::CBinst28(){
return SRA(B);
}

uint8_t cpu::CBinst29(){
return SRA(C);
}

uint8_t cpu::CBinst2A(){
return SRA(D);
}

uint8_t cpu::CBinst2B(){
return SRA(E);
}

uint8_t cpu::CBinst2C(){
return SRA(H);
}

uint8_t cpu::CBinst2D(){
return SRA(L);
}

uint8_t cpu::CBinst2E(){
return SRA_HLADDR();
}

uint8_t cpu::CBinst2F(){
return SRA(A);
}

uint8_t cpu::CBinst30(){
return SWAP(B);
}

uint8_t cpu::CBinst31(){
return SWAP(C);
}

uint8_t cpu::CBinst32(){
return SWAP(D);
}

uint8_t cpu::CBinst33(){
return SWAP(E);
}

uint8_t cpu::CBinst34(){
return SWAP(H);
}

uint8_t cpu::CBinst35(){
return SWAP(L);
}

uint8_t cpu::CBinst36(){
return SWAP_HLADDR();
}

uint8_t cpu::CBinst37(){
return SWAP(A);
}

uint8_t cpu::CBinst38(){
return SRL(B);
}

uint8_t cpu::CBinst39(){
return SRL(C);
}

uint8_t cpu::CBinst3A(){
return SRL(D);
}

uint8_t cpu::CBinst3B(){
return SRL(E);
}

uint8_t cpu::CBinst3C(){
return SRL(H);
}

uint8_t cpu::CBinst3D(){
return SRL(L);
}

uint8_t cpu::CBinst3E(){
return SRL_HLADDR();
}

uint8_t cpu::CBinst3F(){
return SRL(A);
}

uint8_t cpu::CBinst40(){
return BIT(0,B);
}

uint8_t cpu::CBinst41(){
return BIT(0,C);
}

uint8_t cpu::CBinst42(){
return BIT(0,D);
}

uint8_t cpu::CBinst43(){
return BIT(0,E);
}

uint8_t cpu::CBinst44(){
return BIT(0,H);
}

uint8_t cpu::CBinst45(){
return BIT(0,L);
}

uint8_t cpu::CBinst46(){
return BIT_HLADDR(0);
}

uint8_t cpu::CBinst47(){
return BIT(0,A);
}

uint8_t cpu::CBinst48(){
return BIT(1,B);
}

uint8_t cpu::CBinst49(){
return BIT(1,C);
}

uint8_t cpu::CBinst4A(){
return BIT(1,D);
}

uint8_t cpu::CBinst4B(){
return BIT(1,E);
}

uint8_t cpu::CBinst4C(){
return BIT(1,H);
}

uint8_t cpu::CBinst4D(){
return BIT(1,L);
}

uint8_t cpu::CBinst4E(){
return BIT_HLADDR(1);
}

uint8_t cpu::CBinst4F(){
return BIT(1,A);
}

uint8_t cpu::CBinst50(){
return BIT(2,B);
}

uint8_t cpu::CBinst51(){
return BIT(2,C);
}

uint8_t cpu::CBinst52(){
return BIT(2,D);
}

uint8_t cpu::CBinst53(){
return BIT(2,E);
}

uint8_t cpu::CBinst54(){
return BIT(2,H);
}

uint8_t cpu::CBinst55(){
return BIT(2,L);
}

uint8_t cpu::CBinst56(){
return BIT_HLADDR(2);
}

uint8_t cpu::CBinst57(){
return BIT(2,A);
}

uint8_t cpu::CBinst58(){
return BIT(3,B);
}

uint8_t cpu::CBinst59(){
return BIT(3,C);
}

uint8_t cpu::CBinst5A(){
return BIT(3,D);
}

uint8_t cpu::CBinst5B(){
return BIT(3,E);
}

uint8_t cpu::CBinst5C(){
return BIT(3,H);
}

uint8_t cpu::CBinst5D(){
return BIT(3,L);
}

uint8_t cpu::CBinst5E(){
return BIT_HLADDR(3);
}

uint8_t cpu::CBinst5F(){
return BIT(3,A);
}

uint8_t cpu::CBinst60(){
return BIT(4,B);
}

uint8_t cpu::CBinst61(){
return BIT(4,C);
}

uint8_t cpu::CBinst62(){
return BIT(4,D);
}

uint8_t cpu::CBinst63(){
return BIT(4,E);
}

uint8_t cpu::CBinst64(){
return BIT(4,H);
}

uint8_t cpu::CBinst65(){
return BIT(4,L);
}

uint8_t cpu::CBinst66(){
return BIT_HLADDR(4);
}

uint8_t cpu::CBinst67(){
return BIT(4,A);
}

uint8_t cpu::CBinst68(){
return BIT(5,B);
}

uint8_t cpu::CBinst69(){
return BIT(5,C);
}

uint8_t cpu::CBinst6A(){
return BIT(5,D);
}

uint8_t cpu::CBinst6B(){
return BIT(5,E);
}

uint8_t cpu::CBinst6C(){
return BIT(5,H);
}

uint8_t cpu::CBinst6D(){
return BIT(5,L);
}

uint8_t cpu::CBinst6E(){
return BIT_HLADDR(5);
}

uint8_t cpu::CBinst6F(){
return BIT(5,A);
}

uint8_t cpu::CBinst70(){
return BIT(6,B);
}

uint8_t cpu::CBinst71(){
return BIT(6,C);
}

uint8_t cpu::CBinst72(){
return BIT(6,D);
}

uint8_t cpu::CBinst73(){
return BIT(6,E);
}

uint8_t cpu::CBinst74(){
return BIT(6,H);
}

uint8_t cpu::CBinst75(){
return BIT(6,L);
}

uint8_t cpu::CBinst76(){
return BIT_HLADDR(6);
}

uint8_t cpu::CBinst77(){
return BIT(6,A);
}

uint8_t cpu::CBinst78(){
return BIT(7,B);
}

uint8_t cpu::CBinst79(){
return BIT(7,C);
}

uint8_t cpu::CBinst7A(){
return BIT(7,D);
}

uint8_t cpu::CBinst7B(){
return BIT(7,E);
}

uint8_t cpu::CBinst7C(){
return BIT(7,H);
}

uint8_t cpu::CBinst7D(){
return BIT(7,L);
}

uint8_t cpu::CBinst7E(){
return BIT_HLADDR(7);
}

uint8_t cpu::CBinst7F(){
return BIT(7,A);
}

uint8_t cpu::CBinst80(){
return RES(0,B);
}

uint8_t cpu::CBinst81(){
return RES(0,C);
}

uint8_t cpu::CBinst82(){
return RES(0,D);
}

uint8_t cpu::CBinst83(){
return RES(0,E);
}

uint8_t cpu::CBinst84(){
return RES(0,H);
}

uint8_t cpu::CBinst85(){
return RES(0,L);
}

uint8_t cpu::CBinst86(){
return RES_HLADDR(0);
}

uint8_t cpu::CBinst87(){
return RES(0,A);
}

uint8_t cpu::CBinst88(){
return RES(1,B);
}

uint8_t cpu::CBinst89(){
return RES(1,C);
}

uint8_t cpu::CBinst8A(){
return RES(1,D);
}

uint8_t cpu::CBinst8B(){
return RES(1,E);
}

uint8_t cpu::CBinst8C(){
return RES(1,H);
}

uint8_t cpu::CBinst8D(){
return RES(1,L);
}

uint8_t cpu::CBinst8E(){
return RES_HLADDR(1);
}

uint8_t cpu::CBinst8F(){
return RES(1,A);
}

uint8_t cpu::CBinst90(){
return RES(2,B);
}

uint8_t cpu::CBinst91(){
return RES(2,C);
}

uint8_t cpu::CBinst92(){
return RES(2,D);
}

uint8_t cpu::CBinst93(){
return RES(2,E);
}

uint8_t cpu::CBinst94(){
return RES(2,H);
}

uint8_t cpu::CBinst95(){
return RES(2,L);
}

uint8_t cpu::CBinst96(){
return RES_HLADDR(2);
}

uint8_t cpu::CBinst97(){
return RES(2,A);
}

uint8_t cpu::CBinst98(){
return RES(3,B);
}

uint8_t cpu::CBinst99(){
return RES(3,C);
}

uint8_t cpu::CBinst9A(){
return RES(3,D);
}

uint8_t cpu::CBinst9B(){
return RES(3,E);
}

uint8_t cpu::CBinst9C(){
return RES(3,H);
}

uint8_t cpu::CBinst9D(){
return RES(3,L);
}

uint8_t cpu::CBinst9E(){
return RES_HLADDR(3);
}

uint8_t cpu::CBinst9F(){
return RES(3,A);
}

uint8_t cpu::CBinstA0(){
return RES(4,B);
}

uint8_t cpu::CBinstA1(){
return RES(4,C);
}

uint8_t cpu::CBinstA2(){
return RES(4,D);
}

uint8_t cpu::CBinstA3(){
return RES(4,E);
}

uint8_t cpu::CBinstA4(){
return RES(4,H);
}

uint8_t cpu::CBinstA5(){
return RES(4,L);
}

uint8_t cpu::CBinstA6(){
return RES_HLADDR(4);
}

uint8_t cpu::CBinstA7(){
return RES(4,A);
}

uint8_t cpu::CBinstA8(){
return RES(5,B);
}

uint8_t cpu::CBinstA9(){
return RES(5,C);
}

uint8_t cpu::CBinstAA(){
return RES(5,D);
}

uint8_t cpu::CBinstAB(){
return RES(5,E);
}

uint8_t cpu::CBinstAC(){
return RES(5,H);
}

uint8_t cpu::CBinstAD(){
return RES(5,L);
}

uint8_t cpu::CBinstAE(){
return RES_HLADDR(5);
}

uint8_t cpu::CBinstAF(){
return RES(5,A);
}

uint8_t cpu::CBinstB0(){
return RES(6,B);
}

uint8_t cpu::CBinstB1(){
return RES(6,C);
}

uint8_t cpu::CBinstB2(){
return RES(6,D);
}

uint8_t cpu::CBinstB3(){
return RES(6,E);
}

uint8_t cpu::CBinstB4(){
return RES(6,H);
}

uint8_t cpu::CBinstB5(){
return RES(6,L);
}

uint8_t cpu::CBinstB6(){
return RES_HLADDR(6);
}

uint8_t cpu::CBinstB7(){
return RES(6,A);
}

uint8_t cpu::CBinstB8(){
return RES(7,B);
}

uint8_t cpu::CBinstB9(){
return RES(7,C);
}

uint8_t cpu::CBinstBA(){
return RES(7,D);
}

uint8_t cpu::CBinstBB(){
return RES(7,E);
}

uint8_t cpu::CBinstBC(){
return RES(7,H);
}

uint8_t cpu::CBinstBD(){
return RES(7,L);
}

uint8_t cpu::CBinstBE(){
return RES_HLADDR(7);
}

uint8_t cpu::CBinstBF(){
return RES(7,A);
}

uint8_t cpu::CBinstC0(){
return SET(0,B);
}

uint8_t cpu::CBinstC1(){
return SET(0,C);
}

uint8_t cpu::CBinstC2(){
return SET(0,D);
}

uint8_t cpu::CBinstC3(){
return SET(0,E);
}

uint8_t cpu::CBinstC4(){
return SET(0,H);
}

uint8_t cpu::CBinstC5(){
return SET(0,L);
}

uint8_t cpu::CBinstC6(){
return SET_HLADDR(0);
}

uint8_t cpu::CBinstC7(){
return SET(0,A);
}

uint8_t cpu::CBinstC8(){
return SET(1,B);
}

uint8_t cpu::CBinstC9(){
return SET(1,C);
}

uint8_t cpu::CBinstCA(){
return SET(1,D);
}

uint8_t cpu::CBinstCB(){
return SET(1,E);
}

uint8_t cpu::CBinstCC(){
return SET(1,H);
}

uint8_t cpu::CBinstCD(){
return SET(1,L);
}

uint8_t cpu::CBinstCE(){
return SET_HLADDR(1);
}

uint8_t cpu::CBinstCF(){
return SET(1,A);
}

uint8_t cpu::CBinstD0(){
return SET(2,B);
}

uint8_t cpu::CBinstD1(){
return SET(2,C);
}

uint8_t cpu::CBinstD2(){
return SET(2,D);
}

uint8_t cpu::CBinstD3(){
return SET(2,E);
}

uint8_t cpu::CBinstD4(){
return SET(2,H);
}

uint8_t cpu::CBinstD5(){
return SET(2,L);
}

uint8_t cpu::CBinstD6(){
return SET_HLADDR(2);
}

uint8_t cpu::CBinstD7(){
return SET(2,A);
}

uint8_t cpu::CBinstD8(){
return SET(3,B);
}

uint8_t cpu::CBinstD9(){
return SET(3,C);
}

uint8_t cpu::CBinstDA(){
return SET(3,D);
}

uint8_t cpu::CBinstDB(){
return SET(3,E);
}

uint8_t cpu::CBinstDC(){
return SET(3,H);
}

uint8_t cpu::CBinstDD(){
return SET(3,L);
}

uint8_t cpu::CBinstDE(){
return SET_HLADDR(3);
}

uint8_t cpu::CBinstDF(){
return SET(3,A);
}

uint8_t cpu::CBinstE0(){
return SET(4,B);
}

uint8_t cpu::CBinstE1(){
return SET(4,C);
}

uint8_t cpu::CBinstE2(){
return SET(4,D);
}

uint8_t cpu::CBinstE3(){
return SET(4,E);
}

uint8_t cpu::CBinstE4(){
return SET(4,H);
}

uint8_t cpu::CBinstE5(){
return SET(4,L);
}

uint8_t cpu::CBinstE6(){
return SET_HLADDR(4);
}

uint8_t cpu::CBinstE7(){
return SET(4,A);
}

uint8_t cpu::CBinstE8(){
return SET(5,B);
}

uint8_t cpu::CBinstE9(){
return SET(5,C);
}

uint8_t cpu::CBinstEA(){
return SET(5,D);
}

uint8_t cpu::CBinstEB(){
return SET(5,E);
}

uint8_t cpu::CBinstEC(){
return SET(5,H);
}

uint8_t cpu::CBinstED(){
return SET(5,L);
}

uint8_t cpu::CBinstEE(){
return SET_HLADDR(5);
}

uint8_t cpu::CBinstEF(){
return SET(5,A);
}

uint8_t cpu::CBinstF0(){
return SET(6,B);
}

uint8_t cpu::CBinstF1(){
return SET(6,C);
}

uint8_t cpu::CBinstF2(){
return SET(6,D);
}

uint8_t cpu::CBinstF3(){
return SET(6,E);
}

uint8_t cpu::CBinstF4(){
return SET(6,H);
}

uint8_t cpu::CBinstF5(){
return SET(6,L);
}

uint8_t cpu::CBinstF6(){
return SET_HLADDR(6);
}

uint8_t cpu::CBinstF7(){
return SET(6,A);
}

uint8_t cpu::CBinstF8(){
return SET(7,B);
}

uint8_t cpu::CBinstF9(){
return SET(7,C);
}

uint8_t cpu::CBinstFA(){
return SET(7,D);
}

uint8_t cpu::CBinstFB(){
return SET(7,E);
}

uint8_t cpu::CBinstFC(){
return SET(7,H);
}

uint8_t cpu::CBinstFD(){
return SET(7,L);
}

uint8_t cpu::CBinstFE(){
return SET_HLADDR(7);
}

uint8_t cpu::CBinstFF(){
return SET(7,A);
}




