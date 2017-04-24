// Date: 29 March 2017
// Description: Single threaded naive implementation of truss solver
// Based on: https://www.cs.princeton.edu/courses/archive/fall09/cos323/assign/truss/truss.html
// This provides a very good refresher on organizing a truss strucutre into a matrix:
// 	http://commons.bcit.ca/math/examples/civil/linear_algebra/
// https://en.wikipedia.org/wiki/Direct_stiffness_method - good overview of the direct siffness method
// For more on direct stiffness method: 
// https://www.mathworks.com/matlabcentral/fileexchange/31350-truss-solver-and-genetic-algorithm-optimzer?focused=5188720&tab=function#feedbacks

#include "json.hpp"
#include <cstdio>
#include <cstdlib>
#include <string>
#include <sstream>
#include <fstream>
#include <iostream>
#include "trussNode.hpp"
#include <utility>
#include <valarray>


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
    std::cout << j << '\n' << '\n';
    std::vector<trussNode> vertices;
    vertices.reserve(j["vertices"].size());
    // Aggregators for finding reaction forces:
    double externalX = 0;
    double externalY = 0;
    double externalZ = 0;
    std::vector<int> reactionVertexIndices;
    std::vector<std::pair<trussNode*, trussNode*>> edges;
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
    

    if (externalX != externalY != externalZ != 0)
    {
        std::cerr << "ERROR: Truss is not in static equilibrium!\n";
        exit(EXIT_FAILURE);
    }

    int edge_index = 0;
    // Now iterate over the edges and create the connections between trussNodes
    for (json::iterator itr = j["Edges"].begin(); itr != j["Edges"].end(); itr++)
    {
 		int e0 =(*itr)["Endpoints"][0];
 		int e1 =(*itr)["Endpoints"][1];
    	double E =(*itr)["ElasticModulus"];
    	double section =(*itr)["SectionArea"];
    	vertices[e0].addNeighbor( vertices[e1], section, E, edge_index );
    	vertices[e1].addNeighbor( vertices[e0], section, E, edge_index );
        edges.push_back(std::make_pair(&vertices[e0], &vertices[e1]));
        edge_index ++;
    }

    std::cout << "Total applied external forces:\n";
    std::cout << "Fx = " << externalX << std::endl;
    std::cout << "Fy = " << externalY << std::endl;
    std::cout << "Fz = " << externalZ << std::endl;

    // Need to verify that system is statically determinate and that there are enough known external forces?


   	// Verify that the forces on "fixed" nodes correctly add up to the required support forces:


    // Then translate to matrix
    // Solve
    // Write to json
    // Make available to the webgl renderer


    // Create a matrix with size = number of vertices * dimensionality (2 or 3)
    int row;
    row = vertices.size()*DIMENSIONALITY;
    if ( edges.size() > row )
    {
        std::cerr << "ERROR: System is underconstrained! Quitting :(\n";
        exit(EXIT_FAILURE);
    }
    else if (edges.size() < row )
    {
        std::cout << "Warning: System is overconstrained and may not be solveable...\n";
    }
    std::valarray<double> matrix (row * row);
    matrix = 0;
    std::valarray<double> equilibrium_forces (row);
    equilibrium_forces = 0;
    // For each vertex, check if it has each element as a connection (not all will)
    for (int v = 0; v < vertices.size(); v++)
    {
        // For each vertex, we generate three equations of equilibrium
        // Each row in the matrix corresponds to one equation of equilibrium
        // Each entry in each row corresponds to an edge coefficient
        //  if the vertex is part of that edge, the coefficient is the projection in x,y,z for that edge
        //  else the coefficient is 0
        for (int j = 0; j < edges.size(); j++)
        {
            if (edges[j].first == &vertices[v])
            {
                matrix[v*row + j] = vertices[v].xProjNormTo(*edges[j].first);
                matrix[(v+1)*row + j] = vertices[v].yProjNormTo(*edges[j].first);
                matrix[(v+2)*row + j] = vertices[v].zProjNormTo(*edges[j].first);
            }
            else if (edges[j].second == &vertices[v])
            {
                matrix[v*row + j] = vertices[v].xProjNormTo(*edges[j].second);
                matrix[(v+1)*row + j] = vertices[v].yProjNormTo(*edges[j].second);
                matrix[(v+2)*row + j] = vertices[v].zProjNormTo(*edges[j].second); 
            }
        }
    }


    for ( int i = 0; i < row; i++ )
    {
        for ( int j = 0; j < row; j++ )
        {
            std::cout << "\t" << matrix[i*row + j];
        }
        std::cout << "\n";
    }
	return 0;
}
