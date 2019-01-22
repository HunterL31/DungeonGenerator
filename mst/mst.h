#ifndef H_MST
#define H_MST

#include "../delaunay/vector2.h"
#include "../delaunay/edge.h"

#include <vector>
#include <iostream>
#include <stdlib.h>     // srand, rand 
#include <time.h>		//time function
#include <math.h>
#include "../delaunay/edge.h"

// Shortcut for an integer pair 
//typedef pair<int, int> iPair;

//using namespace std;

class Mst
{
    public:
        Mst();
        void addEdge(Edge<float> e);
        void sort();
        void checkVal();
    private:
        using VertexType = Vector2<float>;

        float getWeight(Edge<float> e);

        // vector to hold weight/edge pairs
        std::vector<std::pair<float, Edge<float> > > edges;
        std::vector<Edge<float> > mstEdges;
        std::vector<VertexType> points;
};

Mst::Mst() {}

void Mst::checkVal() { std::cout << edges.size() << " weight/edge pairs\n" << points.size() << " points" << std::endl; }

void Mst::addEdge(Edge<float> e)
{
    bool duplicate = false;

    // Checks if the edge already exists
    for(int i = 0; i < edges.size(); i++)
    {
        if(e == edges[i].second){
            duplicate = true;
            break;
        }
    }

    // If the edge is not a duplicate add it and calculate its weight
    if(!duplicate){
        edges.push_back(std::make_pair(getWeight(e), e));

        // Check if either point of the new edge is in the point vector, if it is not then add it
        bool p1Dupe = false, p2Dupe = false;
        for(int i = 0; i < points.size(); i++){
            if(points[i] == e.p1)
                p1Dupe = true;
            if(points[i] == e.p2)
                p2Dupe = true;
        }
        if(!p1Dupe)
            points.push_back(e.p1);
        if(!p2Dupe)
            points.push_back(e.p2);
    }
}

void Mst::sort()
{

}

float Mst::getWeight(Edge<float> e)
{
    // Returns the weight of the provided edge
    float weight = sqrt( pow((e.p2.x - e.p1.x), 2.0) + pow((e.p2.y - e.p1.y), 2.0));
    return weight;
}
#endif