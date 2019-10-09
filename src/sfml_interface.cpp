#include "sfml_interface.hh"
#include <iostream>

const char* fragment = R"glsl(
        #version 130

        uniform sampler2D pixels;

        uniform sampler2D palette;

        void main(){

            vec4 col = texture(pixels, gl_TexCoord[0].xy);
            gl_FragColor = texture(palette, vec2(85*col.x, 0));

        }
)glsl";


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

	user_buttons.push_back(sf::Keyboard::Right);
	user_buttons.push_back(sf::Keyboard::Left);
	user_buttons.push_back(sf::Keyboard::Up);
	user_buttons.push_back(sf::Keyboard::Down);
	user_buttons.push_back(sf::Keyboard::Z);
	user_buttons.push_back(sf::Keyboard::X);
	user_buttons.push_back(sf::Keyboard::BackSpace);
	user_buttons.push_back(sf::Keyboard::Return);

	key = 0;
	oam_dbg = dbg;
	//window.setVerticalSyncEnabled(true);

	palette.create(4,1);
   palette.update(pal);

	if(!shader.loadFromMemory(fragment, sf::Shader::Fragment)){
        std::cerr << "Shader loading error\n";
    }

    shader.setUniform("pixels", sf::Shader::CurrentTexture);
    shader.setUniform("palette", palette);
}


sfml_interface::~sfml_interface(){
	delete[] px;
	px = nullptr;
	if(oam_dbg){
		delete[] oam_px;
		oam_px = nullptr;
	}

}


void sfml_interface::display(uint8_t* pixels){

	texture.update(pixels);
	window.draw(sprite, &shader);
	window.display();

}


void sfml_interface::show_oam(uint8_t** oam_pixels){

	if(!oam_dbg)
		return;
/*
	for(int y = 0; y < 128; ++y){
		for(int x = 0; x < 40; ++x){
			oam_px[(y*40 + x)*4] = pal[oam_pixels[y][x]].r;
			oam_px[(y*40 + x)*4+1] = pal[oam_pixels[y][x]].g;
			oam_px[(y*40 + x)*4+2] = pal[oam_pixels[y][x]].b;
			oam_px[(y*40 + x)*4+3] = 255;
		}
	}
*/
	oam_texture.update(oam_px);
	debug.draw(oam);
	debug.display();

}

void sfml_interface::check_events(){

	while (window.pollEvent(event)){
		if (event.type == sf::Event::Closed)
			window.close();
		else if(event.type == sf::Event::KeyPressed || event.type == sf::Event::KeyReleased)
			check_key(key);

	
	}

	if(oam_dbg){
		while (debug.pollEvent(event)){
			if (event.type == sf::Event::Closed)
				debug.close();
		}
	}

}


void sfml_interface::check_key(uint8_t &key){

	for(uint8_t i = 0; i < 8; ++i){
		if(sf::Keyboard::isKeyPressed(user_buttons.at(i)))
			key |= 1 << i;
		else
			key &= ~(1 << i);

	}

}









