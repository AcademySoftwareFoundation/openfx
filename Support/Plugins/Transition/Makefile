CXXFLAGS = -g -I../../../include -I../../include 

crossFade.ofx : crossFade.o ../../Library/libOfxSupport.a
	g++ -bundle -all_load -exported_symbols_list ../../include/osxSymbols -o crossFade.ofx crossFade.o  ../../Library/libOfxSupport.a  -framework opengl
	mkdir -p  crossFade.ofx.bundle/Contents/MacOS
	cp  crossFade.ofx  crossFade.ofx.bundle/Contents/MacOS
	cp  Info.plist  crossFade.ofx.bundle/Contents/

clean :
	rm -rf crossFade.ofx crossFade.ofx.bundle crossFade.o


