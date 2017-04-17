#include <vector>
#include <string>
#include <cstdio>
#include <cmath>
#include <vector>
#include <string>

class trussNode
{
public:
	trussNode( double x, double y, double z, 
		bool moveX, bool moveY, bool moveZ, double Fx, double Fy, double Fz);
	void addNeighbor( trussNode const & adj, double section, double E);
	double const getXCoord() const { return this->_coordX; }
	double const getYCoord() const { return this->_coordY; }
	double const getZCoord() const { return this->_coordZ; }

private:
// Helper functions:
	double const distTo( trussNode const & adj);
	double const xProjNormTo( trussNode const & adj);
	double const yProjNormTo( trussNode const & adj);
	double const zProjNormTo( trussNode const & adj);

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
};