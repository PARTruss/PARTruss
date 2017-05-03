CXX = nvcc
CXXLINKER = nvcc
CXXFLAGS = -arch=compute_35 -std=c++11 -lcublas -lcusolver

main: main.o Truss.o Node.o Element.o
	$(CXXLINKER) $(CXXFLAGS) -o main Truss.o Node.o Element.o main.o solveMatrix.o

Truss.o: Truss.cpp Truss.hpp cusolver/solveMatrix.cu
	$(CXX) $(CXXFLAGS) -c Truss.cpp Truss.hpp cusolver/solveMatrix.cu
	
Node.o: Node.cpp Node.hpp
	$(CXX) $(CXXFLAGS) -c Node.cpp Node.hpp

Element.o: Element.cpp Element.hpp
	$(CXX) $(CXXFLAGS) -c Element.cpp Element.hpp
	
main.o: main.cpp json.hpp
	$(CXX) $(CXXFLAGS) -c main.cpp
