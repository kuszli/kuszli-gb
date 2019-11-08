#include "sfml_interface.hh"
#include <iostream>
#include <chrono>

#define VBLANK_IN_MICROSECONDS 16743

const char* fragment_dmg = R"glsl(
        #version 130

        uniform sampler2D pixels;

        uniform sampler2D palette;

        void main(){

            vec4 col = texture(pixels, gl_TexCoord[0].xy);
            gl_FragColor = texture(palette, vec2(85*col.x, 0));

        }
)glsl";


const char* fragment_cgb = R"glsl(
        #version 130

        uniform sampler2D pixels;

        void main(){

            vec4 col = texture(pixels, gl_TexCoord[0].xy);
            gl_FragColor = vec4(col.r * 8, col.g * 8, col.b * 8, 1.0);

        }
)glsl";


sfml_interface::sfml_interface(gameboy* g){

	window.create(sf::VideoMode(160, 144), "kuszli-gb");
	window.setSize(screen_size);
	texture.create(160,144);
	sprite.setTexture(texture);

	prepare_buttons();

	oam_dbg = false;

	palette.create(4,1);
   palette.update(pal);

	gb = g;
	emulation_speed = 1;

	if(gb->get_gb_type() == dmg){
		if(!shader.loadFromMemory(fragment_dmg, sf::Shader::Fragment)){
       	 std::cerr << "Shader loading error\n";
   	 }
		shader.setUniform("palette", palette);
    	shader.setUniform("pixels", sf::Shader::CurrentTexture);
	}
	else{
		if(!shader.loadFromMemory(fragment_cgb, sf::Shader::Fragment)){
       	 std::cerr << "Shader loading error\n";
   	 }
		shader.setUniform("pixels", sf::Shader::CurrentTexture);
	}
   
}




void sfml_interface::prepare_buttons(){

	user_buttons.push_back(sf::Keyboard::Right);
	user_buttons.push_back(sf::Keyboard::Left);
	user_buttons.push_back(sf::Keyboard::Up);
	user_buttons.push_back(sf::Keyboard::Down);
	user_buttons.push_back(sf::Keyboard::Z);
	user_buttons.push_back(sf::Keyboard::X);
	user_buttons.push_back(sf::Keyboard::BackSpace);
	user_buttons.push_back(sf::Keyboard::Return);

	function_buttons.push_back(sf::Keyboard::F1);
	function_buttons.push_back(sf::Keyboard::F2);
	function_buttons.push_back(sf::Keyboard::F3);
	function_buttons.push_back(sf::Keyboard::F4);
	function_buttons.push_back(sf::Keyboard::F5);
	function_buttons.push_back(sf::Keyboard::F6);
	function_buttons.push_back(sf::Keyboard::F7);
	function_buttons.push_back(sf::Keyboard::F8);

	key = func_key = 0;

}

void sfml_interface::event_loop(){


	while(window.isOpen()){
		auto start = std::chrono::steady_clock::now();

		gb->run();

		display(gb->get_display_data());

		if(oam_dbg)
			show_oam(gb->oam_debug());

		check_events();
		gb->set_buttons(get_key());

		auto end = std::chrono::steady_clock::now();

		time_elapsed = VBLANK_IN_MICROSECONDS/emulation_speed - std::chrono::duration_cast<std::chrono::microseconds>(end-start).count();
		if( time_elapsed > 0)
			sf::sleep(sf::microseconds(time_elapsed));

	}


}


void sfml_interface::display(const uint8_t* pixels){

	texture.update(pixels);
	window.draw(sprite, &shader);
	window.display();

}


void sfml_interface::show_oam(const uint8_t* oam_pixels){

	oam_texture.update(oam_pixels);
	debug.draw(oam, &shader);
	debug.display();

}

void sfml_interface::check_events(){

	while (window.pollEvent(event)){
		if (event.type == sf::Event::Closed)
			window.close();
		else if(event.type == sf::Event::KeyPressed || event.type == sf::Event::KeyReleased){
			check_key(key);
			check_function_key();
		}

	
	}

	if(oam_dbg){
		while (debug.pollEvent(event)){
			if (event.type == sf::Event::Closed){
				oam_dbg = false;
				debug.close();
			}
			else if(event.type == sf::Event::KeyPressed || event.type == sf::Event::KeyReleased){
				check_function_key();
			}

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



void sfml_interface::check_function_key(){


	for(uint8_t i = 0; i < 8; ++i){
		if(sf::Keyboard::isKeyPressed(function_buttons.at(i))){
			if(!(func_key & 1 << i)){
				func_key |= 1 << i;
				process_options(1 << i);
				break; //allow only one function key at a time
			}

		}
		else
			func_key &= ~(1 << i);

	}

}

void sfml_interface::process_options(uint8_t opt){

	//std::cout << "function: " << (int)opt << std::endl;
	switch(opt){

		case LOAD_STATE:{
			gb->load_state();
			break;
		}

		case SAVE_STATE:{
			gb->save_state();
			break;
		}

		case TOGGLE_SPEED:{
			emulation_speed *= 2;
			if(emulation_speed == 16)
				emulation_speed = 1;
			std::cout << "Emulation speed: " << (int)emulation_speed << "x\n";
			break;
		}

		case SCREEN_SIZE:{
			screen_size += sf::Vector2u(160,144);
			if(screen_size.y > max_screen_height){
				screen_size.x = 160;
				screen_size.y = 144;
			}
			window.setSize(screen_size);
			std::cout << "Screen size: " << screen_size.x << " x " << screen_size.y << "\n";
			break;
		}

		case BG_MAP_WIN:{
		break;
		}

		case VRAM_WIN:{
		break;
		}

		case OAM_WIN:{
			if(!oam_dbg){
				oam_dbg = true;
				debug.create(sf::VideoMode(40,128), "oam viewer");
				debug.setSize(oam_screen_size);
				if(oam_texture.getSize().x == 0){
					oam_texture.create(40,128);
					oam.setTexture(oam_texture);
				}
			}
			else{
				oam_dbg = false;
				debug.close();
			}
			break;
		}

		case DEBUG:{
		break;
		}
	
		default:
			break;

	}

}


