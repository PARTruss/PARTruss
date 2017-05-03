// Implementation of class for nodes in a truss 

#ifndef NODE
#include "Node.hpp"
#define NODE
#endif
#include <iostream>

Node::Node(double x, double y, double z)
{
    this->_coords = std::valarray<double>(3);
    this->_coords[0] = x;
    this->_coords[1] = y;
    this->_coords[2] = z;
}

bool Node::setLoad(std::valarray<double> & P)
{
    bool fail = false;
    for (int i = 0; i < 3; i ++)
    {
    	if (!this->_constrained[i] && P[i] > 0)
    	{
    		this->_loads[i] = P[i];
    	}
    	else if (P[i] > 0)
    	{
    	    fail=true;
	    	std::cerr << "Can't set a vertex load where there is a constraint.\n";
        }
    }
	return !fail;
}

bool Node::setConstraints(std::valarray<bool> & C)
{
    bool fail = false;
    for (int i = 0; i < 3; i ++)
    {
    	if (this->_loads[i] == 0 && C[i])
    	{
    		this->_constrained[i] = C[i];
    	}
    	else if (C[i])
    	{
    	    fail=true;
	    	std::cerr << "Can't set a vertex load where there is a constraint.\n";
        }
    }
	return !fail;
}


bool Node::addDisplacement(double x, double y, double z)
{
  if ( (x != 0 && this->_constrained[0]) || (y != 0 && this->_constrained[1]) || (z != 0 && this->_constrained[2]) )
  {
    std::cerr << "ERROR: You tried to move a node in a direction it can't move in!\n";
  }
  this->_coords[0] += x;
  this->_coords[1] += y;
  this->_coords[2] += z;
  return true;
}
