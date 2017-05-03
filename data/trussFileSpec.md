# Truss structure file specification for PARTruss project
This file format uses the JSON schema for organization of keys and values
Some inspiration was taken from this document for the Abaqus commercial FME solving tool:
http://imechanica.org/files/1-2%20Learning%20ABAQUS.pdf

## Structure
The truss structure file format has two arrays:
* Vertices
* Edges

### Vertices
Each of these entries specifies a pin-connection for the truss structure, which is where several edges (members) meet. Each value looks like this:

	{
		"XYZPosition": [0, 0, 0],
		"XYZAppliedForces" : [0, 5.0, 0],
		"Anchored" : [true, false, true]
	},

* The three __float__ values tagged by "XYZPosition" are the __x__, __y__, and __z__ coordinates of the vertex with reference to the global three-axis system used in the the simulation.
* The three __float__ values tagged "XYZAppliedForces" are the forces applied to that point in the __x__, __y__, and __z__ directions.
* The array of __boolean__ values tagged "Anchored" [true | false] defines whether this vertex is immoveable in each coordinate axis direction. For simulations in which deformation of the members is allowed, this means that this vertex will not move along that axis, though others may in response to system strains.

### Edges
Each edge represents a structural connection (_member_) between two vertices. Each value looks like this:

	{
		"Endpoints": [ 0, 1 ],
		"ElasticModulus": 0.0,
		"SectionArea": 0.25,
		"Force" : 0.0,
		"Stress" : 0.0
	},

* "Endpoints" is the key referencing the two __integer__ indices referencing the vertices that the member joins.
* ElasticModulus is a __float__ value representing the Young's Modulus (Modulus of Elasticity) of the material that "Member1" is made of. This is only to be used for deformation (strain) calculations, and is not necessary when simply resolving forces in each member for stress analysis.
* "SectionArea" is the __float__ cross sectional area of the member. This is to be used in stress calculations once the individual forces in each member have been calculated.
* "Force" is a __float__ value that indicates the either compressive (negative) or tensile (positive) force for "Member1". 
** _For input, this can be left null as this value will be calculated. Output files should however have this value populated._
* "Stress" is a __float__ value that is equivalent to the "Force" value over the "SectionArea" value. 
** _For input, this can be left null as this value will be calculated. Output files should however have this value populated._

__Note that where the float type is specified, double should be used in its place where possible to leverage higher precision.__
