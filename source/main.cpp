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
#define RADIUS 		15
#define FLOORS 		1
#define DOF			5
#define DUNGSCALE	4
#define DEBUG		0
#define SEED		0

int randRange(int low, int high) { return rand() % high + low; }

int getSeed()
{
	typedef std::chrono::high_resolution_clock myclock;
  	myclock::time_point beginning = myclock::now();

	myclock::duration d = myclock::now() - beginning;

	return d.count() * d.count();
}

int calcColor(float a)
{
	/*
	* Small rooms with an area below 50% of the max get colored blue
	* Medium rooms with an area between 50% - 80% get colored green 
	* Large rooms with an area greater than 80% get colored red
	*/
	if (a <= pow(5 * DUNGSCALE, 2)) 
		return 1; 
	else if (a <= pow(8 * DUNGSCALE, 2) && a > pow(5 * DUNGSCALE, 2))
		return 2;
	else if (a > pow(8 * DUNGSCALE, 2))
		return 3;
}
void generateRooms(room *rooms)
{
	//	Random width and height according to a chi-squared random distribution
	default_random_engine generator;
	chi_squared_distribution<double> distribution(DOF);	

	// Seed the generator
	if(SEED){
		generator.seed(SEED);
		cout << "Using preset dungeon seed " << SEED << endl;
	}else{
		int tempSeed = getSeed();
		generator.seed(tempSeed);
		cout << "Your random dungeon seed is " << tempSeed << endl;
	}

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

			// Get the room color
			rooms[i].setColor(calcColor((number * DUNGSCALE) * (number2 * DUNGSCALE)));
			
		}else{
			//	Edge case for when one dimension is 0 or below
			i--;
		}
	}
}

void overlap(room *rooms, int current)
{
	/*
	*	For every room
	*	Check if it intersects any other room
	*		If yes move it 1 unit in the pre defined direction(anywhere between 0 and 360 degrees) and check again
	*		If no exit
	*/
	float r = 1;
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
int countBig(room *rooms)
{
	int big = 0;
	for(int i = 0; i < ROOMNUM; i++){
			if(rooms[i].getColor() == 3)
				big++;
	}
	return big;
}

void getRoomCenter(room *rooms, vector<Vector2<float> > &points)
{
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

void drawRooms(sf::RenderWindow &window, room *rooms)
{
	for(int i = 0; i < ROOMNUM; i++)
		window.draw(rooms[i].getBox());
}

void drawHallways(sf::RenderWindow &window, vector<room> hallways)
{
	for(int i = 0; i < hallways.size(); i++)
		window.draw(hallways[i].getBox());
}

void initialDraw(sf::RenderWindow &window, room *rooms)
{
	generateRooms(rooms);
	
	window.clear();
	drawRooms(window, rooms);
	window.display();
}

void buttonQ(sf::RenderWindow &window)
{
	cout << "Quiting..." << endl;
	window.close();
}

void buttonG(sf::RenderWindow &window, room *rooms)
{
	cout << "Generating map..." << endl;

	for (int i = 0; i < ROOMNUM; i++){
		overlap(rooms, i);
		window.clear();
		drawRooms(window, rooms);
		window.display();
	}
	
	cout << "	map generated" << endl; 
}

void buttonG2(sf::RenderWindow &window, room *rooms)
{
	cout << "Regenerating map..." << endl;

	generateRooms(rooms);
						
	for (int i = 0; i < ROOMNUM; i++){
		overlap(rooms, i);
		window.clear();
		drawRooms(window, rooms);
		window.display();
	}

	cout << "	map generated" << endl;
}

void buttonD(sf::RenderWindow &window, room *rooms, std::vector<Edge<float> > &spanningEdges)
{
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
		for(int i = 0; i < edges.size(); i++)
			cout << edges[i] << endl;
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
}

void buttonM(sf::RenderWindow &window, room *rooms, std::vector<Edge<float> > &spanningEdges)
{
	cout << "Forming minimum spanning tree" << endl;

	Mst spanningTree;

	for(int i = 0; i < spanningEdges.size(); i++)
		spanningTree.addEdge(spanningEdges[i]);

	if(DEBUG)
		spanningTree.checkVal();

	spanningTree.sort();

	window.clear();

	drawRooms(window, rooms);

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

	spanningEdges.clear();
	spanningEdges = spanningTree.getMST();
	cout << "	Minimum spanning tree completed" << endl;
}

void buttonH(sf::RenderWindow &window, room *rooms, std::vector<Edge<float> > &spanningEdges, std::vector<room> &hallways)
{
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
	}
	drawHallways(window, hallways);
	std::cout << "	Hallways drawn" << std::endl;
	for(int i = 0; i < ROOMNUM; i++){
		for(int j = 0; j < hallways.size(); j++){
			if(rooms[i].getBox().getGlobalBounds().intersects(hallways[j].getBox().getGlobalBounds())){
				hallways.push_back(rooms[i]);
				break;
			}
		}
	}
	window.clear();
	drawHallways(window, hallways);
	window.display();	
}

void handleInput(bool *flags, sf::Event event, sf::RenderWindow &window, room *rooms, std::vector<Edge<float> > &spanningEdges, std::vector<room> &hallways)
{
	// Q tells program to quit in an elegant way
	if(event.key.code == sf::Keyboard::Q){
		buttonQ(window);

	// G generates and distributes the rooms 
	} else if(event.key.code == sf::Keyboard::G && !flags[0]){
		buttonG(window, rooms);
		flags[0] = true;

	// If the rooms have already been distributed, G will also reset the rest of the program
	} else if(event.key.code == sf::Keyboard::G && flags[0]){
		buttonG2(window, rooms);
		flags[1] = false;
		flags[2] = false;
		flags[3] = false;
		spanningEdges.clear();
		hallways.clear();

	// D will perform Delaunay triangulation on the rooms if they have been previously distributed
	} else if(event.key.code == sf::Keyboard::D && flags[0] && !flags[1]){
		buttonD(window, rooms, spanningEdges);
		flags[1] = true;
	
	// M forms a minimal spanning tree from the delaunay graph if delaunay triangulation has been ran on the current map
	}else if(event.key.code == sf::Keyboard::M && flags[0] && flags[1] && !flags[2]){
		buttonM(window, rooms, spanningEdges);
		flags[2] = true;
	
	}else if(event.key.code == sf::Keyboard::H && flags[0] && flags[1] && flags[2] && !flags[3]){
		buttonH(window, rooms, spanningEdges, hallways);
		flags[3] = true;

	// All other key presses are not recognized
	}else{
		cout << "Key press not recognized" << endl;
	}
}

int main()
{
	sf::RenderWindow window(sf::VideoMode(800, 800), "Dungeon Generator");
	window.setFramerateLimit(120);

	//	Event flags
	//generated, delaunay, minimal, hallways
	bool flags[] = {false, false, false, false};

	//	Initialize Rooms array
	room rooms[ROOMNUM];

	// Vector to hold hallways and final rooms
	std::vector<room> hallways;
	
	// Vector to hold the edges used to create the minimum spanning tree
	std::vector<Edge<float> > spanningEdges;

	//	Generate and draw initial cluster of rooms
	initialDraw(window, rooms);

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
					handleInput(flags, event, window, rooms, spanningEdges, hallways);
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