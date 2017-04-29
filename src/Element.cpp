// Implementation of class for members in a truss
// NOTE: Only implementing stiffness method for 2D (for now?)!

#include "Element.hpp"
#include "Node.cpp"

Element::Element(Node & start, Node & end, double area, double E)
{
    _startNode = &start;
    _endNode = &end;
    _sectionArea = area;
    _youngModulus = E;
    // Additional processing and setting of members not directly from args:
    std::valarray<double> coord1(start.getCoords());
    std::valarray<double> coord2(end.getCoords());
    std::valarray<double> diffVec = coord1 - coord2;
    this->_length = std::pow(diffVec, 2.0).sum();
    this->_weight = _sectionArea * _length * _density;
    this->_K = _sectionArea * E / _length;  // Stiffness calculation
    this->_XYZRatio = diffVec/_length;  // Ratio of x,y,z deltas to length
    // Assign congruent transformation matrix as dot product of transpose of 
    //_XYZRatio with itself:
    double cx = this->_XYZRatio[0];
    double cy = this->_XYZRatio[1];
    double cz = this->_XYZRatio[2];
    double K = this->_K;
    double congruent_transformation[3][3] = 
    {   // Row-major format
        { K*pow(cx, 2), K*cy*cx, K*cz*cx },
        { K*cx*cy, K*pow(cy, 2), K*cz*cy },
        { K*cx*cz, K*cy*cz, K*pow(cz, 2) }
    };
    // Now create local stiffness by tiling congruent transformation in the 
    //following pattern:
    //
    //       1 -1
    //      -1  1
    //
    for (int i = 0; i < 36; i ++)
    {
        this->_localStiffness[i] = 0;
    }
    for (int i = 0; i < 6; i ++)
    {
        for (int j = 0; j < 6; j++)
        {
            double mult = ((i < 3 && j < 3) || (i>2 && j>2) ) ? (1.0) : (-1.0);
            this->_localStiffness[i*6+j] = mult * congruent_transformation[i%3][j%3];
        }
    }
    double _yieldStress;  // Stress at which material fails
}

bool Element::operator==(Element const & e2)
{
    return ( this->_startNode == e2.getStart() && this->_endNode == e2.getEnd()
        || this->_startNode == e2.getEnd() && this->_endNode == e2.getStart() );
}