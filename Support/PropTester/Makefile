CXXFLAGS = -g -I../../include -I../include 


propTester.ofx : propTester.o ../Library/libOfxSupport.a
	g++ -bundle -all_load -exported_symbols_list ../include/osxSymbols -o propTester.ofx propTester.o  ../Library/libOfxSupport.a
	cp propTester.ofx propTester.ofx.bundle/Contents/MacOS

