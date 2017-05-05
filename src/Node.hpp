// Class for nodes in a truss
// NOTE: Only implementing stiffness method for 2D (for now?)!

#include <cstddef>
#include <valarray>
#include <cmath>
#include <cstdio>
#include <iostream>

class Node
{
public:
  Node ( double x, double y, double z );
  Node();

//Getters:
  std::valarray<double> getCoords() const { return this->_coords; }
  double const getX() const { return this->_coords[0]; }
  double const getY() const { return this->_coords[1]; }
  double const getZ() const { return this->_coords[2]; }

  std::valarray<double> getLoads() const { return this->_loads; }
  double const getLoadX() const { return this->_loads[0]; }
  double const getLoadY() const { return this->_loads[1]; }
  double const getLoadZ() const { return this->_loads[2]; }

  std::valarray<bool> getConstraints() const { return this->_constrained; }
  bool const getConstX() const { return this->_constrained[0]; }
  bool const getConstY() const { return this->_constrained[1]; }
  bool const getConstZ() const { return this->_constrained[2]; }

  int const getId() const { return this->_id; }
  void setId ( int n ) { this->_id = n; }
  void setPos ( double x, double y, double z ) { this->_coords[0] = x; this->_coords[1] = y; this->_coords[2] = z; }
// Utility:
  bool setConstraints ( std::valarray<bool> &  C );
  bool setLoad ( std::valarray<double> & P );
  bool addDisplacement ( double x, double y, double z );
  void setPosition ( double x, double y, double z );

private:
  // Representation:
  std::valarray<double> _coords = std::valarray<double> ( 3 );
  std::valarray<double> _loads =  std::valarray<double> ( 3 );
  std::valarray<bool> _constrained = std::valarray<bool> ( 3 );
  int _id;
};

