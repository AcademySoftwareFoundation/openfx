SUBDIRS = Basic Custom DepthConverter Invert OpenGL Overlay Rectangle Test


default : 
	@ echo making sub projects... $(SUBDIRS)
	for i in $(SUBDIRS) ; do cd $$i; make -f MakefileOSX; cd ..; done
