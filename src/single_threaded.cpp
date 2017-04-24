// Date: 29 March 2017
// Description: Single threaded naive implementation of truss solver
// Based on: https://www.cs.princeton.edu/courses/archive/fall09/cos323/assign/truss/truss.html
// This provides a very good refresher on organizing a truss strucutre into a matrix:
// 	http://commons.bcit.ca/math/examples/civil/linear_algebra/
#include "json.hpp"
#include <cstdio>
#include <cstdlib>
#include <string>
#include <sstream>
#include <fstream>
#include <iostream>
#include "trussNode.hpp"

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
    std::cout << j << '\n' << '\n';
    std::vector<trussNode> vertices;
    vertices.reserve(j["vertices"].size());
    // Aggregators for finding reaction forces:
    double externalX = 0;
    double externalY = 0;
    double externalZ = 0;
    std::vector<int> reactionVertexIndices;
    // Loop through all vertices and add them to the vector
    int index = 0;
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
        vertices.push_back( trussNode(x, y, z, moveX, moveY, moveZ, Fx, Fy, Fz) );
        // Collect all vertices that have unfilled reaction forces
        if ( vertices.end()->getMoveX() || vertices.end()->getMoveY() || vertices.end()->getMoveZ() )
        {
        	reactionVertexIndices.push_back(index);
        }
	    // Sum the external forces in the X, Y, and Z directions:
        externalX += vertices.end()->getExtXForce(); 
        externalY += vertices.end()->getExtYForce();
        externalZ += vertices.end()->getExtZForce();
        index += 1;
    }

    // Now iterate over the edges and create the connections between trussNodes
    for (json::iterator itr = j["Edges"].begin(); itr != j["Edges"].end(); itr++)
    {
 		int e0 =(*itr)["Endpoints"][0];
 		int e1 =(*itr)["Endpoints"][1];
    	double E =(*itr)["ElasticModulus"];
    	double section =(*itr)["SectionArea"];
    	vertices[e0].addNeighbor( vertices[e1], section, E );
    	vertices[e1].addNeighbor( vertices[e0], section, E );
    }

    std::cout << "Total applied external forces:\n";
    std::cout << "Fx = " << externalX << std::endl;
    std::cout << "Fy = " << externalY << std::endl;
    std::cout << "Fz = " << externalZ << std::endl;

    // Need to verify that system is statically determinate and that there are enough known external forces


   	// Verify that the forces on "fixed" nodes correctly add up to the required support forces:


    // Then translate to matrix
    // Solve
    // Write to json
    // Make available to the webgl renderer
	return 0;
}
