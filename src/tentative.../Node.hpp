// Class for nodes in a truss 
// NOTE: Only implementing stiffness method for 2D (for now?)!

#include <valarray>

class Node
{
public:
    Node(double x, double y, double z);

//Getters:
    std::valarray<double> const getCoords() const { return this->_coords; }
    double const getX() const { return this->_coords[0]; }
	double const getY() const { return this->_coords[1]; }
	double const getZ() const { return this->_coords[2]; }
	
    std::valarray<double> const getLoads() const { return this->_loads; }
    double const getLoadX() const { return this->_loads[0]; }
	double const getLoadY() const { return this->_loads[1]; }
	double const getLoadZ() const { return this->_loads[2]; }	
	
	bool const * const getConstraints() const { return this->_constrained; } 
	bool const getConstX() const { return this->_constrained[0]; }
	bool const getConstY() const { return this->_constrained[1]; }
	bool const getConstZ() const { return this->_constrained[2]; }
	
	int const getId() const { return this->_id; }
	void setId(int n) { this->_id = n; }
	
// Utility:
    bool operator==(Node const & n2);
    bool setConstraints(bool * C);
    bool setLoad(double * P);

private:
    // Representation:
    std::valarray<double> _coords(3);
    std::valarray<double> _loads(3);
    bool _constrained[3];
    static int _counter = 1;
    int _id;
};