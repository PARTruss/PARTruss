// Truss class implementation
// NOTE: Only implementing stiffness method for 2D (for now?)!


Truss::Truss(std::vector<Element> const & Elements, std::vector<Node> const & Nodes)
{
    // Iterate over elements and nodes and add them:
    for (int i = 0; i < Nodes.size(); i++)
    {
        this->addNode(Nodes[i]);
    }
    
    for (int i = 0; i < Elements.size(); i++)
    {
        if this->addElement(Elements[i])
        {
            _totalWeight += Elements[i].getWeight();
        }
    }
}

 // This is where the magic needs to happen...
std::vector<double> const & Truss::solve(std::valarray<double> & Forces, 
    std::valarray<double> & Displacements, std::vector<double> & DCs )   
{
    int numNodes = this->_nodes.size();
    // std::vector<std::vector<double>> Soln;
    
    return NULL;
}

std::ostream Truss::outputJSON()
{
    // This needs to output JSON stuff...
    // Go through each node and output it in json formatted goodness
    // Go through each vertex and output it in json format
    return NULL;
}


bool Truss::addNode( Node & n )
{
    bool alreadyExists = false;
    for ( int i = 0; i < this->_nodes.size(); i++ )
    {
        alreadyExists |= ( n == this->_nodes[i]);
    }
    if ( alreadyExists ){
        std::cerr << "Error: this node has already been added to the truss. Skipping!\n";
        return false;
    }
    _nodes.push_back(n);
    return true;
}

bool Truss::addElement( Element & e )
{
    bool alreadyExists = false;
    for ( int i = 0; i < this->_elements.size(); i++ )
    {
        alreadyExists |= ( e == this->_elements[i]);
    }
    if ( alreadyExists ){
        std::cerr << "Error: this element has already been added to the truss. Skipping!\n";
        return false;
    }
    _elements.push_back(e);
    return true;   
}