CXXFLAGS = -g -I../../../include -I../../include 

noise.ofx : noise.o randomGenerator.o ../../Library/libOfxSupport.a
	g++ -bundle -all_load -exported_symbols_list ../../include/osxSymbols -o noise.ofx noise.o randomGenerator.o ../../Library/libOfxSupport.a  -framework opengl
	mkdir -p  noise.ofx.bundle/Contents/MacOS
	cp  noise.ofx  noise.ofx.bundle/Contents/MacOS
	cp  Info.plist  noise.ofx.bundle/Contents/


