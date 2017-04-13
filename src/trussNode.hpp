#include <vector>
#include <string>
#include <stdio>

class trussNode
{
public:
	trussNode(const &std::string label, double x, double y, double z, 
		bool moveX, bool moveY, bool moveZ);
	void addNeighbor(const &trussNode adj);
	void getPosition();


private:
// Representation:
	std::string _label;
	bool _moveX;
	bool _moveY;
	bool _moveZ;
	double _coordX;
	double _coordY;
	double _coordZ;
	// Vector of neighboring (connected) vertices
	std::vector<trussNode> _neighbors;
	// Vector of member cross sectional areas corresponding to each neighbor
	std::vector<double> _sections;
	// Vector of Young's Moduli for each member
	std::vector<double> _E;
}