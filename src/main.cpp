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
#include <istream>
#include <ostream>
#include <ctime>

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

struct timespec times[12];
string timeMeanings[] = {
    "Read input data file",
    "Parse vertices from input",
    "Parse edges from input",
    "Calculating load/restraint matrices",
    "Calculating global stiffness matrix",
    "Calculating reduced matrix",
    "Solving on GPU",
    "Calculating displacements",
    "Calculating forces/stresses",
    "Solving system complete",
    "Writing to output file"
}


int DEBUGLVL=0;
int COMMENTARY=0;

// Set dimensionality of the simulation:
int DIMENSIONALITY = 2;
// for convenience
using json = nlohmann::json;

void usage( int argc, char ** argv )
{
	if ( argc != 2 )
	{
        std::cerr<<"Usage is:"<<std::endl;
        std::cerr<<argv[0]<<" <inputFile> [-vvvvv] [-ccccc] -o <outFile>"<<std::endl;
        std::cerr<<argv[0]<<" -o -   (to read from STDIN, write to STDOUT)"<<std::endl;
		exit(EXIT_FAILURE);
	}
}

void parseArgs(int argc, char* argv[], std::istream* &input, std::ostream* &output){
    input = NULL;
    output = NULL;
    DEBUGLVL = 0;
    COMMENTARY = 0;
    char* outfile = NULL;
    char* infile = NULL;
    for(int i=1;i<argc;i++){
        if(argv[i][0]=='-'){
            int c = 1;
            while(argv[i][c]!='\0'){
                if(argv[i][c]=='v' || argv[i][c]=='V')
                    DEBUGLVL++;
                if(argv[i][c]=='c' || argv[i][c]=='C')
                    COMMENTARY++;
                if(argv[i][c]=='o'){
                    if(argc<i+2){
                        usage(argc, argv);
                    }
                    outfile = argv[i+1];
                    i++;
                    break;
                }
                c++;  
            }
        }
        else{
            infile = argv[i];
        }
    }
    if(infile == NULL)
        input = &std::cin;
    else
        input = new std::ifstream(infile);
    if(outfile == NULL) usage(argc, argv);
    if(strcmp(outfile,"-")==0)
        output = &std::cout;
    else
        output = new std::ofstream(outfile);
}

int main( int argc, char ** argv )
{
    std::istream *input;
    std::ostream *output;
    parseArgs(argc, argv, input, output);
	//usage(argc, argv);
	// TODO: add err handling for file reading...
    json j;
    if(COMMENTARY > 0)
        std::cout << "Reading input file"<<std::endl;
    clock_gettime(CLOCK_MONOTONIC, &times[0]);
    *input >> j;
    clock_gettime(CLOCK_MONOTONIC, &times[1]);
    
if(DEBUGLVL > 2){
    std::cout << "Vertices\n";
    for (json::iterator i = j["Vertices"].begin(); i != j["Vertices"].end(); i++) {
        std::cout << *i << '\n';
    }
    std::cout << "\nEdges\n";
    for (json::iterator i = j["Edges"].begin(); i != j["Edges"].end(); i++) {
        std::cout << *i << '\n';
    }
    std::cout << "\n\n";
}

    std::vector<Node> vertices(j["Vertices"].size());
    std::vector<Element> edges(j["Edges"].size());

    // Loop through all vertices and add them to the vector
    for (int pos = 0; pos < j["Vertices"].size(); pos++)
    {
        double x = j["Vertices"][pos]["XYZPosition"][0];
	double y = j["Vertices"][pos]["XYZPosition"][1];
        double z = j["Vertices"][pos]["XYZPosition"][2];
        bool moveX = j["Vertices"][pos]["Anchored"][0];
        bool moveY = j["Vertices"][pos]["Anchored"][1];
        bool moveZ = j["Vertices"][pos]["Anchored"][2];
        double Fx = j["Vertices"][pos]["XYZAppliedForces"][0];
        double Fy = j["Vertices"][pos]["XYZAppliedForces"][1];
        double Fz = j["Vertices"][pos]["XYZAppliedForces"][2];
        Node &n = vertices[pos];
        n.setPosition(x, y, z);
        std::valarray<bool> constraints {moveX, moveY, moveZ};
        std::valarray<double> loads {Fx, Fy, Fz };
        n.setConstraints(constraints);
        n.setLoad(loads);
        n.setId(pos);
        //vertices[pos] = n;
    }
    clock_gettime(CLOCK_MONOTONIC, &times[2]);

    // Now iterate over the edges and create the connections between trussNodes
    for (int pos = 0; pos < j["Edges"].size(); pos++)
    {
     	int e0 = j["Edges"][pos]["Endpoints"][0];
     	int e1 = j["Edges"][pos]["Endpoints"][1];
    	double E = j["Edges"][pos]["ElasticModulus"];
    	double section = j["Edges"][pos]["SectionArea"];
	    Element &e = edges[pos];
        e.setElem(vertices[e0], vertices[e1], section, E);
        e.setId(pos);
    }
    clock_gettime(CLOCK_MONOTONIC, &times[3]);

    // Then translate to Truss
    
    Truss t = Truss(edges,vertices);
    
    // Solve
    int retval = t.solve();
    clock_gettime(CLOCK_MONOTONIC, &times[10]);

    if ( retval != 0 )
    {
        if(COMMENTARY>2)
            std::cout << "WOO! Truss has been solved!\n";
    }
    else
    {
      std::cerr << "ERROR: Something went wrong in solving the truss :(\n";
    }
    // Write to json  
    // Make available to the webgl renderer??
    t.outputJSON(*output);
    clock_gettime(CLOCK_MONOTONIC, &times[11]);
    if(output->rdbuf() != std::cout.rdbuf())
        dynamic_cast<std::ofstream*>(output)->close();
    float elapsed[11];
    for(int i=0;i<11;i++){
        elapsed[i] = (times[i+1].tv_sec - times[0].tv_sec);
        elapsed[i] += (times[i+1].tv_nsec - times[0].tv_nsec) / 1000000000.0;
    }

    for(int i=0;i<11;i++){
        if(i==0)
            std::cout<<elapsed[i]<<" ("<<elapsed[i]<<")";
        else
            std::cout<<elapsed[i]<<" ("<<elapsed[i]-elapsed[i-1]<<")";
        cout<<"\t"<<timeMeanings[i]<<endl;
    }

    return 0;
}
