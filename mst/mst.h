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
        vector<Edge<float> > getMST();
    private:
        using VertexType = Vector2<float>;

        float getWeight(Edge<float> e);

        bool checkPoint(VertexType p);
        bool checkEdge(Edge<float> e);

        // Vector to hold points
        std::vector<std::pair<VertexType, int> >points;

        // Vector to hold weight/edge pairs
        std::vector<std::pair<float, Edge<float> > > edges;

        // Vector to hold points that have been added to the MST
        std::vector<VertexType> mstPoints;

        // Vector to hold edges that have been added to the MST
        std::vector<Edge<float> > mstEdges;
};

Mst::Mst() {}

void Mst::checkVal() { std::cout << "\nMST: " << edges.size() << " weight/edge pairs\nMST:" << points.size() << " points" << std::endl; }

vector<Edge<float> > Mst::getMST() { return mstEdges; }

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
            if(points[i].first == e.p1)
                p1Dupe = true;
            if(points[i].first == e.p2)
                p2Dupe = true;
        }
        if(!p1Dupe)
            points.push_back(std::make_pair(e.p1, __INT_MAX__));
        if(!p2Dupe)
            points.push_back(std::make_pair(e.p2, __INT_MAX__));
    }
}

bool Mst::checkPoint(VertexType p)
{
    for(int i = 0; i < mstPoints.size(); i++)
        if(mstPoints[i] == p)
            return true;

    return false;
}

bool Mst::checkEdge(Edge<float> e)
{
    for(int i = 0; i < mstEdges.size(); i++)
        if(mstEdges[i] == e)
            return true;
    
    return  false;
}

/*  Algorithm Pseudo-Code
* 1) Create point vector mstPoints to keep track of all vertices already added to the MST
* 2) Assign a key value of INT_MAX to all vertices in vector points and then set the key value of the first point to 0 to assign the start point
* 3) While mstPoints is not equal in size to points
*  1) Pick vertex u which is not in mstPoints and has the smallest key value
*  2) Include u in mstPoints
*  3) Update key value of all adjacent vertices of u using the following process:
*   1) For every adjacent vertext v
*   2) If weight of edge u-v is less than the previous key value of point v, update the key value as the weight of edge u-v
*  4) Include edge u-v in the mstEdge vector
*/

void Mst::sort()
{
    // Push the first point into the MST set and set its key value to 0
    points[0].second = 0;
    mstPoints.push_back(points[0].first);

    // Holds the current point 
    VertexType curPoint = points[0].first;

    //Holds all connecting edges and their weights
    std::vector<std::pair<float, Edge<float> > > tempEdges;

    while(mstPoints.size() != points.size()){

        //Get any edge connecting to current version of MST
        for(int i = 0; i < edges.size(); i++){
            if(checkPoint(edges[i].second.p1) ^ checkPoint(edges[i].second.p2)){
                //std::cout << "Edge " << i << " connects to current point at " << curPoint << " and has a weight of " << edges[i].first << std::endl; 
                tempEdges.push_back(edges[i]);
            }
        }

        // Find edge with lowest weight
        int smallest = 0;
        for(int i = 1; i < tempEdges.size(); i++){
            if(tempEdges[i].first < tempEdges[smallest].first)
                smallest = i;
        }

        // Push the smallest of the edges onto the MST graph
        mstEdges.push_back(tempEdges[smallest].second);

        // Push the non duplicate point from the new edge onto the graph
        if(!checkPoint(tempEdges[smallest].second.p1))
            mstPoints.push_back(tempEdges[smallest].second.p1);
        else
            mstPoints.push_back(tempEdges[smallest].second.p2);

        // Clear tempEdge vector
        tempEdges.clear();
        
        //std::cout << "Added point " << mstPoints.size() << " to MST" << std::endl;
    }
    std::cout << "  Minimal spanning tree completed" << std::endl;
    return;
}

float Mst::getWeight(Edge<float> e)
{
    // Returns the weight of the provided edge
    float weight = sqrt( pow((e.p2.x - e.p1.x), 2.0) + pow((e.p2.y - e.p1.y), 2.0));
    return weight;
}
#endif