# Generate 3D truss files in json to be viewed and solved

import sys
import json

if len(sys.argv) < 3:
	print("USAGE: {} truss_repetitions output_filename.json".format(sys.argv[0]))
        sys.exit(1)
# Number of adjacent truss sections that will be created and connected
num_repetitions = int(sys.argv[1])

SectionArea = 0.25
ElasticModulus = 25000000000

# Base 2d template truss structure, which can be repeated and connected adjacently
locations = [ [0.0, 0.0, 0.0], [3.0, 2.0, 0.0], [6.0, 4.0, 0.0], [9.0, 2.0, 0.0], [12.0, 0.0, 0.0], [8.0, 0.0, 0.0], [4.0, 0.0, 0.0] ]
connections = [ [0, 1], [1, 2], [2, 3], [3, 4], [4, 5], [5, 6], [6 0], [1 6], [2, 6], [2, 5], [3, 5] ]
#inter_layer_connections [  ]
json_to_dump = { 'Vertices':[], 'Edges':[] }

for i in range(num_repetitions):
	for loc in locations:
		loc[2] += i
		vertex = { 'XYZPosition':loc, 'XYZAppliedForces':[0.0,0.0,0.0], 'Anchored':[0.0,0.0,0.0] }
		json_to_dump['Vertices'].append(vertex)

	for conn in connections:
		offset = len(locations) * i
		conn = [x+offset for x in conn]
		edge = { 'Endpoints':conn, 'ElasticModulus':ElasticModulus, 'SectionArea':SectionArea, 'Force':0.0, 'Stress':0.0 }
		json_to_dump['Edges'].append(edge)

json_str = json.dumps(json_to_dump, indent=4, sort_keys=True)
with open(sys.argv[2], 'w') as f:
	f.write(json_str)
	f.close()

# Tesselate 2D triangles?:
