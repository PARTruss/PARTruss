// trussNode class

class trussNode
{
public:
	trussNode(const &std::string label, double x, double y, double z, 
		bool moveX, bool moveY, bool moveZ)
	: _label(label), _coordX(x), _coordY(y), _coordZ(z), _moveX(moveX), _moveY(moveY), _moveZ(moveZ)
	{
		_neighbors = std::vector();
		_sections = std::vector();
		_E = std::vector();
	}
	void addNeighbor(const &trussNode adj)
	{
		_neighbors.push_back(adj)
	}
	
};