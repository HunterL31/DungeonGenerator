#ifndef ENTITY_SRC
#define ENTITY_SRC
#include <SFML/Graphics.hpp>
#include <iostream>

using namespace std;

class Entity {
	public:
		Entity();
		sf::Sprite getSprite();
		sf::Texture getTexture();
		void setTexture(sf::Texture t);

	private:
		sf::Sprite sprite;
		sf::Texture texture;


};

Entity::Entity() {}

sf::Sprite Entity::getSprite() { return sprite; }

sf::Texture Entity::getTexture() { return texture; }

void Entity::setTexture(sf::Texture t) {
	texture = t;
	sprite.setTexture(texture);
}

#endif