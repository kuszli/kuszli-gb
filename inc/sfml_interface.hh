#ifndef sfml_interfaceh
#define sfml_interfaceh

#include <SFML/Graphics.hpp>
#include <cstdint>
#include <vector>
#include "gameboy.hh"

enum options{

	LOAD_STATE = 1 << 0,
	SAVE_STATE = 1 << 1,
	TOGGLE_SPEED = 1 << 2,
	SCREEN_SIZE = 1 << 3,
	BG_MAP_WIN = 1 << 4,
	VRAM_WIN = 1 << 5,
	OAM_WIN = 1 << 6,
	DEBUG = 1 << 7,

};

class sfml_interface{

	gameboy* gb;
	const uint16_t max_screen_height = sf::VideoMode::getDesktopMode().height;

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
	std::vector<sf::Keyboard::Key> function_buttons;

	uint8_t key, func_key;
	bool oam_dbg;

	void check_key(uint8_t &key);
	void check_function_key();
	void prepare_buttons();
	void process_options(uint8_t opt);

public:

	sfml_interface(gameboy* g);
	//~sfml_interface();
	uint8_t get_key() const { return key; }
	void display(const uint8_t* pixels);
	void show_oam(const uint8_t* oam_pixels);
	void check_events();
	bool is_window_open() const { return window.isOpen(); }

};


#endif
