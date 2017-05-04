// Truss class implementation
// Note: IDX2C(i,j,ld) (((i)*(ld))+(j)) defined in header as shortcut for indexing into matrix

#include <iostream>
#include <fstream>
#include <cstddef>
#include <valarray>
#include <cmath>
#ifndef TRUSS
#include "Truss.hpp"
#define TRUSS
#endif
#ifndef JSON
#include "json.hpp"
#define JSON
#endif

using json = nlohmann::json;

extern int solveMatrix(double *A_in, int n, double *b_in, double *x_out);

bool nodeEqual(Node n1, Node n2 )
{
    return n1.getX() == n2.getX() &&
        n1.getY() == n2.getY() &&
        n1.getZ() == n2.getZ();
}

bool elemEqual(Element & e1, Element & e2)
{
  return ( nodeEqual(*e1.getStart(), *e2.getEnd()) &&
        nodeEqual(*e1.getEnd(), *e2.getStart()) ) ||
      ( nodeEqual(*e1.getStart(), *e2.getStart()) &&
      nodeEqual(*e1.getEnd(), *e2.getEnd()) );
}

Truss::Truss(std::vector<Element> & Elements, std::vector<Node> & Nodes)
{
    // Iterate over elements and nodes and add them:
    for (int i = 0; i < Nodes.size(); i++)
    {
        if (this->addNode(Nodes[i]))
        {
            this->_nodes[this->_nodes.size()-1].setId(i);
            Nodes[i].setId(i);
        }
    }
    
    for (int i = 0; i < Elements.size(); i++)
    {
        if (this->addElement(Elements[i]))
        {
            this->_elements[this->_elements.size()-1].setId(i);
            // _totalWeight += Elements[i].getWeight();
        }
    }
}

 // This is where the magic needs to happen...
 // Very much based off of the approach in 
 // https://www.mathworks.com/matlabcentral/fileexchange/31350-truss-solver-and-genetic-algorithm-optimzer?focused=5188720&tab=function#feedbacks
bool Truss::solve()   
{
    int numNodes = this->_nodes.size();
    int numEdges = this->_elements.size();
    // Allocate a bunch of arrays:
    double* K = (double*)calloc( sizeof(double), pow(numNodes * 3, 2) ); // Global stiffness matrix
    bool* Re = (bool*)calloc( sizeof(bool), numNodes * 3 );          // All restraints on each node 
    double* Ld = (double*)calloc( sizeof(double), numNodes * 3 );        // All loads on each node
    if ( K == NULL || Re == NULL || Ld == NULL )
    {
        std::cerr << "ERROR: Malloc failed to allocate memory in the truss solver member function!\n";
        return false;
    }
    // Populate the load and restraint matrices from the nodes:
    for ( int i = 0 ; i < numNodes; i ++ )
    {
        int nodeId = this->_nodes[i].getId();   // Note that node numbering starts at 0, not 1
        std::cout << "Node id: " << nodeId << "\n";
        std::cout << "Farthest index: "<<IDX2C(nodeId, 2, numNodes) << "\n";
        Re[IDX2C(nodeId, 0, 3)] = this->_nodes[i].getConstX();
        Re[IDX2C(nodeId, 1, 3)] = this->_nodes[i].getConstY();
        Re[IDX2C(nodeId, 2, 3)] = this->_nodes[i].getConstZ();
        Ld[IDX2C(nodeId, 0, 3)] = this->_nodes[i].getLoadX();
        Ld[IDX2C(nodeId, 1, 3)] = this->_nodes[i].getLoadY();
        Ld[IDX2C(nodeId, 2, 3)] = this->_nodes[i].getLoadZ();
        std::cout << "Node " << i << " constraints:\n";
        std::cout << Re[IDX2C(nodeId, 0, numNodes)] << std::endl;
        std::cout << Re[IDX2C(nodeId, 1, numNodes)] << std::endl;
        std::cout << Re[IDX2C(nodeId, 2, numNodes)] << std::endl;
    }
    // Now for each element, add its global-coordinate stiffness matrix to the system matrix K
    // The locations where each quadrant of the element matrix fit into the system matrix
    //are based on the indices of the nodes at each end of the element.
    for (int i = 0; i < numEdges; i++)
    {             
        int node1 = this->_elements[i].getStart()->getId();
        int node2 = this->_elements[i].getEnd()->getId();
                        // Indices based on first node...and second node
        int indices[6] = { 3*node1, 3*node1+1, 3*node1+2, 3*node2, 3*node2+1, 3*node2+2 };
        const double * local_stiffness = this->_elements[i].getLocalStiffness();
        // NOTE: Node numbering starts at 0; hence why the indexing above works.
        std::cout << "Accessing local stiffness matrices to construct global matrix:\n";
        for (int j = 0; j < 6; j ++ )
        {
            for (int k = 0; k < 6; k++ )
            {
                // Add the values from the element's stiffness matrix onto the global matrix
                std::cout << local_stiffness[IDX2C(j, k, 6)] << "\t";
                K[IDX2C(indices[j], indices[k], numNodes*3)] += local_stiffness[IDX2C(j, k, 6)];
            }
            std::cout << "\n";
        }
    }
    // Now that the global stiffness matrix exists, save it!
    this->_systemStiffnessMatrix = K;
    this->_stiffnessMatrixSize = numNodes * 3;
    // Now need to filter the stiffness matrix based on which nodes are/aren't restrained:
    // (it's a degrees of freedom indexing vector)
    std::vector<int> dof;
    for (int i = 0; i < numNodes*3; i++)
    {   
        // Iterate over Re as a flattened matrix (vector where each group of three rows
        // is the x,y,z restrictions on movement of that node)
        if (!Re[i]) // If that direction is a unrestrained degree of free motion for the node, save the
                    // value. These indices will be filters on which the system stiffness matrix
                    // and load vector are sliced.
        {
            dof.push_back(i);
        }
    }
   
    std::cout << "Printing system matrix: (full)\n";
    for (int i = 0; i < 3*numNodes; i++)
    {
      for (int j = 0; j < 3*numNodes; j++)
      {
        std::cout << K[IDX2C(i, j, numNodes*3)] << "\t";
      }
      std::cout << "\n";
    }
    std::cout << "Printed the system matrix.\n";


    // TODO: use thrust or something to efficiently filter the K matrix and Ld vector (view Ld as a flattened matrix)?
    // Entire row-columns that correspond to the axis on which a node is constrained must be dropped.
    // Corresponding forces in restrained directions for each node in the external force matrix/vector
    //are also to be dropped.
    // Construct the simplified system stiffness matrix (missing entries corresponding to indices in 
    //degrees_of_freedom matrix)
    double * A = (double*)calloc( sizeof(double), pow(dof.size(), 2) );
    // Vector to hold the know external forces:
    double * f = (double*)calloc( sizeof(double), dof.size() );
    // Vector to hold the solved displacements of moveable nodes
    double * d = (double*)calloc( sizeof(double), dof.size() );
    if ( A == NULL || f == NULL || d == NULL )
    {
        std::cerr << "ERROR: Malloc failed to allocate memory in the truss solver member function!\n";
        return false;
    }
    // Copying over just the values that matter into A and f
    for (int i = 0; i < dof.size(); i++) {
        for (int k = 0; k < dof.size(); k++) {
            A[IDX2C(i, k, dof.size())] = K[IDX2C(dof[i], dof[k], 3*numNodes)];
        }
        f[i] = Ld[dof[i]];  // This turns the 3 x numNodes matrix Ld into a filtered column vector
    }
    for (int i = 0; i < dof.size(); i++) {
        for (int k = 0; k < dof.size(); k++) {
            std::cout << A[IDX2C(i, k, dof.size())] << " \t";
        }
        std::cout << "\n";
        //f[i] = Ld[dof[i]];  // This turns the 3 x numNodes matrix Ld into a filtered column vector
    }
    for (int i = 0; i < dof.size(); i++) {
        std::cout << f[i];
    }
    // At this point the system can now be solved for the displacement of each node!
    // Formula is d = A\f in MATLAB, or d = A^-1 f in more mathy terms.
    int cuda_status = solveMatrix( A, dof.size(), f, d );
    if ( cuda_status != 0 )
    {
      std::cerr << "ERROR: Call to CuSolve in truss solve member function failed!\n";
      std::cerr << "\tReturn value: " << cuda_status << std::endl;
      return false;
    }
    // Now expand the total displacement matrix:
    double * D = (double *)calloc( sizeof(double), numNodes * 3);
    if ( D == NULL )
    {
      std::cerr << "ERROR: could not allocate memory for full displacement vector.\n";
      return false;
    }
    for ( int i = 0; i < dof.size(); i++ )
    {
      // All other displacements are 0, implicitly by calloc
      D[dof[i]] = d[i];
    }
    // Update all nodes with their respective displacements:
    for ( int i = 0; i < numNodes; i++ )
    {
      int nodeId = this->_nodes[i].getId();
      this->_nodes[i].addDisplacement(D[nodeId * 3], D[nodeId * 3 + 1], D[nodeId * 3 + 2]);
    }
    // Now solve for the force in each element and update it:
    for ( int i = 0; i < numEdges; i++ )
    {
      int elemId = this->_elements[i].getId();
      int node1 = this->_elements[i].getStart()->getId();
      int node2 = this->_elements[i].getEnd()->getId();
      int indices[6] = { 3*node1, 3*node1+1, 3*node1+2, 3*node2, 3*node2+1, 3*node2+2 };
      double disp_delt[3] = {
        D[indices[3]] - D[indices[0]], 
        D[indices[4]] - D[indices[1]], 
        D[indices[5]] - D[indices[2]] 
        };
      std::valarray<double> XYZRatio = this->_elements[i].getXYZRatios();
      this->_elements[i].setForce( disp_delt[0]*XYZRatio[0] + disp_delt[1]*XYZRatio[1] + disp_delt[2]*XYZRatio[2] );
      std::cout << disp_delt[0]*XYZRatio[0] + disp_delt[1]*XYZRatio[1] + disp_delt[2]*XYZRatio[2] << std::endl;
      
    }
    // Note that indices not stored in dof have a 0 displacement for that node and coordinate direction (xyz).
    // Then the force in each element is k( (1/L)(dx, dy, dz)dot(displacement_node_2 - displacement_node1) )
    // Where displacement of each node is a 3-vector for the x,y,z components.
    // At this point the changes in each location should be written back, the forces in each element should be stored,
    // And json output should be written out.
          
    return true;
}

void Truss::outputJSON()
{
    // This needs to output JSON stuff...
    // Go through each node and output it in json formatted goodness
    // Go through each vertex and output it in json format
    int numNodes = this->_nodes.size();
    int numEdges = this->_elements.size();
    json j;
    std::string vertices = "[";
    for (int i = 0; i < numNodes; i++)
    {
        vertices += "{\"XYZPosition\": " +  array2string(this->_nodes[i].getCoords());
        vertices += ", \"XYZAppliedForces\": " + array2string(this->_nodes[i].getLoads());
        vertices += ", \"Anchored\": " + array2string(this->_nodes[i].getConstraints() ) + "}";
        if ( i < numNodes - 1) { vertices += ", "; }
    }
    vertices += "]";
    j["Vertices"] = json::parse(vertices);
    
    std::string edges = "[";
    for (int i = 0; i < numEdges; i++)
    {
        std::array<int,2> endpoints = {this->_elements[i].getStart()->getId(), this->_elements[i].getEnd()->getId()};
        edges += "{\"Endpoints\": " + array2string(endpoints);
    	edges += ", \"ElasticModulus\": " +  std::to_string(this->_elements[i].getMod());
    	edges += ", \"SectionArea\": " + std::to_string(this->_elements[i].getArea());
    	edges += ", \"Force\": " + std::to_string(this->_elements[i].getForce());
    	edges += ", \"Stress\": " + std::to_string(this->_elements[i].getStress()) + '}';
        if ( i < numEdges - 1) { edges += ", "; }
    }
    edges += "]";
   std::cout << edges;
    j["Edges"] = json::parse(edges);
    std::ofstream o("trussOut.json");
    o << std::setw(4) << j << std::endl;
}


bool Truss::addNode( Node & n )
{
    bool alreadyExists = false;
    for ( int i = 0; i < this->_nodes.size(); i++ )
    {
        alreadyExists |= nodeEqual(n, this->_nodes[i] );
    }
    if ( alreadyExists ){
        std::cerr << "Error: this node has already been added to the truss. Skipping!\n";
        return false;
    }
    _nodes.push_back(n);
    std::cout << "Node addition successful.\n";
    return true;
}

bool Truss::addElement( Element & e )
{
    bool alreadyExists = false;
    for ( int i = 0; i < this->_elements.size(); i++ )
    {
        alreadyExists |= elemEqual(this->_elements[i], e);
    }
    if ( alreadyExists ){
        std::cerr << "Error: this element has already been added to the truss. Skipping!\n";
        return false;
    }
    _elements.push_back(e);
    return true;   
}

