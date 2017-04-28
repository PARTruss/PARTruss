// Implementation of class for members in a truss
// NOTE: Only implementing stiffness method for 2D (for now?)!

Element::Element(Node const & start, Node const & end, double area, double E)
{
    _startNode = &start;
    _endNode = &end;
    _sectionArea = area;
    _youngModulus = E;
    // Additional processing and setting of members not directly from args:
    std::valarray coord1(start.getCoords());
    std::valarray coord2(end.getCoords());
    std::valarray diffVec = coord1 - coord2;
    _length = sum(pow(diffVec, 2));
    _weight = _sectionArea * _length * _density;
    _K = _sectionArea * E / _length;  // Stiffness calculation
    _XYZRatio = diffVec/_length;  // Ratio of x,y,z deltas to length
    // Assign congruent transformation matrix as dot product of transpose of 
    //_XYZRatio with itself:
    double cx = _ZYXRatio[0];
    double cy = _ZYXRatio[1];
    double cz = _ZYXRatio[2];
    double congruent_transformation[3][3] = 
    {   // Row-major format
        { _K*pow(cx, 2), _K*cy*cx, _K*cz*cx },
        { _K*cx*cy, _K*pow(cy, 2), _K*cz*cy },
        { _K*cx*cz, _K*cy*cz, _K*pow(cz, 2) }
    };
    // Now create local stiffness by tiling congruent transformation in the 
    //following pattern:
    //
    //       1 -1
    //      -1  1
    //
    for (int i = 0; i < 36; i ++)
    {
        _localStiffness[i] = 0;
    }
    for (int i = 0; i < 6; i ++)
    {
        for (int j = 0; j < 6; j++)
        {
            double mult = ((i < 3 && j < 3) || (i>2 && j>2) ) ? (1.0) : (-1.0);
            _localStiffness[i*6+j] = mult * congruent_transformation[i%3][j%3];
        }
    }
    double _yieldStress;  // Stress at which material fails
    this->setId(this->_counter++);
}

bool Element::operator==(Element const & e2)
{
    return ( this->_startNode == e2.getStart() && this->_endNode == e2.getEnd()
        || this->_startNode == e2.getEnd() && this->_endNode == e2.getStart() );
}