#include "sfml_interface.hh"

sfml_interface::sfml_interface(bool dbg){
	window.create(sf::VideoMode(160, 144), "kuszli-gb");
	window.setSize(screen_size);
	texture.create(160,144);
	sprite.setTexture(texture);
	px = new  uint8_t[160*144*4];

	if(dbg){
		debug.create(sf::VideoMode(40,128), "oam viewer");
		debug.setSize(oam_screen_size);
		oam_texture.create(40,128);
		oam.setTexture(oam_texture);
		oam_px = new uint8_t[128*40*4];
	}
	
	key = 0;
	oam_dbg = dbg;
}


sfml_interface::~sfml_interface(){
	delete[] px;
	px = nullptr;
	if(oam_dbg){
		delete[] oam_px;
		oam_px = nullptr;
	}

}


void sfml_interface::display(uint8_t** pixels){

	for(int y = 0; y < 144; ++y){
		for(int x = 0; x < 160; ++x){
			px[(y*160 + x)*4] = pal[pixels[y][x]].r;
			px[(y*160 + x)*4+1] = pal[pixels[y][x]].g;
			px[(y*160 + x)*4+2] = pal[pixels[y][x]].b;
			px[(y*160 + x)*4+3] = 255;
		}	
	}

	texture.update(px);
	window.draw(sprite);
	window.display();

}


void sfml_interface::show_oam(uint8_t** oam_pixels){

	if(!oam_dbg)
		return;

	for(int y = 0; y < 128; ++y){
		for(int x = 0; x < 40; ++x){
			oam_px[(y*40 + x)*4] = pal[oam_pixels[y][x]].r;
			oam_px[(y*40 + x)*4+1] = pal[oam_pixels[y][x]].g;
			oam_px[(y*40 + x)*4+2] = pal[oam_pixels[y][x]].b;
			oam_px[(y*40 + x)*4+3] = 255;
		}
	}

	oam_texture.update(oam_px);
	debug.draw(oam);
	debug.display();

}

void sfml_interface::check_events(){

	while (window.pollEvent(event)){
		if (event.type == sf::Event::Closed)
			window.close();
		else if(event.type == sf::Event::KeyPressed || event.type == sf::Event::KeyReleased)
			sfml_buttons::read(key);
	
	}

	if(oam_dbg){
		while (debug.pollEvent(event)){
			if (event.type == sf::Event::Closed)
				debug.close();
		}
	}

}


