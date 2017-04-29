// Implementation of class for nodes in a truss 

Node(double x, double y, double z)
{
    this->_coords = std::valarray(3);
    this->_coords[0] = x;
    this->_coords[1] = y;
    this->_coords[2] = z;
    this->id = (this->_counter++);
}

bool Node::operator==( Node const & n2 )
{
    // If a node has the same coords as another, they are equivalent
    return this->_coords == n2.getCoords();
}

bool Node::setLoad(double * P)
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
	    	std::cerr << "Can't set a vertex load where there is a constraint.\n"
        }
    }
	return !fail;
}

bool Node::setConstraints(bool * C)
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
	    	std::cerr << "Can't set a vertex load where there is a constraint.\n"
        }
    }
	return !fail;
}