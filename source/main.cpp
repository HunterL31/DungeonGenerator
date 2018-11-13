#include <iostream>
#include <SFML/Graphics.hpp>
#include "../headers/room.h"
#include <stdlib.h>     // srand, rand 
#include <time.h>		//time function
#include <math.h>
#include <SFML/OpenGL.hpp>

#define ROOMNUM 150
#define RADIUS 300
#define FLOORS 2

using namespace std;

void input() {

}

void logic() {

}

int randRange(int low, int high) { return rand() % high + low; }

void generateRooms(room *rooms, int size) {
	srand(time(NULL));
	int range;
	/* 
	*	70% chance of small room
	*		width and height between 5 - 7
	*	20% chance of medium room
	*		width and height between 10 - 15
	*	10% chance of large room
	*		width and height between 17 - 25
	*/
	for (int i = 0; i < size; i++) {
		range = randRange(1, 100);

		if (range >= 1 && range < 70) {
			rooms[i].setDim(randRange(1, 3) + 4, randRange(1, 3) + 4);
			//rooms[i].setColor(1);
		} else if (range >= 70 && range < 90) {
			rooms[i].setDim(randRange(1, 6) + 9, randRange(1, 6) + 9);
			//rooms[i].setColor(2);
		} else if (range >= 90 && range <= 100) {
			rooms[i].setDim(randRange(1, 9) + 16, randRange(1, 9) + 16);
			//rooms[i].setColor(3);
		}

		float t = (float)(randRange(0, 10000)) / 1591.5494309;
		float a = ((float)(randRange(0, 10000)) / (10000.0/400.0))*(RADIUS/400);

		rooms[i].setOrigin(a*sin(t)+400, a*cos(t)+400);
	}

	/*
	*	Small rooms with an area between 25 - 49 get colored blue
	*	Medium rooms with an area between 100 - 225 get colored green 
	*	Large rooms with an area between 289 - 625 get colored yellow
	*/
	int small = 0, med = 0, large = 0;
	for (int i = 0; i < size; i++) {
		if (rooms[i].getW()*rooms[i].getH() <= 49) { 
			rooms[i].setColor(1); 
			small++;
		} else if (rooms[i].getW()*rooms[i].getH() <= 225 && (rooms[i].getW()*rooms[i].getH() >= 100)) { 
			rooms[i].setColor(2); 
			med++;
		} else if ((rooms[i].getW()*rooms[i].getH() >= 289)) { 
			rooms[i].setColor(3); 
			large++;
		}
	}
	cout << "\nGenerated " << small << " small rooms, " << med << " medium rooms, " << large << " large rooms" << endl;

	//	If the last room in room array is not large, make it large to avoid having gap and to ensure we have central room
	if(rooms[ROOMNUM - 1].getColor() != 3){
		room temproom;
		for(int i = 0; i < ROOMNUM; i++){
			if (rooms[i].getColor() == 3){
				temproom = rooms[ROOMNUM - 1];
				rooms[ROOMNUM - 1] = rooms[i];
				rooms[i] = temproom;
			}
		}
		rooms[ROOMNUM - 1].setColor(4);
	}
}

void overlap(room *rooms, int current) {
	/*
	*	For every room
	*	Check if it intersects any other room
	*		If yes move it .1 units in the pre defined direction(anywhere between 0 and 360 degrees) and check again
	*		If no exit
	*/

	float r = .1;
	float t = (float)(rand() % 10000) / 1591.5494309;

	for (int i = 0; i < ROOMNUM; i++) {
		if (rooms[current].getBox().getGlobalBounds().intersects(rooms[i].getBox().getGlobalBounds()) && current != i ) {
			i = -1;
			r += .1;
			rooms[current].setOrigin(r*sin(t) + 400, r*cos(t) + 400);
		}
	}
}

void getRoomCenter(room *rooms, int big, int xy[][2]) {

	//	For every big room, grab the coordinates of its center
	int /*xy[big][2],*/ counter = 0;
	for(int i = 0; counter < big; i++){
		if(rooms[i].getColor() == 3 || rooms[i].getColor() == 4){
			xy[counter][0] = rooms[i].getCenterX();
			xy[counter][1] = rooms[i].getCenterY();
			counter++;
		}
	}
}

//	Counts the number of big rooms in the room array
int countBig(room *rooms){
	int big = 0;

	for(int i = 0; i < ROOMNUM; i++){
			if(rooms[i].getColor() == 3 || rooms[i].getColor() == 4)
				big++;
	}

	return big;
}

int main() {
	sf::RenderWindow window(sf::VideoMode(800, 800), "Dungeon Generator");
	window.setFramerateLimit(120);

	//	Event flags
	bool generated = false, pathFound = false;

	//	Initialize Rooms array
	room rooms[ROOMNUM];

	//	Center array
	int xy;

	//	Randomly generate each rooms dimensions
	generateRooms(rooms, ROOMNUM);

	// Create array that will hold coordinates of the center of each big room
	int centers[countBig(rooms)][2];

	while (window.isOpen()) {

		//	Move rooms to new locations and re draw the scene
		if(!generated){
			for (int i = 0; i < ROOMNUM; i++) {
				overlap(rooms, i);
				window.clear();
				for (int j = 0; j <= i; j++) { 
					window.draw(rooms[j].getBox()); 
				}
				window.display();
			}
			generated = true;
		}

		//	Run rooms through algorithm to calculate shortest path and trim the excess rooms
		if(!pathFound){
			getRoomCenter(rooms, countBig(rooms), centers);

			pathFound = true;
		}
		//sf::sleep(sf::milliseconds(1000));
	}
	return 0;
}