// Class for members in a truss
// NOTE: Only implementing stiffness method for 2D (for now?)!
// Each element object contains reference to the two nodes it connects 
// Each element also contains the cross sectional area, elastic modulus, length, 
class Element
{
  public:
    // Constructor:
    Element();
    
    // Utility:
    bool operator==(Element const & e2);
    
    // Setter:
    void setId(int id) { this->_id = id; }
    
    // Getters:
    Node const * const getStart() const { return this->_startNode; }
    Node const * const getEnd() const { return this->_endNode; }
    double getWeight() const { return this->_weight; }
    int const getId() const { return this->_id; }
  
  //private:
    // Representation:
    Node * _startNode;
    Node * _endNode;
    double _sectionArea
    double _length;
    std::valarray<double> _XYZRatio(3);
    double[36] _localStiffness;  // 6x6 local stiffness matrix (T'kT)
    double _youngModulus;
    double _weight;
    static double _density = 0.284; // Hard-coded density of steel (lbs/in^3)
    static double _yieldStress = 36000;  // Hard coded stress at which material fails (psi)
    int _id = -1;   // Default invalid value
    static int _counter = 1;
};