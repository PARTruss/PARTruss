// Truss class implementation
// Note: IDX2C(i,j,ld) (((j)*(ld))+(i)) defined as shortcut for indexing into matrix
Truss::Truss(std::vector<Element> const & Elements, std::vector<Node> const & Nodes)
{
    // Iterate over elements and nodes and add them:
    for (int i = 0; i < Nodes.size(); i++)
    {
        if (this->addNode(Nodes[i]))
        {
            Nodes[i].setId(i);
        }
    }
    
    for (int i = 0; i < Elements.size(); i++)
    {
        if (this->addElement(Elements[i]))
        {
            Elements[i].setId(i);
            // _totalWeight += Elements[i].getWeight();
        }
    }
}

 // This is where the magic needs to happen...
 // Very much based off of the approach in 
 // https://www.mathworks.com/matlabcentral/fileexchange/31350-truss-solver-and-genetic-algorithm-optimzer?focused=5188720&tab=function#feedbacks
std::vector<double> const & Truss::solve(std::valarray<double> & Forces, 
    std::valarray<double> & Displacements, std::vector<double> & DCs )   
{
    int numNodes = this->_nodes.size();
    int numEdges = this->_elements.size();
    // Allocate a bunch of arrays:
    double* K = calloc( sizeof(double), pow(numNodes * 3, 2) ); // Global stiffness matrix
    bool* Re = calloc( sizeof(bool), numNodes * 3 );          // All restraints on each node 
    double* Ld = calloc( sizeof(double), numNodes * 3 );        // All loads on each node
    if ( K == NULL || Re == NULL || Ld == NULL )
    {
        std::cerr << "ERROR: Malloc failed to allocate memory in the truss solver member function!\n";
        exit(EXIT_FAILURE);
    }
    // Populate the load and restraint matrices from the nodes:
    for ( int i = 0 ; i < this->_nodes.size(); i ++ )
    {
        int nodeId = this->_nodes[i].getId();   // Note that node numbering starts at 0, not 1 
        Re[IDX2C(nodeId, 0, numNodes)] = this->_nodes.getConstX();
        Re[IDX2C(nodeId, 1, numNodes)] = this->_nodes.getConstY();
        Re[IDX2C(nodeId, 2, numNodes)] = this->_nodes.getConstZ();
        Ld[IDX2C(nodeId, 0, numNodes)] = this->_nodes.getLoadX();
        Ld[IDX2C(nodeId, 1, numNodes)] = this->_nodes.getLoadY();
        Ld[IDX2C(nodeId, 2, numNodes)] = this->_nodes.getLoadZ();
    }
    // Now for each element, add its global-coordinate stiffness matrix to the system matrix K
    // The locations where each quadrant of the element matrix fit into the system matrix
    //are based on the indices of the nodes at each end of the element.
    for (int i = 0; i < this->_elements.size(); i++)
    {             
        int node1 = this->_elements[i].getStart()->getId();
        int node2 = this->_elements[i].getEnd()->getId();
                        // Indices based on first node...and second node
        int indices[6] = { 3*node1, 3*node1+1, 3*node1+2, 3*node2, 3*node2+1, 3*node2+2 };
        double * local_stiffness = this->_elements[i].getLocalStiffness();
        // NOTE: Node numbering starts at 0; hence why the indexing above works.
        for (j = 0; j < 6; j ++ )
        {
            for (k = 0; k < 6; k++ )
            {
                // Add the values from the element's stiffness matrix onto the global matrix
                K[IDX2C(indices[j], indices[k], numNodes*3)] += local_stiffness[IDX2C(j, k, 6)];
            }
        }
    }
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
    n.setId()
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