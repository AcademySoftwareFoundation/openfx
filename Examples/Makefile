SUBDIRS = Basic Custom DepthConverter Invert OpenGL Overlay Rectangle Test

default: 
	@ echo making sub projects... $(SUBDIRS)
	for i in $(SUBDIRS) ; do cd $$i; make ; cd ..; done

clean:
	for i in $(SUBDIRS) ; do cd $$i; make clean; cd ..; done

release:
	for i in $(SUBDIRS) ; do cd $$i; make release; cd ..; done
