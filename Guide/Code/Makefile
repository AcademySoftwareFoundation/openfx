SUBDIRS = Basics Invert Gain

.PHONY : default clean

default : 
	@ echo making sub projects... $(SUBDIRS)
	for i in $(SUBDIRS) ; do cd $$i; make; make install; cd ..; done

clean :
	for i in $(SUBDIRS) ; do cd $$i; make clean; cd ..; done
	rm -rf built_plugins
