CXXFLAGS = -g -I../../include -I../include 


propTester.ofx : propTester.o
	g++ -bundle -all_load -exported_symbols_list ../ofxSymbols -o propTester.ofx propTester.o  ../Library/libOfxSupport.a
