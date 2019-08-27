#ifndef sfmlbtns
#define sfmlbtns

#include <cstdint>
#include <SFML/Window.hpp>

#define RIGHT (1 << 0)
#define LEFT (1 << 1)
#define UP (1 << 2)
#define DOWN (1 << 3)
#define A (1 << 4)
#define B (1 << 5)
#define SELECT (1 << 6)
#define START (1 << 7)


class sfml_buttons{

public:
	static void read(uint8_t &button);



};

#endif
