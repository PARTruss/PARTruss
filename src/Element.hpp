// Class for members in a truss
// NOTE: Only implementing stiffness method for 2D (for now?)!
// Each element object contains reference to the two nodes it connects 
// Each element also contains the cross sectional area, elastic modulus, length, 

#include <cstddef>
#include <cmath>
#include <valarray>

#ifndef NODE
#include "Node.hpp"
#define NODE
#endif

class Element
{
  public:
    // Constructor:
    Element(Node &, Node &, double, double);
    
    // Utility:
    bool operator==(Element const & e2);
    
    // Setter:
    void setId(int id) { this->_id = id; }
    
    // Getters:
    Node const * const getStart() const { return this->_startNode; }
    Node const * const getEnd() const { return this->_endNode; }
    double const * const getLocalStiffness() const { return this->_localStiffness; }
    double getWeight() const { return this->_weight; }
    int const getId() const { return this->_id; }
    double const getMod() const { return this->_youngModulus; }
    double const getArea() const { return this->_sectionArea; } 
  
  //private:
    // Representation:
    Node * _startNode;
    Node * _endNode;
    double _sectionArea;
    double _length;
    std::valarray<double> _XYZRatio = std::valarray<double>(3);
    double _localStiffness[36];  // 6x6 local stiffness matrix (T'kT)
    double _youngModulus;
    double _weight;
    double _K;
    constexpr static double _density = 0.284; // Hard-coded density of steel (lbs/in^3)
    constexpr static double _yieldStress = 36000;  // Hard coded stress at which material fails (psi)
    int _id = -1;   // Default invalid value
};