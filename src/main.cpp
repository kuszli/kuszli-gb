#include<iostream>
#include"cpu.hh"
#include<unistd.h>
#include<sys/time.h>
#include<chrono>
#include<fstream>


#define GB_CPU_SPEED 4194000
#define GB_SCREEN_REFRESH_RATE 59.73
#define INSTRUCTIONS_PER_VBLANK 17549
#define VBLANK_IN_MICROSECONDS 1000000/59.73

//using namespace std;


uint8_t katia(){
return 4;
}

int main(){

cpu* _cpu = new cpu();


uint8_t opcode;
uint8_t cycles = 0;

int y = 0;


while(1){

auto start = std::chrono::steady_clock::now();
for(int i = 0; i < INSTRUCTIONS_PER_VBLANK; i+=cycles){
	cycles = _cpu->execute(_cpu->decode());
}
auto end = std::chrono::steady_clock::now();

usleep(VBLANK_IN_MICROSECONDS - std::chrono::duration_cast<std::chrono::microseconds>(end-start).count()); 

std::cout << "VBLANK "  << ++y << "\n";
}



}
