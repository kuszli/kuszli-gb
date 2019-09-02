#include<iostream>

#include "defines.h"
#include "cpu.hh"
#include "timer.hh"
#include "lcd_driver.hh"
#include "interrupts.hh"
#include "memory.hh"
#include "joypad.hh"
#include "dma.hh"
#include "sfml_buttons.hh"
#include<unistd.h>
#include<cstdlib>
#include<sys/time.h>
#include<chrono>
#include<string>
#include <SFML/Graphics.hpp>


#define GB_CPU_SPEED 4194000
#define GB_SCREEN_REFRESH_RATE 59.73
#define INSTRUCTIONS_PER_VBLANK 70224
#define VBLANK_IN_MICROSECONDS 1000000/59.73

//using namespace std;



int main(int argc, char** argv){

if(argc < 2){
	std::cout << "Wrong number of parameters. Usage: gb \"rom_name.gb\"\n";
	return -1;
	}

_memory memory(argv[1]);

uint16_t break_point = atoi(argv[2]);
std::cout << break_point << std::endl;

uint8_t** pixels;
uint8_t* px = new  uint8_t[160*144*4];

uint8_t key = 0;

cpu* _cpu = new cpu(memory);
timer* _timer = new timer(memory);
lcd_driver*	_lcd_driver = new lcd_driver(memory);
interrupts*	_interrupts = new interrupts(memory);
dma* _dma = new dma(memory);
joypad* _joypad = new joypad(memory);



uint8_t opcode;
uint8_t cycles = 4;

int y = 0;

auto print_regs = [&](){std::cout << "PC: " << std::hex << (unsigned int)*(_cpu->regs16[1])<< "\nAF: " << (unsigned int)*(_cpu->regs[0]) << " " << (int)*(_cpu->regs[1]) << " BC: " << (unsigned int)*(_cpu->regs[2]) << " " << (int)*(_cpu->regs[3]) << "\nDE: " << (unsigned int)*(_cpu->regs[4]) << " " << (int)*(_cpu->regs[5]) << " HL: " << (unsigned int)*(_cpu->regs[6]) << " " << (int)*(_cpu->regs[7]) << std::endl << "LY: " << (int) _lcd_driver->debug() << " SP: " << (int)*_cpu->regs16[0] <<  std::endl;};


unsigned int speed = 1000;
char c;

sf::RenderWindow window(sf::VideoMode(160, 144), "kuszli-gb");


sf::Texture texture;
texture.create(160,144);


sf::Sprite sprite(texture);

sf::Color pal[4] = {sf::Color(155,188,15), sf::Color(48,98,48),  sf::Color(139,172,15), sf::Color(15,56,15)};

char mode = 1;
int spee = 0;

bool br_mode = false;

while(window.isOpen()){

auto start = std::chrono::steady_clock::now();

for(int i = 0; i < INSTRUCTIONS_PER_VBLANK; i+=cycles){
	cycles = _cpu->execute(_cpu->decode());
	_timer->update(cycles);
	_dma->update(cycles);
	_lcd_driver->update(cycles);
	_joypad->update(key);
	_cpu->handle_interrupts(_interrupts->check());
/*
if(*(_cpu->regs16[1]) == break_point || br_mode){
	print_regs();
	std::cin.get(c);
	break_point = 0;
	br_mode = true;
	//if(c == 'k')
		//spee += 1000;
	////else
		//spee=i;
}
*/
}
spee = 0;
pixels = _lcd_driver->screen();


for(int y = 0; y < 144; ++y){
	for(int x = 0; x < 160; ++x){
		px[(y*160 + x)*4] = pal[pixels[y][x]].r;
		px[(y*160 + x)*4+1] = pal[pixels[y][x]].g;
		px[(y*160 + x)*4+2] = pal[pixels[y][x]].b;
		px[(y*160 + x)*4+3] = 255;
	}
}


sf::Event event;
while (window.pollEvent(event)){
	if (event.type == sf::Event::Closed)
		window.close();
}



sfml_buttons::read(key);

texture.update(px);
window.draw(sprite);
window.display();

auto end = std::chrono::steady_clock::now();

if(16742 - std::chrono::duration_cast<std::chrono::microseconds>(end-start).count() < 0)
	std::cout << "ERR\n";
else
//std::cout << std::chrono::duration_cast<std::chrono::microseconds>(end-start).count() << std::endl;
usleep(16742 - std::chrono::duration_cast<std::chrono::microseconds>(end-start).count()); 

}


delete[] px;

}


