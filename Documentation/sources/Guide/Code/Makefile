SUBDIRS = Example1 Example2 Example3 Example4 Example5

OPTIMISE = -g

.PHONY : default clean

default :
	@ echo making sub projects... $(SUBDIRS)
	for i in $(SUBDIRS) ; do $(MAKE) -C $$i install; done

clean :
	for i in $(SUBDIRS) ; do $(MAKE) -C $$i clean; done
	rm -rf built_plugins
