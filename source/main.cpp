#include <iostream>
#include <SFML/Graphics.hpp>
#include "../headers/room.h"
#include <stdlib.h>     // srand, rand 
#include <time.h>		//time function
#include <math.h>
#include <SFML/OpenGL.hpp>
#include <random>

//	Delaunay library includes 
#include "../delaunay/vector2.h"
#include "../delaunay/triangle.h"
#include "../delaunay/delaunay.h"

#define ROOMNUM 	300
#define RADIUS 		25
#define FLOORS 		2
#define QUICK 		0
#define DOF			4.5
#define DUNGSCALE	3
#define DEBUG		0

using namespace std;

int randRange(int low, int high) { return rand() % high + low; }

void generateRooms(room *rooms) {
	//	Random width and height according to a chi-squared random distribution
	
	default_random_engine generator;
	chi_squared_distribution<double> distribution(DOF);	

	for (int i = 0; i < ROOMNUM; i++) {
		double number = distribution(generator);
		double number2 = distribution(generator);

		if(number > 0 && number2 > 0 && number != number2){
			//	Set the dimensions of the room
			rooms[i].setDim(number * DUNGSCALE, number2 * DUNGSCALE);

			//	Set the origin of the room within the pre-defined radius
			float t = (float)(randRange(0, 10000)) / 1591.5494309;
			float a = ((float)(randRange(0, 10000)) / 10000.0)*(RADIUS);
			rooms[i].setOrigin(a*sin(t)+400, a*cos(t)+400);
			
		}else{
			//	Edge case for when one dimension is 0 or below
			i--;
		}
	}
	/*
	*	Small rooms with an area below 200 get colored blue
	*	Medium rooms with an area between 200 - 500 get colored green 
	*	Large rooms with an area greater than 500 get colored yellow
	*/
	//int small = 0, med = 0, large = 0;
	for (int i = 0; i < ROOMNUM; i++) {
		if (rooms[i].getW()*rooms[i].getH() <= 200) { 
			rooms[i].setColor(1); 
			//small++;
		} else if (rooms[i].getW()*rooms[i].getH() <= 500 && (rooms[i].getW()*rooms[i].getH() > 200)) { 
			rooms[i].setColor(2); 
			//med++;
		} else if ((rooms[i].getW()*rooms[i].getH() > 500)) { 
			rooms[i].setColor(3); 
			//large++;
		}
	}

	//	If the last room in room array is not large, make it large to avoid having gap and to ensure we have central room
	if(rooms[ROOMNUM - 1].getColor() != 3){
		room temproom;
		for(int i = 0; i < ROOMNUM; i++){
			if (rooms[i].getColor() == 3){
				temproom = rooms[ROOMNUM - 1];
				rooms[ROOMNUM - 1] = rooms[i];
				rooms[i] = temproom;
				break;
			}
		}
		//rooms[ROOMNUM - 1].setColor(4);
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
			r += 1.5;
			rooms[current].setOrigin(r*sin(t) + 400, r*cos(t) + 400);
		}
	}
}

void getRoomCenter(room *rooms, int big, int xy[][2]) {
	//	For every big room, grab the coordinates of its center
	int counter = 0;
	for(int i = 0; counter < big; i++){
		if(rooms[i].getColor() == 3 || rooms[i].getColor() == 4){
			xy[counter][0] = rooms[i].getCenterX();
			xy[counter][1] = rooms[i].getCenterY();
			counter++;
		}
	}
}

//	Counts the number of big rooms in an array of rooms
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
	bool generated = false, delaunay = false, minimal = false;

	//	Initialize Rooms array
	room rooms[ROOMNUM];

	//	Generate and draw initial cluster of rooms
	generateRooms(rooms);
	window.clear();
	for(int i = 0; i < ROOMNUM; i++)
		window.draw(rooms[i].getBox());
	window.display();

	while (window.isOpen()) {
		sf::Event event;
		while (window.pollEvent(event)){
			switch (event.type){

				case sf::Event::Closed:
					window.close();
					break;

				case sf::Event::KeyPressed:
					if(event.key.code == sf::Keyboard::Q){
						cout << "Quiting..." << endl;
						window.close();
					} else if(event.key.code == sf::Keyboard::G && !generated){
						generated = true;
						cout << "Generating map..." << endl;

						for (int i = 0; i < ROOMNUM; i++){
							overlap(rooms, i);
							if(!QUICK){
								window.clear();
								for (int j = 0; j <= i; j++) { 
									window.draw(rooms[j].getBox()); 
								}
								window.display();
							}else if(QUICK){
								for (int j = 0; j <= i; j++) { 
									window.draw(rooms[j].getBox()); 
								}
							}
						}
						window.display();
						cout << "	map generated" << endl; 
					} else if(event.key.code == sf::Keyboard::G && generated){
						cout << "Regenerating map..." << endl;
						delaunay = false;//	Should change to clear the delaunay triangulation if its been made
						minimal = false;//	Should change to clear the minimal spanning tree if its made
						generateRooms(rooms);
						window.clear();
						for (int i = 0; i < ROOMNUM; i++){
							overlap(rooms, i);
							if(!QUICK){
								window.clear();
								for (int j = 0; j <= i; j++) { 
									window.draw(rooms[j].getBox()); 
								}
								window.display();
							}else if(QUICK){
								for (int j = 0; j <= i; j++) { 
									window.draw(rooms[j].getBox()); 
								}
							}
						}
						window.display();
						cout << "	map generated" << endl;
					} else if(event.key.code == sf::Keyboard::D && generated && !delaunay){
						cout << "Forming Delaunay triangulation..." << endl;

						// Create array that will hold coordinates of the center of each big room
						int centers[countBig(rooms)][2];
						// Populate array with center coordinates for each room
						getRoomCenter(rooms, countBig(rooms), centers);
						cout << "	Using " << countBig(rooms) << " main rooms" << endl;

						// Transfer points to a vector of vector2s
						std::vector<Vector2<float> > points;
						for(int i = 0; i < countBig(rooms); ++i) {
							points.push_back(Vector2<float>(centers[i][0], centers[i][1]));
						}

						// Create Delaunay and triangle object and optionally print out debug message
						Delaunay<float> triangulation;
						const std::vector<Triangle<float> > triangles = triangulation.triangulate(points);
						std::cout << "	" << triangles.size() << " triangles generated\n" << std::endl;
						const std::vector<Edge<float> > edges = triangulation.getEdges();
						if(DEBUG){
							std::cout << "Points: " << points.size() << std::endl;
							for(const auto &p : points)
								std::cout << p << std::endl;
							std::cout << "Triangles: " << triangles.size() << std::endl;
							for(const auto &t : triangles)
								std::cout << t << std::endl;
							std::cout << "Edges: " << edges.size() << std::endl;
							for(const auto &e : edges)
								std::cout << e << std::endl;
						}
						
						//	Vector to hold edges
						std::vector<sf::RectangleShape*> edgeShapes;

						for(const auto p : points){
							sf::RectangleShape *c1 = new sf::RectangleShape(sf::Vector2f(4, 4));
							c1->setPosition(p.x, p.y);
							c1->setFillColor(sf::Color::Red);
							edgeShapes.push_back(c1);
						}

						std::vector<std::array<sf::Vertex, 2> > lines;
						for(const auto &e : edges) {
							lines.push_back({{
								sf::Vertex(sf::Vector2f(e.p1.x + 2, e.p1.y + 2)),
								sf::Vertex(sf::Vector2f(e.p2.x + 2, e.p2.y + 2))
							}});
						}
						
						for(const auto &l : lines) {
							window.draw(l.data(), 2, sf::Lines);
						}
						
						window.display();
						delaunay = true;
					
					}else if(event.key.code == sf::Keyboard::M && generated && delaunay && !minimal){
						cout << "Forming minimal spanning tree" << endl;
						minimal = true;
					}else{
						cout << "Key press not recognized" << endl;
					}

					break;

				case sf::Event::Resized:
					cout << "new width: " << event.size.width << endl;
					cout << "new height: " << event.size.height << endl;
					break;

				default:
					break;
			}
		}
	}
	return 0;
}