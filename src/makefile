CXX = nvcc
CXXLINKER = nvcc
CXXFLAGS = -g -arch=compute_35 -std=c++11 -Xcompiler -fopenmp
LDFLAGS = -lcublas -lcusolver -arch=compute_35 -lgomp

main: main.o Truss.o Node.o Element.o
	$(CXXLINKER) $(LDFLAGS) -o main Truss.o Node.o Element.o main.o solveMatrix.o

Truss.o: Truss.cpp Truss.hpp cusolver/solveMatrix.cu
	$(CXX) $(CXXFLAGS) -c Truss.cpp cusolver/solveMatrix.cu
	
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
