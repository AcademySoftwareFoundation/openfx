CXXFLAGS = -I../../include
OPTIMIZER = -O3

invert.dso : invert.o
	$(CXX) -shared invert.o -o invert.dso
	strip -fhls invert.dso

