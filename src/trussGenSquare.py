# Generate 3D truss files in json to be viewed and solved

import sys
import json
import random

random.seed()
# Maximum force allowabe:
MAX_FORCE_X = 500
MAX_FORCE_Y = 500
MAX_FORCE_Z = 500

if len(sys.argv) < 3:
	print("USAGE: {} truss_repetitions output_filename.json".format(sys.argv[0]))
        sys.exit(1)
# Number of adjacent truss sections that will be created and connected
num_repetitions = int(sys.argv[1])

SectionArea = 0.25
ElasticModulus = 25000000000

# Base 2d template truss structure, which can be repeated and connected adjacently
locations = [ [0.0, 0.0, 0.0], [3.0, 2.0, 0.0], [6.0, 4.0, 0.0], [9.0, 2.0, 0.0], [12.0, 0.0, 0.0], [8.0, 0.0, 0.0], [4.0, 0.0, 0.0] ]
delta = [sum([ x[0] for x in locations])/len(locations), \
	 sum([ x[1] for x in locations])/len(locations), \
	 sum([ x[0] for x in locations])/len(locations) ]
locations = [[i[0]-delta[0],i[1]-delta[1],i[2]-delta[2]] for i in locations]
print locations
connections = [ [0, 1], [1, 2], [2, 3], [3, 4], [4, 5], [5, 6], [6, 0], [1, 6], [2, 6], [2, 5], [3, 5] ]

#inter_layer_connections [ [], [], [], [] ]
json_to_dump = { 'Vertices':[], 'Edges':[] }

for i in range(0,num_repetitions):
	for idx, loc in enumerate(locations):
		loc2 = loc[:]
		loc2[2] = i - (num_repetitions/2.0)
                if idx == 0 or idx == 4:
                    anchored = [True, True, True]
                else:
                    anchored = [False, False, False]
                if idx == 1 or idx == 2 or idx ==3:
                    AppForce = [ MAX_FORCE_X * random.uniform(-1, 1), MAX_FORCE_Y * random.uniform(-1, 1), MAX_FORCE_Z * random.uniform(-1, 1) ]
                else:
                    AppForce = [ 0.0, 0.0, 0.0 ]
		vertex = { 'XYZPosition':loc2, 'XYZAppliedForces':AppForce, 'Anchored':anchored }
		json_to_dump['Vertices'].append(vertex)

	for conn in connections:
		offset = len(locations) * i
		conn2 = [x+offset for x in conn]
		edge = { 'Endpoints':conn2, 'ElasticModulus':ElasticModulus, 'SectionArea':SectionArea, 'Force':0.0, 'Stress':0.0 }
		json_to_dump['Edges'].append(edge)

        if i > 0:
            for idx in range(len(locations)):
                conn2 = [idx + (i-1)*len(locations), idx + i*len(locations)]
                edge2 = { 'Endpoints':conn2, 'ElasticModulus':ElasticModulus, 'SectionArea':SectionArea, 'Force':0.0, 'Stress':0.0 }
                json_to_dump['Edges'].append(edge2)
                

json_str = json.dumps(json_to_dump, indent=4, sort_keys=True)
with open(sys.argv[2], 'w') as f:
	f.write(json_str)
	f.close()

# Tesselate 2D triangles?:
