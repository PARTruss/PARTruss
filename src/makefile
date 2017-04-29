CXX = clang
CXXLINKER = g++
CXXFLAGS = --std=c++11

Truss.o: Truss.cpp Truss.hpp
	$(CXX) $(CXXFLAGS) -c Truss.cpp Truss.hpp
	
Node.o: Node.cpp Node.hpp
	$(CXX) $(CXXFLAGS) -c Node.cpp Node.hpp

Element.o: Element.cpp Element.hpp
	$(CXX) $(CXXFLAGS) -c Element.cpp Element.hpp
	
main.o: main.cpp json.hpp
	$(CXX) $(CXXFLAGS) -c main.cpp

main: main.o Truss.o Node.o Element.o
	$(CXXLINKER) $(CXXFLAGS) -o main Truss.o Node.o Element.o main.o
