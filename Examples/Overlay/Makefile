CXXFLAGS = -I../../include 
OPTIMIZER = -O3

overlay.dso : overlay.o
	$(CXX) -shared overlay.o -o overlay.dso
	strip -fhls overlay.dso

