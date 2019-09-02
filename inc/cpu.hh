#ifndef cpuh
#define cpuh


#include<iostream>
#include<cstdint>
#include "memory.hh"
#include "defines.h"


#define AF ((A << 8) | F)
#define BC ((B << 8) | C)
#define DE ((D << 8) | E)
#define HL ((H << 8) | L)

#define ZERO_FLAG (1<<7)
#define SUBTRACT_FLAG (1<<6)
#define HALF_CARRY_FLAG (1<<5)
#define CARRY_FLAG (1<<4)



enum state{
	running,
	halted,
	stopped,
};





class cpu{

private:

/* 8 bit registers 
Flags in F register in order: zero flag, subtract flag, half carry flag, 
carry flag (4 most significant bits)
*/

	uint8_t A,B,C,D,E,F,H,L;

// 16 bit registers
	uint16_t SP, PC;

	_memory memory;

	state _state;

	bool IME; //Interrupt Master Enable
	
	const uint8_t interrupt_address[5] = {0x40, 0x48, 0x50, 0x58, 0x60};


//generic instructions*******************************************************
	uint8_t PUSH2(uint8_t a, uint8_t b);

	uint8_t NOP();

	uint8_t LD_REG16_d16(uint8_t &reg_high, uint8_t &reg_low);

	uint8_t LD_REG8_d8(uint8_t &reg);

	uint8_t LD_REG16_A(const uint16_t reg);

	uint8_t INC_REG16(uint8_t &reg_high, uint8_t &reg_low);

	uint8_t INC_REG8(uint8_t &reg);

	uint8_t DEC_REG16(uint8_t &reg_high, uint8_t &reg_low);

	uint8_t DEC_REG8(uint8_t &reg);

	uint8_t RLCA();

	uint8_t LD_a16_SP();

	uint8_t ADD_HL_REG16(const uint16_t reg);

	uint8_t LD_A_ADDR(const uint16_t reg_addr);

	uint8_t RRCA();

	uint8_t STOP();

	uint8_t LD_ADDR_A(const uint16_t reg_addr);

	uint8_t RLA();

	uint8_t JR_r8();

	uint8_t RRA();

	uint8_t JR_NZ_r8();

	uint8_t LD_HLINC_A();
	
	uint8_t LD_HLDEC_A();

	uint8_t DAA();

	uint8_t JR_Z_r8();

	uint8_t LD_A_HLINC();

	uint8_t LD_A_HLDEC();

	uint8_t CPL();

	uint8_t JR_NC_r8();

	uint8_t LD_SP_d16();

	uint8_t INC_SP();

	uint8_t DEC_SP();

	uint8_t INC_HLADDR();

	uint8_t DEC_HLADDR();

	uint8_t LD_HLADDR_d8();

	uint8_t SCF();

	uint8_t JR_C_r8();

	uint8_t CCF();

	uint8_t LD_REG8_REG8(uint8_t &reg1, const uint8_t reg2);

	uint8_t LD_HLADDR_REG8(const uint8_t reg);

	uint8_t LD_REG8_HLADDR(uint8_t &reg);

	uint8_t HALT();

	uint8_t ADD_A_REG8(const uint8_t reg);

	uint8_t ADD_A_HLADDR();
	
	uint8_t ADC_A_REG8(const uint8_t reg);

	uint8_t ADC_A_HLADDR();

	uint8_t SUB_REG8(const uint8_t reg);

	uint8_t SUB_HLADDR();

	uint8_t SBC_REG8(const uint8_t reg);

	uint8_t SBC_HLADDR();

	uint8_t AND_REG8(const uint8_t reg);

	uint8_t AND_HLADDR();

	uint8_t XOR_REG8(const uint8_t reg);

	uint8_t XOR_HLADDR();

	uint8_t OR_REG8(const uint8_t reg);

	uint8_t OR_HLADDR();

	uint8_t CP_REG8(const uint8_t reg);

	uint8_t CP_HLADDR();

	uint8_t RET_NZ();

	uint8_t RET_NC();

	uint8_t POP(uint8_t &reg_high, uint8_t &reg_low);

	uint8_t JP_NZ();

	uint8_t JP_NC();

	uint8_t JP();

	uint8_t CALL_NZ();

	uint8_t CALL_NC();

	uint8_t PUSH(const uint16_t reg);

	uint8_t ADD_A_d8();

	uint8_t SUB_A_d8();

	uint8_t AND_A_d8();

	uint8_t OR_A_d8();

	uint8_t RST(uint8_t address);

	uint8_t RET_Z();

	uint8_t RET_C();

	uint8_t RET();

	uint8_t RETI();

	uint8_t JP_Z();

	uint8_t JP_C();

	uint8_t JP_HLADDR();

	uint8_t PREFIX_CB();

	uint8_t CALL_Z();

	uint8_t CALL_C();

	uint8_t CALL();

	uint8_t ADC_A_d8();

	uint8_t SBC_A_d8();

	uint8_t XOR_A_d8();

	uint8_t CP_A_d8();

	uint8_t LDH_a8_A();

	uint8_t LDH_A_a8();

	uint8_t LD_FFC_A();

	uint8_t LD_A_FFC();

	uint8_t DI();

	uint8_t ADD_SP_r8();

	uint8_t LD_HL_SPr8();

	uint8_t LD_SP_HL();

	uint8_t LD_ADDR_A();

	uint8_t LD_A_ADDR();

	uint8_t EI();

	uint8_t ERR();

//CB functions***************************************************************

	uint8_t RLC(uint8_t &reg);

	uint8_t RLC_HLADDR();

	uint8_t RRC(uint8_t &reg);

	uint8_t RRC_HLADDR();

	uint8_t RL(uint8_t &reg);

	uint8_t RL_HLADDR();

	uint8_t RR(uint8_t &reg);

	uint8_t RR_HLADDR();

	uint8_t SLA(uint8_t &reg);

	uint8_t SLA_HLADDR();

	uint8_t SRA(uint8_t &reg);

	uint8_t SRA_HLADDR();

	uint8_t SWAP(uint8_t &reg);

	uint8_t SWAP_HLADDR();

	uint8_t SRL(uint8_t &reg);

	uint8_t SRL_HLADDR();

	uint8_t BIT(const uint8_t bit, const uint8_t reg);

	uint8_t BIT_HLADDR(const uint8_t bit);

	uint8_t RES(const uint8_t bit, uint8_t &reg);
	
	uint8_t RES_HLADDR(const uint8_t bit);

	uint8_t SET(const uint8_t bit, uint8_t &reg);
	
	uint8_t SET_HLADDR(const uint8_t bit);



//***************************************************************************

//*********cpu intructions***************************************************

uint8_t inst00();
uint8_t inst01();
uint8_t inst02();
uint8_t inst03();
uint8_t inst04();
uint8_t inst05();
uint8_t inst06();
uint8_t inst07();
uint8_t inst08();
uint8_t inst09();
uint8_t inst0A();
uint8_t inst0B();
uint8_t inst0C();
uint8_t inst0D();
uint8_t inst0E();
uint8_t inst0F();
uint8_t inst10();
uint8_t inst11();
uint8_t inst12();
uint8_t inst13();
uint8_t inst14();
uint8_t inst15();
uint8_t inst16();
uint8_t inst17();
uint8_t inst18();
uint8_t inst19();
uint8_t inst1A();
uint8_t inst1B();
uint8_t inst1C();
uint8_t inst1D();
uint8_t inst1E();
uint8_t inst1F();
uint8_t inst20();
uint8_t inst21();
uint8_t inst22();
uint8_t inst23();
uint8_t inst24();
uint8_t inst25();
uint8_t inst26();
uint8_t inst27();
uint8_t inst28();
uint8_t inst29();
uint8_t inst2A();
uint8_t inst2B();
uint8_t inst2C();
uint8_t inst2D();
uint8_t inst2E();
uint8_t inst2F();
uint8_t inst30();
uint8_t inst31();
uint8_t inst32();
uint8_t inst33();
uint8_t inst34();
uint8_t inst35();
uint8_t inst36();
uint8_t inst37();
uint8_t inst38();
uint8_t inst39();
uint8_t inst3A();
uint8_t inst3B();
uint8_t inst3C();
uint8_t inst3D();
uint8_t inst3E();
uint8_t inst3F();
uint8_t inst40();
uint8_t inst41();
uint8_t inst42();
uint8_t inst43();
uint8_t inst44();
uint8_t inst45();
uint8_t inst46();
uint8_t inst47();
uint8_t inst48();
uint8_t inst49();
uint8_t inst4A();
uint8_t inst4B();
uint8_t inst4C();
uint8_t inst4D();
uint8_t inst4E();
uint8_t inst4F();
uint8_t inst50();
uint8_t inst51();
uint8_t inst52();
uint8_t inst53();
uint8_t inst54();
uint8_t inst55();
uint8_t inst56();
uint8_t inst57();
uint8_t inst58();
uint8_t inst59();
uint8_t inst5A();
uint8_t inst5B();
uint8_t inst5C();
uint8_t inst5D();
uint8_t inst5E();
uint8_t inst5F();
uint8_t inst60();
uint8_t inst61();
uint8_t inst62();
uint8_t inst63();
uint8_t inst64();
uint8_t inst65();
uint8_t inst66();
uint8_t inst67();
uint8_t inst68();
uint8_t inst69();
uint8_t inst6A();
uint8_t inst6B();
uint8_t inst6C();
uint8_t inst6D();
uint8_t inst6E();
uint8_t inst6F();
uint8_t inst70();
uint8_t inst71();
uint8_t inst72();
uint8_t inst73();
uint8_t inst74();
uint8_t inst75();
uint8_t inst76();
uint8_t inst77();
uint8_t inst78();
uint8_t inst79();
uint8_t inst7A();
uint8_t inst7B();
uint8_t inst7C();
uint8_t inst7D();
uint8_t inst7E();
uint8_t inst7F();
uint8_t inst80();
uint8_t inst81();
uint8_t inst82();
uint8_t inst83();
uint8_t inst84();
uint8_t inst85();
uint8_t inst86();
uint8_t inst87();
uint8_t inst88();
uint8_t inst89();
uint8_t inst8A();
uint8_t inst8B();
uint8_t inst8C();
uint8_t inst8D();
uint8_t inst8E();
uint8_t inst8F();
uint8_t inst90();
uint8_t inst91();
uint8_t inst92();
uint8_t inst93();
uint8_t inst94();
uint8_t inst95();
uint8_t inst96();
uint8_t inst97();
uint8_t inst98();
uint8_t inst99();
uint8_t inst9A();
uint8_t inst9B();
uint8_t inst9C();
uint8_t inst9D();
uint8_t inst9E();
uint8_t inst9F();
uint8_t instA0();
uint8_t instA1();
uint8_t instA2();
uint8_t instA3();
uint8_t instA4();
uint8_t instA5();
uint8_t instA6();
uint8_t instA7();
uint8_t instA8();
uint8_t instA9();
uint8_t instAA();
uint8_t instAB();
uint8_t instAC();
uint8_t instAD();
uint8_t instAE();
uint8_t instAF();
uint8_t instB0();
uint8_t instB1();
uint8_t instB2();
uint8_t instB3();
uint8_t instB4();
uint8_t instB5();
uint8_t instB6();
uint8_t instB7();
uint8_t instB8();
uint8_t instB9();
uint8_t instBA();
uint8_t instBB();
uint8_t instBC();
uint8_t instBD();
uint8_t instBE();
uint8_t instBF();
uint8_t instC0();
uint8_t instC1();
uint8_t instC2();
uint8_t instC3();
uint8_t instC4();
uint8_t instC5();
uint8_t instC6();
uint8_t instC7();
uint8_t instC8();
uint8_t instC9();
uint8_t instCA();
uint8_t instCB();
uint8_t instCC();
uint8_t instCD();
uint8_t instCE();
uint8_t instCF();
uint8_t instD0();
uint8_t instD1();
uint8_t instD2();
uint8_t instD3();
uint8_t instD4();
uint8_t instD5();
uint8_t instD6();
uint8_t instD7();
uint8_t instD8();
uint8_t instD9();
uint8_t instDA();
uint8_t instDB();
uint8_t instDC();
uint8_t instDD();
uint8_t instDE();
uint8_t instDF();
uint8_t instE0();
uint8_t instE1();
uint8_t instE2();
uint8_t instE3();
uint8_t instE4();
uint8_t instE5();
uint8_t instE6();
uint8_t instE7();
uint8_t instE8();
uint8_t instE9();
uint8_t instEA();
uint8_t instEB();
uint8_t instEC();
uint8_t instED();
uint8_t instEE();
uint8_t instEF();
uint8_t instF0();
uint8_t instF1();
uint8_t instF2();
uint8_t instF3();
uint8_t instF4();
uint8_t instF5();
uint8_t instF6();
uint8_t instF7();
uint8_t instF8();
uint8_t instF9();
uint8_t instFA();
uint8_t instFB();
uint8_t instFC();
uint8_t instFD();
uint8_t instFE();
uint8_t instFF();

//*******************************************************************************
//****************cb instructions************************************************

uint8_t CBinst00();
uint8_t CBinst01();
uint8_t CBinst02();
uint8_t CBinst03();
uint8_t CBinst04();
uint8_t CBinst05();
uint8_t CBinst06();
uint8_t CBinst07();
uint8_t CBinst08();
uint8_t CBinst09();
uint8_t CBinst0A();
uint8_t CBinst0B();
uint8_t CBinst0C();
uint8_t CBinst0D();
uint8_t CBinst0E();
uint8_t CBinst0F();
uint8_t CBinst10();
uint8_t CBinst11();
uint8_t CBinst12();
uint8_t CBinst13();
uint8_t CBinst14();
uint8_t CBinst15();
uint8_t CBinst16();
uint8_t CBinst17();
uint8_t CBinst18();
uint8_t CBinst19();
uint8_t CBinst1A();
uint8_t CBinst1B();
uint8_t CBinst1C();
uint8_t CBinst1D();
uint8_t CBinst1E();
uint8_t CBinst1F();
uint8_t CBinst20();
uint8_t CBinst21();
uint8_t CBinst22();
uint8_t CBinst23();
uint8_t CBinst24();
uint8_t CBinst25();
uint8_t CBinst26();
uint8_t CBinst27();
uint8_t CBinst28();
uint8_t CBinst29();
uint8_t CBinst2A();
uint8_t CBinst2B();
uint8_t CBinst2C();
uint8_t CBinst2D();
uint8_t CBinst2E();
uint8_t CBinst2F();
uint8_t CBinst30();
uint8_t CBinst31();
uint8_t CBinst32();
uint8_t CBinst33();
uint8_t CBinst34();
uint8_t CBinst35();
uint8_t CBinst36();
uint8_t CBinst37();
uint8_t CBinst38();
uint8_t CBinst39();
uint8_t CBinst3A();
uint8_t CBinst3B();
uint8_t CBinst3C();
uint8_t CBinst3D();
uint8_t CBinst3E();
uint8_t CBinst3F();
uint8_t CBinst40();
uint8_t CBinst41();
uint8_t CBinst42();
uint8_t CBinst43();
uint8_t CBinst44();
uint8_t CBinst45();
uint8_t CBinst46();
uint8_t CBinst47();
uint8_t CBinst48();
uint8_t CBinst49();
uint8_t CBinst4A();
uint8_t CBinst4B();
uint8_t CBinst4C();
uint8_t CBinst4D();
uint8_t CBinst4E();
uint8_t CBinst4F();
uint8_t CBinst50();
uint8_t CBinst51();
uint8_t CBinst52();
uint8_t CBinst53();
uint8_t CBinst54();
uint8_t CBinst55();
uint8_t CBinst56();
uint8_t CBinst57();
uint8_t CBinst58();
uint8_t CBinst59();
uint8_t CBinst5A();
uint8_t CBinst5B();
uint8_t CBinst5C();
uint8_t CBinst5D();
uint8_t CBinst5E();
uint8_t CBinst5F();
uint8_t CBinst60();
uint8_t CBinst61();
uint8_t CBinst62();
uint8_t CBinst63();
uint8_t CBinst64();
uint8_t CBinst65();
uint8_t CBinst66();
uint8_t CBinst67();
uint8_t CBinst68();
uint8_t CBinst69();
uint8_t CBinst6A();
uint8_t CBinst6B();
uint8_t CBinst6C();
uint8_t CBinst6D();
uint8_t CBinst6E();
uint8_t CBinst6F();
uint8_t CBinst70();
uint8_t CBinst71();
uint8_t CBinst72();
uint8_t CBinst73();
uint8_t CBinst74();
uint8_t CBinst75();
uint8_t CBinst76();
uint8_t CBinst77();
uint8_t CBinst78();
uint8_t CBinst79();
uint8_t CBinst7A();
uint8_t CBinst7B();
uint8_t CBinst7C();
uint8_t CBinst7D();
uint8_t CBinst7E();
uint8_t CBinst7F();
uint8_t CBinst80();
uint8_t CBinst81();
uint8_t CBinst82();
uint8_t CBinst83();
uint8_t CBinst84();
uint8_t CBinst85();
uint8_t CBinst86();
uint8_t CBinst87();
uint8_t CBinst88();
uint8_t CBinst89();
uint8_t CBinst8A();
uint8_t CBinst8B();
uint8_t CBinst8C();
uint8_t CBinst8D();
uint8_t CBinst8E();
uint8_t CBinst8F();
uint8_t CBinst90();
uint8_t CBinst91();
uint8_t CBinst92();
uint8_t CBinst93();
uint8_t CBinst94();
uint8_t CBinst95();
uint8_t CBinst96();
uint8_t CBinst97();
uint8_t CBinst98();
uint8_t CBinst99();
uint8_t CBinst9A();
uint8_t CBinst9B();
uint8_t CBinst9C();
uint8_t CBinst9D();
uint8_t CBinst9E();
uint8_t CBinst9F();
uint8_t CBinstA0();
uint8_t CBinstA1();
uint8_t CBinstA2();
uint8_t CBinstA3();
uint8_t CBinstA4();
uint8_t CBinstA5();
uint8_t CBinstA6();
uint8_t CBinstA7();
uint8_t CBinstA8();
uint8_t CBinstA9();
uint8_t CBinstAA();
uint8_t CBinstAB();
uint8_t CBinstAC();
uint8_t CBinstAD();
uint8_t CBinstAE();
uint8_t CBinstAF();
uint8_t CBinstB0();
uint8_t CBinstB1();
uint8_t CBinstB2();
uint8_t CBinstB3();
uint8_t CBinstB4();
uint8_t CBinstB5();
uint8_t CBinstB6();
uint8_t CBinstB7();
uint8_t CBinstB8();
uint8_t CBinstB9();
uint8_t CBinstBA();
uint8_t CBinstBB();
uint8_t CBinstBC();
uint8_t CBinstBD();
uint8_t CBinstBE();
uint8_t CBinstBF();
uint8_t CBinstC0();
uint8_t CBinstC1();
uint8_t CBinstC2();
uint8_t CBinstC3();
uint8_t CBinstC4();
uint8_t CBinstC5();
uint8_t CBinstC6();
uint8_t CBinstC7();
uint8_t CBinstC8();
uint8_t CBinstC9();
uint8_t CBinstCA();
uint8_t CBinstCB();
uint8_t CBinstCC();
uint8_t CBinstCD();
uint8_t CBinstCE();
uint8_t CBinstCF();
uint8_t CBinstD0();
uint8_t CBinstD1();
uint8_t CBinstD2();
uint8_t CBinstD3();
uint8_t CBinstD4();
uint8_t CBinstD5();
uint8_t CBinstD6();
uint8_t CBinstD7();
uint8_t CBinstD8();
uint8_t CBinstD9();
uint8_t CBinstDA();
uint8_t CBinstDB();
uint8_t CBinstDC();
uint8_t CBinstDD();
uint8_t CBinstDE();
uint8_t CBinstDF();
uint8_t CBinstE0();
uint8_t CBinstE1();
uint8_t CBinstE2();
uint8_t CBinstE3();
uint8_t CBinstE4();
uint8_t CBinstE5();
uint8_t CBinstE6();
uint8_t CBinstE7();
uint8_t CBinstE8();
uint8_t CBinstE9();
uint8_t CBinstEA();
uint8_t CBinstEB();
uint8_t CBinstEC();
uint8_t CBinstED();
uint8_t CBinstEE();
uint8_t CBinstEF();
uint8_t CBinstF0();
uint8_t CBinstF1();
uint8_t CBinstF2();
uint8_t CBinstF3();
uint8_t CBinstF4();
uint8_t CBinstF5();
uint8_t CBinstF6();
uint8_t CBinstF7();
uint8_t CBinstF8();
uint8_t CBinstF9();
uint8_t CBinstFA();
uint8_t CBinstFB();
uint8_t CBinstFC();
uint8_t CBinstFD();
uint8_t CBinstFE();
uint8_t CBinstFF();


uint8_t (cpu::**opcode_array)();	
uint8_t (cpu::**CB_array)();	

//******************************************************************************
	
public:

	cpu(_memory& mem);
	~cpu();
	uint8_t decode(); //{return memory[PC++];}
	uint8_t execute(const uint8_t opcode){return (this->*opcode_array[opcode])();} 
	void handle_interrupts(uint8_t addr);
	uint8_t** regs;
	uint16_t** regs16;

};




#endif

