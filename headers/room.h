#ifndef ROOM_SRC
#define ROOM_SRC
#include <SFML/Graphics.hpp>
#include <iostream>

using namespace std;

class room {
	public:
		room();
		void setDim(int x, int y);
		void setOrigin(int x, int y);
		int getH();
		int getW();
		int getOriginX();
		int getOriginY();
		void setColor(int x);
		int getColor();
		int getCenterX();
		int getCenterY();
		sf::RectangleShape getBox();

	private:
		void setCenter();
		int width, height, depth;
		int originX, originY;
		int centerX, centerY;
		int color;
		sf::RectangleShape box;

};

room::room() {}

void room::setDim(int w, int h) {
	width = w;
	height = h;
	box.setSize(sf::Vector2f(w, h));
}

void room::setOrigin(int x, int y) {
	originX = x;
	originY = y;
	box.setPosition(x, y);
	//box.setOutlineThickness(1);
	//box.setOutlineColor(sf::Color(sf::Color::White));
	setCenter();
}

int room::getW() { return width; }

int room::getH() { return height; }

int room::getOriginX() { return originX; }

int room::getOriginY() { return originY; }

void room::setColor(int x)  { 
	color = x;
	if (color == 1) { box.setFillColor(sf::Color::Blue); }
	else if (color == 2) { box.setFillColor(sf::Color::Green); }
	else if (color == 3) { box.setFillColor(sf::Color::Red); }
	else {box.setFillColor(sf::Color::Red);}
}

int room::getColor() { return color; }

int room::getCenterX() { return centerX; }

int room::getCenterY() { return centerY; }

sf::RectangleShape room::getBox() {
	return box;
}

void room::setCenter(){
	centerX = originX + (width / 2);
	centerY = originY + (height / 2);
}

#endif
