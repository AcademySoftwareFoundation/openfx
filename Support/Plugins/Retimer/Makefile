CXXFLAGS = -g -I../../../include -I../../include 

retimer.ofx : retimer.o ../../Library/libOfxSupport.a
	g++ -bundle -all_load -exported_symbols_list ../../include/osxSymbols -o retimer.ofx retimer.o  ../../Library/libOfxSupport.a  -framework opengl
	mkdir -p  retimer.ofx.bundle/Contents/MacOS
	cp  retimer.ofx  retimer.ofx.bundle/Contents/MacOS
	cp  Info.plist  retimer.ofx.bundle/Contents/



