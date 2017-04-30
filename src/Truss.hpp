#include <vector>
#include <iostream>

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

#include <cstdlib>
#ifndef IDX2C
// Macro to map from 
#define IDX2C(i,j,ld) (((j)*(ld))+(i))
#endif

// NOTE: Only implementing stiffness method for 2D (for now?)!


class Truss
{
public:
    Truss(std::vector<Element> & , std::vector<Node> & );
    std::vector<double *> solve(std::valarray<double> & Forces, std::valarray<double> & Displacements );
    void outputJSON(std::ostream & f);    // Output the solved json version to be visualized
    
// Getters:
    double const getWeight() const { return this->_totalWeight; }
//private:
    // Representation:
    std::vector<Element> _elements;
    std::vector<Node> _nodes;
    double _totalWeight = 0.0;
    double * _systemStiffnessMatrix = NULL;
    int _stiffnessMatrixSize = 0;
    
    
    // Utility:
    bool addNode( Node & n );
    bool addElement( Element & e );
};

//Helper function for outputting to JSON
template <typename T>
std::string array2string(T a)
{
    std::stringstream ss;
    ss << "[";
    for(size_t i = 0; i < a.size(); ++i)
    {
      if(i != 0)
        ss << ",";
      ss << a[i];
    }
    ss << "]";
    std::string s = ss.str();
    return s;
}

