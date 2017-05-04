NVC = nvcc
CXX = g++
CXXLINKER = nvcc
CXXFLAGS = -g -std=c++11 -fopenmp
NVCFLAGS = -g -std=c++11 -arch=compute_61
LDFLAGS = -lcublas -lcusolver -arch=compute_61 -lgomp

main: main.o Truss.o Node.o Element.o solveMatrix.o
	$(CXXLINKER) $(LDFLAGS) -o main Truss.o Node.o Element.o main.o solveMatrix.o

Truss.o: Truss.cpp Truss.hpp
	$(CXX) $(CXXFLAGS) -c Truss.cpp

solveMatrix.o: cusolver/solveMatrix.cu cusolver/solveMatrix.h
	$(NVC) $(NVCFLAGS) -c cusolver/solveMatrix.cu
	
Node.o: Node.cpp Node.hpp
	$(CXX) $(CXXFLAGS) -c Node.cpp

Element.o: Element.cpp Element.hpp
	$(CXX) $(CXXFLAGS) -c Element.cpp
	
main.o: main.cpp 
	$(CXX) $(CXXFLAGS) -c main.cpp

clean: 
	rm main | true
	rm -r *.o | true
	rm -r *.gch | true
