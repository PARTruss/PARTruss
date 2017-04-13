#include "trussNode.hpp"

// trussNode class implementation 

trussNode::trussNode(const std::string & label, double x, double y, double z, 
	bool moveX, bool moveY, bool moveZ, double Fx, double Fy, double Fz)
: _label(label), _coordX(x), _coordY(y), _coordZ(z), _moveX(moveX), 
_moveY(moveY), _moveZ(moveZ), _extForceX(Fx), _extForceY(Fy), _extForceZ(Fz)
{
	_neighbors = std::vector<trussNode>();
	_sections = std::vector<double>();
	_E = std::vector<double>();
}
void trussNode::addNeighbor(const trussNode & adj, double section, double E)
{
	_neighbors.push_back(adj);
	_sections.push_back(section);
	_E.push_back(E);
}

double const trussNode::distTo( trussNode const & adj)
{
	double dx = _coordX - adj.getXCoord();
	double dy = _coordY - adj.getYCoord();
	double dz = _coordZ - adj.getZCoord();
	return sqrt( dx*dx + dy*dy * dz*dz );
}

double const trussNode::xProjNormTo( trussNode const & adj)
{
	double mag = this->distTo(adj);
	double dx = abs( _coordX - adj.getXCoord() );
	if ( _coordX > adj.getXCoord() )
	{
		return dx/mag;
	}
	return -1 * (dx/mag);
}

double const trussNode::yProjNormTo( trussNode const & adj)
{
	double mag = this->distTo(adj);
	double dy = abs( _coordY - adj.getYCoord() );
	if ( _coordY > adj.getYCoord() )
	{
		return dy/mag;
	}
	return -1 * (dy/mag);
}

double const trussNode::zProjNormTo( trussNode const & adj)
{
	double mag = this->distTo(adj);
	double dz = abs( _coordY - adj.getYCoord() );
	if ( _coordY > adj.getYCoord() )
	{
		return dz/mag;
	}
	return -1 * (dz/mag);
}