# PARTruss
Parallel truss structure simulator. Group project in Applied Parallel Computing for Engineers
## Overall Functionality:
Each straight section of the structure (a truss) can only transmit forces along its axis. These trusses may deform by changing length, though this would be a secondary/stretch goal.
First and foremost, the forces applied to each truss can be solved in parallel and the resulting stress in the member can be shown by color-coding. Over stressed trusses may fail, changing the dynamics of the system.

## Deliverables:
Graphics front-end to display 2D truss structures
C++ backend to parse truss description files (.tds)
Parallel C++ backend to simulate truss structure in response to applied loads
Graphics frontend to color code truss sections with stress in each member
Add internal forces from weight of truss sections and connecting nodes

## Stretch functionalities
Implement and display strain in members (deformation, i.e. stretching)
    C++ backend and OpenGL frontend
Extend the program to parse, represent, and simulate 3D truss structures.
Allow the user to dynamically apply loads to various nodes in the system while it is being simulated
Allow truss material properties to be specified, so that different failing stresses and deformation constants can be used per section.

## Technologies
CUDA/OpenGL
Thrust
C++
Presentation Outline
Finite Element Analysis
Total system can be modeled as many smaller, interconnected points
Simplifies looking at stressors, tensors, and deformations
Can provide analysis for weak points in system
Truss Analysis
Subset of FEA
Subdivide a finite element system into points and edges
Using linear systems, can compute statistics at any point in the system
Will be initially handled in 2D bridges, but may be expanded into 3D and general truss analysis as the scope permits

## References/Resources:
https://www.cs.princeton.edu/courses/archive/fall09/cos323/assign/truss/truss.html
https://en.wikipedia.org/wiki/Finite_element_method

