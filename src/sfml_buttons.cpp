#include "sfml_buttons.hh"






void sfml_buttons::read(uint8_t & button){

	if(sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
		button |= RIGHT;
	else
		button &= ~RIGHT;

	if(sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
		button |= LEFT;
	else
		button &= ~LEFT;

	if(sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
		button |= UP;
	else
		button &= ~UP;

	if(sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
		button |= DOWN;
	else
		button &= ~DOWN;

	if(sf::Keyboard::isKeyPressed(sf::Keyboard::Z))
		button |= A;
	else
		button &= ~A;

	if(sf::Keyboard::isKeyPressed(sf::Keyboard::X))
		button |= B;
	else
		button &= ~B;

	if(sf::Keyboard::isKeyPressed(sf::Keyboard::V))
		button |= SELECT;
	else
		button &= ~SELECT;

	if(sf::Keyboard::isKeyPressed(sf::Keyboard::Return))
		button |= START;
	else
		button &= ~START;

}
