CXX = clang
CXXLINKER = g++
CXXFLAGS = --std=c++11

trussNode.o: trussNode.cpp
	$(CXX) $(CXXFLAGS) -c trussNode.cpp trussNode.hpp

single_threaded.o: single_threaded.cpp json.hpp
	$(CXX) $(CXXFLAGS) -c single_threaded.cpp

main: single_threaded.o trussNode.o
	$(CXXLINKER) $(CXXFLAGS) -o main trussNode.o single_threaded.o