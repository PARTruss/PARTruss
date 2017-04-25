#include <vector>
#include <string>
#include <cstdio>
#include <cmath>
#include <vector>
#include <string>
#include <iostream>

class trussNode
{
public:
	trussNode( double x, double y, double z, 
		bool moveX, bool moveY, bool moveZ, double Fx, double Fy, double Fz);
	void addNeighbor( trussNode const & adj, double section, double E, int element_index);

// Getters:
	double const getXCoord() const { return this->_coordX; }
	double const getYCoord() const { return this->_coordY; }
	double const getZCoord() const { return this->_coordZ; }
	double const getExtXForce() const { return this->_extForceX; }
	double const getExtYForce() const { return this->_extForceY; }
	double const getExtZForce() const { return this->_extForceZ; }
	bool const getMoveX() const {return this->_moveX; }
	bool const getMoveY() const {return this->_moveY; }
	bool const getMoveZ() const {return this->_moveZ; }

// Setters:
	bool setExtXForceIfUnset(double Fx);
	bool setExtYForceIfUnset(double Fy);
	bool setExtZForceIfUnset(double Fz);

// Helper functions:
	double const distTo( trussNode const & adj);
	double const xProjNormTo( trussNode const & adj);
	double const yProjNormTo( trussNode const & adj);
	double const zProjNormTo( trussNode const & adj);

private:

// Representation:
	bool _moveX;
	bool _moveY;
	bool _moveZ;
	double _coordX;
	double _coordY;
	double _coordZ;
	double _extForceX;
	double _extForceY;
	double _extForceZ;
	// Vector of neighboring (connected) vertices
	std::vector<trussNode> _neighbors;
	// Vector of member cross sectional areas corresponding to each neighbor
	std::vector<double> _sections;
	// Vector of Young's Moduli for each member
	std::vector<double> _E;
	// Vector of number of the member
	std::vector<int> _elementIndices;
};