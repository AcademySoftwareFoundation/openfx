CXXFLAGS = -g -I../../../include -I../../include 

basic.ofx : basic.o ../../Library/libOfxSupport.a
	g++ -bundle -all_load -exported_symbols_list ../../include/osxSymbols -o basic.ofx basic.o  ../../Library/libOfxSupport.a  -framework opengl
	mkdir -p  basic.ofx.bundle/Contents/MacOS
	cp  basic.ofx  basic.ofx.bundle/Contents/MacOS
	cp  Info.plist  basic.ofx.bundle/Contents/

clean :
	rm -rf basic.ofx basic.ofx.bundle basic.o

