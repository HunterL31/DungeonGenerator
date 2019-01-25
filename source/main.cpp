#include <iostream>
#include <SFML/Graphics.hpp>
#include "../headers/room.h"
#include <stdlib.h>     // srand, rand 
#include <time.h>		//time function
#include <math.h>
#include <SFML/OpenGL.hpp>
#include <random>
#include <chrono>

//	Delaunay library includes 
#include "../delaunay/vector2.h"
#include "../delaunay/triangle.h"
#include "../delaunay/delaunay.h"

// MST library includes
#include "../mst/mst.h"

#define ROOMNUM 	150
#define RADIUS 		5
#define FLOORS 		1
#define QUICK 		0
#define DOF			4.5
#define DUNGSCALE	3.5
#define DEBUG		0

int randRange(int low, int high) { return rand() % high + low; }

void generateRooms(room *rooms) {
	typedef std::chrono::high_resolution_clock myclock;
  	myclock::time_point beginning = myclock::now();

	//	Random width and height according to a chi-squared random distribution
	default_random_engine generator;
	chi_squared_distribution<double> distribution(DOF);	

	// Seed the generator
	myclock::duration d = myclock::now() - beginning;
	generator.seed(d.count());
	cout << "Your dungeons seed is " << d.count() << endl;

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

//	Counts the number of big rooms in an array of rooms
int countBig(room *rooms){
	int big = 0;
	for(int i = 0; i < ROOMNUM; i++){
			if(rooms[i].getColor() == 3 || rooms[i].getColor() == 4)
				big++;
	}
	return big;
}

void getRoomCenter(room *rooms, vector<Vector2<float> > &points) {
	int roomCount = countBig(rooms);

	//	For every big room, grab the coordinates of its center
	int counter = 0;
	for(int i = 0; counter < roomCount; i++){
		if(rooms[i].getColor() == 3 || rooms[i].getColor() == 4){
			points.push_back(Vector2<float>(rooms[i].getCenterX(), rooms[i].getCenterY()));
			counter++;
		}
	}
}

int main() {
	sf::RenderWindow window(sf::VideoMode(800, 800), "Dungeon Generator");
	window.setFramerateLimit(120);

	//	Event flags
	bool generated = false, delaunay = false, minimal = false;

	//	Initialize Rooms array
	room rooms[ROOMNUM];

	std::vector<room> hallways;
	

	std::vector<Edge<float> > spanningEdges;

	//	Generate and draw initial cluster of rooms
	generateRooms(rooms);
	window.clear();
	for(int i = 0; i < ROOMNUM; i++)
		window.draw(rooms[i].getBox());
	window.display();

	// Window loop that runs while the window is open
	while (window.isOpen()) {
		sf::Event event;

		// Event loop that watches for key presses
		while (window.pollEvent(event)){
			switch (event.type){
				
				// Triggers when the window is closed
				case sf::Event::Closed:
					window.close();
					break;

				// Case for when a key is pressed
				case sf::Event::KeyPressed:

					// Q tells program to quit in an elegant way
					if(event.key.code == sf::Keyboard::Q){
						cout << "Quiting..." << endl;
						window.close();

					// G generates and distributes the rooms 
					} else if(event.key.code == sf::Keyboard::G && !generated){
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
						generated = true;

					// If the rooms have already been distributed, G will remake them and redistribute them
					} else if(event.key.code == sf::Keyboard::G && generated){
						cout << "Regenerating map..." << endl;
						delaunay = false;
						minimal = false;
						
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
						spanningEdges.clear();
						hallways.clear();
						cout << "	map generated" << endl;

					// D will perform Delaunay triangulation on the rooms if they have been previously distributed
					} else if(event.key.code == sf::Keyboard::D && generated && !delaunay){
						cout << "Forming Delaunay triangulation..." << endl;

						// Vector to hold coordinates of each large rooms center
						std::vector<Vector2<float> > points;

						// Populate point vector with center coordinates for each room
						getRoomCenter(rooms, points);
						cout << "	Using " << countBig(rooms) << " main rooms" << endl;

						// Create Delaunay object, triangulate the points and optionally print out debug message
						Delaunay<float> triangulation;
						const std::vector<Triangle<float> > triangles = triangulation.triangulate(points);
						
						// Get the values for each edge in the triangulation and store them in a vector
						const std::vector<Edge<float> > edges = triangulation.getEdges();

						// If debugging is turned on, print values of all points, triangles and edges
						if(DEBUG){
							// Number of triangles
							std::cout << "	" << triangles.size() << " triangles generated\n" << std::endl;

							// Number of points
							std::cout << "Points: " << points.size() << std::endl;
							for(const auto &p : points)
								std::cout << p << std::endl;

							// Number of triangles
							std::cout << "Triangles: " << triangles.size() << std::endl;
							for(const auto &t : triangles)
								std::cout << t << std::endl;

							// Number of Edges
							std::cout << "Edges: " << edges.size() << std::endl;
							for(int i = 0; i < edges.size(); i++){
								cout << edges[i] << endl;
							}
						}

						// Send all edges to Minimal Spanning Tree Object
						for(int i = 0; i < edges.size(); i++)
							spanningEdges.push_back(edges[i]);
						
						
						
						//	Create vector to hold edge shapes so that SFML can display them 
						std::vector<sf::RectangleShape*> edgeShapes;

						// Loops through points and creates rectangle shapes for each edge
						for(const auto p : points){
							sf::RectangleShape *c1 = new sf::RectangleShape(sf::Vector2f(4, 4));
							c1->setPosition(p.x, p.y);
							c1->setFillColor(sf::Color::Red);
							edgeShapes.push_back(c1);
						}

						// Add in the opposite corners to form rectangles
						std::vector<std::array<sf::Vertex, 2> > lines;
						for(const auto &e : edges) {
							lines.push_back({{
								sf::Vertex(sf::Vector2f(e.p1.x + 2, e.p1.y + 2)),
								sf::Vertex(sf::Vector2f(e.p2.x + 2, e.p2.y + 2))
							}});
						}
						
						// Draw each edge in the triangulation
						for(const auto &l : lines) {
							window.draw(l.data(), 2, sf::Lines);
						}
						
						// Display each edge and set delaunay flag to true
						window.display();
						delaunay = true;
					
					// M forms a minimal spanning tree from the delaunay graph if delaunay triangulation has been ran on the current map
					}else if(event.key.code == sf::Keyboard::M && generated && delaunay && !minimal){
						cout << "Forming minimal spanning tree" << endl;

						Mst spanningTree;

						for(int i = 0; i < spanningEdges.size(); i++)
							spanningTree.addEdge(spanningEdges[i]);

						if(DEBUG)
							spanningTree.checkVal();

						spanningTree.sort();

						window.clear();

						for(int i = 0; i < ROOMNUM; i++)
							window.draw(rooms[i].getBox());

						vector<Edge<float> > mstEdges = spanningTree.getMST();
						std::vector<sf::Vertex > lines;

						for(int i = 0; i < mstEdges.size(); i++){
							if(DEBUG)
								std::cout << mstEdges[i] << std::endl;
							lines.push_back(sf::Vertex(
								sf::Vertex(sf::Vector2f(mstEdges[i].p1.x + 2, mstEdges[i].p1.y + 2))
							));
							lines.push_back(sf::Vertex(
								sf::Vertex(sf::Vector2f(mstEdges[i].p2.x + 2, mstEdges[i].p2.y + 2))
							));
						}
							window.draw(&lines[0], lines.size(), sf::Lines);
						

						window.display();

						minimal = true;
						spanningEdges.clear();
						spanningEdges = spanningTree.getMST();
						std::cout << "  Minimal spanning tree completed" << std::endl;
					
					}else if(event.key.code == sf::Keyboard::H && generated && delaunay && minimal){
						std::cout << "Drawing hallways" << std::endl;

						for(int i = 0; i < spanningEdges.size(); i++){
							int x1 = spanningEdges[i].p1.x, y1 = spanningEdges[i].p1.y;
							int x2 = spanningEdges[i].p2.x, y2 = spanningEdges[i].p2.y;
							room horizontal, vertical;

							horizontal.setColor(1);
							vertical.setColor(1);

							if(DEBUG)
								std::cout << "	Hallway from (" << x1 << ", " << y1 << ") to (" << x2 << ", " << y2 << ")" << endl;

							if(x1 < x2){
								horizontal.setDim(x2 - x1, 3);
								horizontal.setOrigin(x1, y1 - 1);
								if(y1 < y2){
									vertical.setDim(3, y2 - y1);
									vertical.setOrigin(x2 - 1, y1);
								}else{
									vertical.setDim(3, y1 - y2);
									vertical.setOrigin(x2 - 1, y2);
								}
								
							}else{
								horizontal.setDim(x1 - x2, 3);
								horizontal.setOrigin(x2, y1 - 1);
								if(y1 < y2){
									vertical.setDim(3, y2 - y1);
									vertical.setOrigin(x2 - 1, y1);
								}else{
									vertical.setDim(3, y1 - y2);
									vertical.setOrigin(x2 - 1, y2);
								}
							}

							hallways.push_back(horizontal);
							hallways.push_back(vertical);

							window.draw(horizontal.getBox());
							window.draw(vertical.getBox());
							window.display();
						}
						std::cout << "	Hallways drawn" << std::endl;
						for(int i = 0; i < ROOMNUM; i++){
							for(int j = 0; j < hallways.size(); j++){
								if(rooms[i].getBox().getGlobalBounds().intersects(hallways[j].getBox().getGlobalBounds())){
									//rooms[i].setOrigin(0, 0);
									//rooms[i].setDim(0, 0);
									//std::cout << "Room " << i << " does not collide with a hallway" << std::endl;
									hallways.push_back(rooms[i]);
									break;
								}
							}
						}
						window.clear();
						//for(int i = 0; i < ROOMNUM; i++)
							//window.draw(rooms[i].getBox());
						for(int i = 0; i < hallways.size(); i++)
							window.draw(hallways[i].getBox());
						window.display();
						//rooms[current].getBox().getGlobalBounds().intersects(rooms[i].getBox().getGlobalBounds()) && current != i 
					// All other key presses are not recognized
					}else{
						cout << "Key press not recognized" << endl;
					}

					break;

				// Trigges when the window is resized and prints out message if debug mode is on
				case sf::Event::Resized:
					if(DEBUG){
						cout << "new width: " << event.size.width << endl;
						cout << "new height: " << event.size.height << endl;
					}
					
					break;

				default:
					break;
			}
		}
	}
	return 0;
}