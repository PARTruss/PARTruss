#include "trussNode.hpp"
#include <iostream>

// trussNode class implementation 

trussNode::trussNode( double x, double y, double z, 
	bool moveX, bool moveY, bool moveZ, double Fx, double Fy, double Fz)
: _coordX(x), _coordY(y), _coordZ(z), _moveX(moveX), 
_moveY(moveY), _moveZ(moveZ), _extForceX(Fx), _extForceY(Fy), _extForceZ(Fz)
{
	_neighbors = std::vector<trussNode>();
	_sections = std::vector<double>();
	_E = std::vector<double>();
	_elementIndices = std::vector<int>();
}
void trussNode::addNeighbor(const trussNode & adj, double section, double E, int element_index)
{
	_neighbors.push_back(adj);
	_sections.push_back(section);
	_E.push_back(E);
	_elementIndices.push_back(element_index);
}

double const trussNode::distTo( trussNode const & adj)
{
	double dx = _coordX - adj.getXCoord();
	double dy = _coordY - adj.getYCoord();
	double dz = _coordZ - adj.getZCoord();
	return sqrt( dx*dx + dy*dy + dz*dz );
}

double const trussNode::xProjNormTo( trussNode const & adj)
{
	double mag = this->distTo(adj);
	double dx = adj.getXCoord() - _coordX;
	if (mag == 0) std::cerr	<< "Zero size magnitude! Not good!\n";
	return dx/mag;
}

double const trussNode::yProjNormTo( trussNode const & adj)
{
	double mag = this->distTo(adj);
	double dy = adj.getYCoord() - _coordY;
	if (mag == 0) std::cerr	<< "Zero size magnitude! Not good!\n";
	return dy/mag;
}

double const trussNode::zProjNormTo( trussNode const & adj)
{
	double mag = this->distTo(adj);
	double dz = adj.getYCoord() - _coordY;
	if (mag == 0) std::cerr	<< "Zero size magnitude! Not good!\n";
	return dz/mag;
}

bool trussNode::setExtXForceIfUnset(double Fx)
{
	if (this->_extForceX == 0)
	{
		this->_extForceX = Fx;
		return true;
	}
	return false;
}
bool trussNode::setExtYForceIfUnset(double Fy)
{
	if (this->_extForceY == 0)
	{
		this->_extForceY = Fy;
		return true;
	}
	return false;
}
bool trussNode::setExtZForceIfUnset(double Fz)
{
	if (this->_extForceZ == 0)
	{
		this->_extForceZ = Fz;
		return true;
	}
	return false;
}