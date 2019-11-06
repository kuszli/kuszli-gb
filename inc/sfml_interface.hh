#ifndef sfml_interfaceh
#define sfml_interfaceh

#include <SFML/Graphics.hpp>
#include <cstdint>
#include <vector>
#include "gameboy.hh"



class sfml_interface{

	sf::Vector2u screen_size = sf::Vector2u(480,432);
	sf::Vector2u oam_screen_size = sf::Vector2u(160,512);

	uint8_t pal[16] = {255, 255, 255, 255, 160, 160, 160, 255, 75, 75, 75, 255, 0, 0, 0, 255};
	sf::RenderWindow window;
	sf::RenderWindow debug;
	sf::Texture texture;
	sf::Texture oam_texture;
	sf::Texture palette;
	sf::Sprite sprite;
	sf::Sprite oam;
	sf::Event event;
	sf::Shader shader;

	std::vector<sf::Keyboard::Key> user_buttons;

	uint8_t* px; 
	uint8_t* oam_px;
	uint8_t key;
	bool oam_dbg;

	void check_key(uint8_t &key);

public:

	sfml_interface(bool dbg, _gb_type gbt);
	~sfml_interface();
	uint8_t get_key() const { return key; }
	void display(const uint8_t* pixels);
	void show_oam(const uint8_t* const* oam_pixels);
	void check_events();
	bool is_window_open() const { return window.isOpen(); }

};


#endif
