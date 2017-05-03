// Date: 29 March 2017
// Description: Single threaded naive implementation of truss solver
// Based on: https://www.cs.princeton.edu/courses/archive/fall09/cos323/assign/truss/truss.html
// This provides a very good refresher on organizing a truss strucutre into a matrix:
// 	http://commons.bcit.ca/math/examples/civil/linear_algebra/
// https://en.wikipedia.org/wiki/Direct_stiffness_method - good overview of the direct siffness method
// For more on direct stiffness method: 
// https://www.mathworks.com/matlabcentral/fileexchange/31350-truss-solver-and-genetic-algorithm-optimzer?focused=5188720&tab=function#feedbacks

#include <cstdio>
#include <cstdlib>
#include <string>
#include <sstream>
#include <fstream>
#include <iostream>
#include <utility>
#include <valarray>
#include <iomanip>

#ifndef JSON
#include "json.hpp"
#define JSON
#endif

#ifndef ELEM
#include "Element.hpp"
#define ELEM
#endif

#ifndef NODE
#include "Node.hpp"
#define NODE
#endif

#ifndef TRUSS
#include "Truss.hpp"
#define TRUSS
#endif

#define DEBUG 3

// Set dimensionality of the simulation:
int DIMENSIONALITY = 2;
// for convenience
using json = nlohmann::json;

void usage( int argc, char ** argv )
{
	if ( argc != 2 )
	{
		fprintf(stderr, "USAGE: %s <truss filename>\n", argv[0]);
		exit(EXIT_FAILURE);
	}
}

int main( int argc, char ** argv )
{
	usage(argc, argv);
	// TODO: add err handling for file reading...
	std::ifstream i(argv[1]);
    json j;
    i >> j;
    
#if DEBUG > 2
    std::cout << "Vertices\n";
    for (json::iterator i = j["Vertices"].begin(); i != j["Vertices"].end(); i++) {
        std::cout << *i << '\n';
    }
    std::cout << "\nEdges\n";
    for (json::iterator i = j["Edges"].begin(); i != j["Edges"].end(); i++) {
        std::cout << *i << '\n';
    }
    std::cout << "\n\n";
#endif

    std::vector<Node> vertices;
    vertices.reserve(j["vertices"].size());
    std::vector<Element> edges;
    // Loop through all vertices and add them to the vector
    for (json::iterator itr = j["Vertices"].begin(); itr != j["Vertices"].end(); itr++)
    {
        double x = (*itr)["XYZPosition"][0];
		double y =(*itr)["XYZPosition"][1];
        double z =(*itr)["XYZPosition"][2];
        bool moveX =(*itr)["Anchored"][0];
        bool moveY =(*itr)["Anchored"][1];
        bool moveZ =(*itr)["Anchored"][2];
        double Fx =(*itr)["XYZAppliedForces"][0];
        double Fy =(*itr)["XYZAppliedForces"][1];
        double Fz =(*itr)["XYZAppliedForces"][2];
        Node n = Node(x, y, z);
        std::valarray<bool> constraints {moveX, moveY, moveZ};
        std::valarray<double> loads {Fx, Fy, Fz };
        n.setConstraints(constraints);
        n.setLoad(loads);
        vertices.push_back( n );
    }

    // Now iterate over the edges and create the connections between trussNodes
    for (json::iterator itr = j["Edges"].begin(); itr != j["Edges"].end(); itr++)
    {
 		int e0 =(*itr)["Endpoints"][0];
 		int e1 =(*itr)["Endpoints"][1];
    	double E =(*itr)["ElasticModulus"];
    	double section =(*itr)["SectionArea"];
    	Element e = Element(vertices[e0], vertices[e1], section, E);
        edges.push_back( e );
    }
    // Then translate to Truss
    
    Truss t = Truss(edges,vertices);
    
    // Solve
    if ( t.solve() )
    {
      std::cout << "WOO! Truss has been solved!\n";
    }
    else
    {
      std::cerr << "ERROR: Something went wrong in solving the truss :(\n";
    }
    // Write to json  
    // Make available to the webgl renderer??
    t.outputJSON();
    return 0;
}
