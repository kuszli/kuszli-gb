
#include"cpu.hh"



cpu::cpu(){
memory = new uint8_t[65536];
memory[0] = 128;
memory[1] = 32;
memory[2] = 64;
PC = 0;
}

cpu::~cpu(){
delete[] memory;
}

//*********generic cpu::instructions*********************************************

uint8_t cpu::NOP(){ return 4; }

uint8_t cpu::LD_REG16_d16(uint8_t &reg_high, uint8_t &reg_low){
	reg_low = memory[++PC];
	reg_high = memory[++PC];
	return 12;
}

uint8_t cpu::LD_REG8_d8(uint8_t &reg){
	reg = memory[++PC];
	return 8;
}


uint8_t cpu::LD_REG16_A(const uint16_t reg){
	memory[reg] = A;
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
	if(reg_low == 0xFF);
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

//shift left by 1 byte and set new bit 0 to old bit 7
	A = (A << 1) | ((A & 1<<7)>>7);

	F &= ~ZERO_FLAG;
	F &= ~SUBTRACT_FLAG;
	F &= ~HALF_CARRY_FLAG;
	return 4;
}

uint8_t cpu::LD_a16_SP(){
	uint8_t low = memory[++PC];
	uint8_t high = memory[++PC];
	uint16_t addr = (high << 8) | low;
	memory[addr] = SP & 0xFF;
	memory[addr+1] = SP >> 8;
	return 20;
}

uint8_t cpu::ADD_HL_REG16(const uint16_t reg){
	uint32_t res = HL + reg;

	if(res & 1<<16)
		F |= CARRY_FLAG;
	else 
		F &= ~CARRY_FLAG;

	if( ((HL & 0xFFF) + (reg & 0xFFF)) & 1<<12)
		F |= HALF_CARRY_FLAG;
	else 
		F &= ~HALF_CARRY_FLAG;
	
	F &= ~SUBTRACT_FLAG;
	
	H = (res & 0xFF00) >> 8;
	L = (res & 0x00FF);
	return 8;
}




uint8_t cpu::LD_A_ADDR(const uint16_t reg_addr){
	A = memory[reg_addr];
	return 8;
}

uint8_t cpu::RRCA(){
	if(A & 1<<0)
		F |= CARRY_FLAG;
	else
		F &= ~CARRY_FLAG;

//shift right by 1 byte and set new bit 7 to old bit 0
	A = (A >> 1) | ((A & 1<<0)<<7);

	F &= ~ZERO_FLAG;
	F &= ~SUBTRACT_FLAG;
	F &= ~HALF_CARRY_FLAG;
	return 4;
}

uint8_t cpu::STOP(){
// to do
}

uint8_t cpu::LD_ADDR_A(const uint16_t reg_addr){
	memory[reg_addr] = A;
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
	PC+=memory[PC]+1;
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
	if(F & ZERO_FLAG)
		return 8;
	else{
		PC+=memory[PC]+1;
		return 12;
	}
}

uint8_t cpu::LD_HLINC_A(){
	memory[HL] = A;
	INC_REG16(H,L); //this might need some workaround
	return 8;
}

uint8_t cpu::LD_HLDEC_A(){
	memory[HL] = A;
	DEC_REG16(H,L); //this might need some workaround
	return 8;
}

uint8_t cpu::DAA(){
	uint8_t value_to_add = 0;
	
	if( (A & 0xF) > 0x9)
		value_to_add |= 0x6;
	else if(A > 0x99)
		value_to_add |= 0x60;

	return 4; 

}




uint8_t cpu::JR_Z_r8(){
	if((F & ZERO_FLAG)==0)
		return 8;
	else{
		PC+=memory[PC]+1;
		return 12;
	}
}

uint8_t cpu::LD_A_HLINC(){
	A = memory[HL];
	INC_REG16(H,L); //this might need some workaround
	return 8;
}

uint8_t cpu::LD_A_HLDEC(){
	A = memory[HL];
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
	if(F & CARRY_FLAG)
		return 8;
	else{
		PC+=memory[PC]+1;
		return 12;
	}
}

uint8_t cpu::LD_SP_d16(){
	uint8_t reg_low = memory[++PC];
	uint8_t reg_high = memory[++PC];
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
	if( (memory[HL] & 0xF) == 0xF)
		F |= HALF_CARRY_FLAG;
	else 
		F &= ~HALF_CARRY_FLAG;

	++memory[HL];

	if(memory[HL] == 0)
		F |= ZERO_FLAG;
	else 
		F &= ~ZERO_FLAG;

// clear subtract flag
	F &= ~SUBTRACT_FLAG;
	return 12;
}


uint8_t cpu::DEC_HLADDR(){
	if( (memory[HL] & 0xF) == 0)
		F |= HALF_CARRY_FLAG;
	else 
		F &= ~HALF_CARRY_FLAG;
	--memory[HL];

	if(memory[HL] == 0)
		F |= ZERO_FLAG;
	else 
		F &= ~ZERO_FLAG;
//set subtract flag
	F |= SUBTRACT_FLAG;
	return 12;
}



uint8_t cpu::LD_HLADDR_d8(){
	memory[HL] = memory[++PC];
	return 12;
}

uint8_t cpu::SCF(){
	F |= CARRY_FLAG;
	F &= ~HALF_CARRY_FLAG;
	F &= ~SUBTRACT_FLAG;
	return 4;
}

uint8_t cpu::JR_C_r8(){
	if((F & CARRY_FLAG)==0)
		return 8;
	else{
		PC+=memory[PC]+1;
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
	memory[HL] = reg;
	return 8;
}

uint8_t cpu::LD_REG8_HLADDR(uint8_t &reg){
	reg = memory[HL];
	return 8;
}

uint8_t cpu::HALT(){}

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
	uint16_t res = A+memory[HL];
	if( ((A & 0xF) + (memory[HL] & 0xF)) & 1 << 4)
		F |= HALF_CARRY_FLAG;
	else 
		F &= ~HALF_CARRY_FLAG;

	if(res & 1 << 8)
		F |= CARRY_FLAG;
	else 
		F &= ~CARRY_FLAG;

	A+=memory[HL];
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
		F &= CARRY_FLAG;

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

	uint16_t res = A+memory[HL]+flag;

	if( ((A & 0xF) + (memory[HL] & 0xF) + flag) & 1 << 4)
		F |= HALF_CARRY_FLAG;
	else
		F &= ~HALF_CARRY_FLAG;

	if(res & 1 << 8)
		F |= CARRY_FLAG;
	else
		F &= CARRY_FLAG;

	A+=memory[HL]+flag;

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
	if(memory[HL] > A)
		F |= CARRY_FLAG;
	else
		F &= ~CARRY_FLAG;

	if((memory[HL] & 0xF) > (A & 0xF))
		F |= HALF_CARRY_FLAG;
	else
		F &= ~HALF_CARRY_FLAG;	

	A-=memory[HL];

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

	if((reg + flag & 0xF) > (A & 0xF))
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

	if(memory[HL] + flag > A)
		F |= CARRY_FLAG;
	else
		F &= ~CARRY_FLAG;

	if((memory[HL] + flag & 0xF) > (A & 0xF))
		F |= HALF_CARRY_FLAG;
	else
		F &= ~HALF_CARRY_FLAG;	

	A-=(memory[HL]+flag);

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

	return 4;
}


uint8_t cpu::AND_HLADDR(){
	A &= memory[HL];
	if(A==0)
		F |= ZERO_FLAG;
	else
		F &= ~ZERO_FLAG;
	F |= HALF_CARRY_FLAG;

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
	A ^= memory[HL];
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
	A |= memory[HL];
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
	if(memory[HL] > A)
		F |= CARRY_FLAG;
	else
		F &= ~CARRY_FLAG;

	if((memory[HL] & 0xF) > (A & 0xF))
		F |= HALF_CARRY_FLAG;
	else
		F &= ~HALF_CARRY_FLAG;	


	if(A == memory[HL])
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
		uint8_t tmp = memory[SP++];
		PC = (memory[SP++]) << 8 | tmp;
		return 20;
	}
}


uint8_t cpu::RET_NC(){

	if(F & CARRY_FLAG)
		return 8;
	else{
		uint8_t tmp = memory[SP++];
		PC = (memory[SP++]) << 8 | tmp;
		return 20;
	}
}


uint8_t cpu::POP(uint8_t &reg_high, uint8_t &reg_low){
	reg_low = memory[SP++];
	reg_high = memory[SP++];
	return 12;
}

uint8_t cpu::JP_NZ(){
	uint8_t tmpl = memory[++PC];
	uint8_t tmph = memory[++PC];
	if(F & ZERO_FLAG)
		return 12;
	else{
		PC = (tmph << 8) | tmpl;
		return 16;
	}
}



uint8_t cpu::JP_NC(){
	uint8_t tmpl = memory[++PC];
	uint8_t tmph = memory[++PC];
	if(F & CARRY_FLAG)
		return 12;
	else{
		PC = (tmph << 8) | tmpl;
		return 16;
	}
}

uint8_t cpu::JP(){
	uint8_t tmpl = memory[++PC];
	uint8_t tmph = memory[++PC];
	PC = (tmph << 8) | tmpl;
	return 16;	
}

uint8_t cpu::CALL_NZ(){
	uint8_t tmpl = memory[++PC];
	uint8_t tmph = memory[++PC];
	if(F & ZERO_FLAG)
		return 12;
	else{
		uint16_t pctmp = PC+1;
		memory[--SP] = (pctmp >> 8);
		memory[--SP] = (pctmp & 0xFF);
		PC = (tmph << 8) | tmpl;
		return 24;
	}

}

uint8_t cpu::CALL_NC(){
	uint8_t tmpl = memory[++PC];
	uint8_t tmph = memory[++PC];
	if(F & CARRY_FLAG)
		return 12;
	else{
		uint16_t pctmp = PC+1;
		memory[--SP] = (pctmp >> 8);
		memory[--SP] = (pctmp & 0xFF);
		PC = (tmph << 8) | tmpl;
		return 24;
	}

}

uint8_t cpu::PUSH(const uint16_t reg){
	memory[--SP] = (reg >> 8);
	memory[--SP] = (reg & 0xFF);
	return 16;
}


uint8_t cpu::ADD_A_d8(){
	uint8_t tmp = memory[++PC];
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
	uint8_t tmp = memory[++PC];
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
	uint8_t tmp = memory[++PC];
	A &= tmp;
	if(A==0)
		F |= ZERO_FLAG;
	else
		F &= ~ZERO_FLAG;

	F |= HALF_CARRY_FLAG;

	return 8;
}

uint8_t cpu::OR_A_d8(){
	uint8_t tmp = memory[++PC];
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
		uint8_t tmp = memory[SP++];
		PC = (memory[SP++]) << 8 | tmp;
		return 20;
	}
}

uint8_t cpu::RET_C(){

	if((F & CARRY_FLAG)==0)
		return 8;
	else{
		uint8_t tmp = memory[SP++];
		PC = (memory[SP++]) << 8 | tmp;
		return 20;
	}
}

uint8_t cpu::RET(){
		uint8_t tmp = memory[SP++];
		PC = (memory[SP++]) << 8 | tmp;
		return 16;
}


uint8_t cpu::RETI(){
	uint8_t tmp = memory[SP++];
	PC = (memory[SP++]) << 8 | tmp;
	interrupts_enabled = true;
	return 16;
}

uint8_t cpu::JP_Z(){
	uint8_t tmpl = memory[++PC];
	uint8_t tmph = memory[++PC];
	if((F & ZERO_FLAG)==0)
		return 12;
	else{
		PC = (tmph << 8) | tmpl;
		return 16;
	}
}

uint8_t cpu::JP_C(){
	uint8_t tmpl = memory[++PC];
	uint8_t tmph = memory[++PC];
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

//TODO
uint8_t cpu::PREFIX_CB(){
return 8;
}

uint8_t cpu::CALL_Z(){
	uint8_t tmpl = memory[++PC];
	uint8_t tmph = memory[++PC];
	if((F & ZERO_FLAG)==0)
		return 12;
	else{
		uint16_t pctmp = PC+1;
		memory[--SP] = (pctmp >> 8);
		memory[--SP] = (pctmp & 0xFF);
		PC = (tmph << 8) | tmpl;
		return 24;
	}
}


uint8_t cpu::CALL_C(){
	uint8_t tmpl = memory[++PC];
	uint8_t tmph = memory[++PC];
	if((F & CARRY_FLAG)==0)
		return 12;
	else{
		uint16_t pctmp = PC+1;
		memory[--SP] = (pctmp >> 8);
		memory[--SP] = (pctmp & 0xFF);
		PC = (tmph << 8) | tmpl;
		return 24;
	}
}

uint8_t cpu::CALL(){
	uint8_t tmpl = memory[++PC];
	uint8_t tmph = memory[++PC];

	uint16_t pctmp = PC+1;
	memory[--SP] = (pctmp >> 8);
	memory[--SP] = (pctmp & 0xFF);
	PC = (tmph << 8) | tmpl;
	return 24;
}


uint8_t cpu::ADC_A_d8(){
	uint8_t flag = (F & CARRY_FLAG)? 1:0;
	uint8_t val = memory[++PC];
	uint16_t res = A+val+flag;

	if( ((A & 0xF) + (val & 0xF) + flag) & 1 << 4)
		F |= HALF_CARRY_FLAG;
	else
		F &= ~HALF_CARRY_FLAG;

	if(res & 1 << 8)
		F |= CARRY_FLAG;
	else
		F &= CARRY_FLAG;

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
	uint8_t val = memory[++PC];

	if(val + flag > A)
		F |= CARRY_FLAG;
	else
		F &= ~CARRY_FLAG;

	if((val + flag & 0xF) > (A & 0xF))
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
	uint8_t val = memory[++PC];
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
	uint8_t val = memory[++PC];
	A |= val;
	if(A == 0)
		F |= ZERO_FLAG;
	else
		F &= ~ZERO_FLAG;

	F &= ~CARRY_FLAG;
	F &= ~HALF_CARRY_FLAG;
	F &= ~SUBTRACT_FLAG;

	return 8;
}

uint8_t cpu::LDH_a8_A(){
	uint8_t val = memory[++PC];
	memory[0xFF00 + val] = A;
	return 12;
}

uint8_t cpu::LDH_A_a8(){
	uint8_t val = memory[++PC];
	A = memory[0xFF00 + val];
	return 12;
}


uint8_t cpu::LD_FFC_A(){
	memory[0xFF00 + C] = A;
	return 8;
}

uint8_t cpu::LD_A_FFC(){
	A = memory[0xFF00 + C];
	return 8;
}

uint8_t cpu::DI(){
	interrupts_enabled = false;
	return 4;
}

uint8_t cpu::ADD_SP_r8(){
	uint8_t val = memory[++PC];
	if(((SP & 0xFFF) + val) & 1<<12)
		F |= HALF_CARRY_FLAG;
	else
		F &= ~HALF_CARRY_FLAG;
	uint32_t res = SP+val;
	if(res & 1<<16)
		F |= CARRY_FLAG;
	else
		F &= ~CARRY_FLAG;
	
	SP+=val;

	F &= ~ZERO_FLAG;
	F &= ~SUBTRACT_FLAG;
	return 16;
}

uint8_t cpu::LD_HL_SPr8(){
	ADD_SP_r8();
	H = ((SP & 0xFF00)>>8);
	L = (SP & 0xFF);
	return 12;
}

uint8_t cpu::LD_SP_HL(){
	SP = HL;
	return 8;
}

uint8_t cpu::LD_ADDR_A(){
	uint8_t tmpl = memory[++PC];
	uint8_t tmph = memory[++PC];
	memory[(tmph << 8) | tmpl] = A;
	return 16;
}


uint8_t cpu::LD_A_ADDR(){
	uint8_t tmpl = memory[++PC];
	uint8_t tmph = memory[++PC];
	A = memory[(tmph << 8) | tmpl];
	return 16;
}

uint8_t cpu::EI(){
	interrupts_enabled = true;
	return 4;
}

uint8_t cpu::ERR(){
	return -1;
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

	uint8_t tmp = memory[HL];

	if(tmp  & 1 <<7)
		F |= CARRY_FLAG;
	else
		F &= ~CARRY_FLAG;

	memory[HL] = (tmp << 1);
	if(F & CARRY_FLAG)
		memory[HL] |= 1 << 0;

	if(memory[HL] == 0)
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

	uint8_t tmp = memory[HL];

	if(tmp  & 1 << 0)
		F |= CARRY_FLAG;
	else
		F &= ~CARRY_FLAG;

	memory[HL] = (tmp >> 1);
	if(F & CARRY_FLAG)
		memory[HL] |= 1 << 7;
	

	if(reg == 0)
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

	uint8_t tmp = memory[HL];
	uint8_t bit = (F & CARRY_FLAG)? 1:0;

	if(tmp  & 1 << 7)
		F |= CARRY_FLAG;
	else
		F &= ~CARRY_FLAG;

	memory[HL] = (tmp << 1);
	if(bit == 1)
		memory[HL] |= 1 << 0;

	if(memory[HL] == 0)
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

	uint8_t tmp = memory[HL];
	uint8_t bit = (F & CARRY_FLAG)? 1:0;

	if(tmp  & 1 << 0)
		F |= CARRY_FLAG;
	else
		F &= ~CARRY_FLAG;

	memory[HL] = (tmp >> 1);
	if(bit == 1)
		memory[HL] |= 1 << 7;

	if(memory[HL] == 0)
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

	uint8_t tmp = memory[HL];

	if(tmp  & 1 << 7)
		F |= CARRY_FLAG;
	else
		F &= ~CARRY_FLAG;

	memory[HL] = (tmp << 1);

	if(memory[HL] == 0)
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

	uint8_t bit = (reg & 1 << 7)? 1:0;
	uint8_t tmp = memory[HL];

	if(tmp  & 1 << 0)
		F |= CARRY_FLAG;
	else
		F &= ~CARRY_FLAG;

	memory[HL] = (tmp >> 1);
	if(bit == 1)
		memory[HL] |= (1 << 7);

	if(memory[HL] == 0)
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

	uint8_t tmpl = memory[HL] & 0xF;
	uint8_t tmph = memory[HL] & 0xF0;
	memory[HL] = (tmpl << 4) | (tmph >> 4);

	if(memory[HL] == 0)
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

	uint8_t tmp = memory[HL];

	if(tmp  & 1 << 0)
		F |= CARRY_FLAG;
	else
		F &= ~CARRY_FLAG;

	memory[HL] = (tmp >> 1);

	if(memory[HL] == 0)
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

	

uint8_t cpu::BIT_HLAADR(const uint8_t bit){

	if(memory[HL] & 1 << bit)
		F &= ~ZERO_FLAG;
	else
		F |= ZERO_FLAG;

	F &= ~SUBTRACT_FLAG;
	F |= HALF_CARRY_FLAG;

	return 16;
}

uint8_t cpu::RES(const uint8_t bit, uint8_t &reg){
	reg &= ~(1 << bit);
	return 8;
}

	
uint8_t cpu::RES_HLAADR(const uint8_t bit){
	memory[HL] &= ~(1 << bit);
	return 16;
}

uint8_t cpu::SET(const uint8_t bit, uint8_t &reg){
	reg |= (1 << bit);
	return 8;
}
	
uint8_t cpu::SET_HLAADR(const uint8_t bit){
	memory[HL] |= (1 << bit);
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
LD_REG8_d8(H);
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


		











