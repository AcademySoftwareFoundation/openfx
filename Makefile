SUBDIRS = HostSupport Examples Support/Plugins Support/PropTester Guide/Code

all: subdirs

.PHONY: subdirs clean $(SUBDIRS)

subdirs: $(SUBDIRS)

$(SUBDIRS):
	$(MAKE) $(MFLAGS) -C $@

clean :
	for i in $(SUBDIRS) ; do \
	  $(MAKE) $(MFLAGS) -C $$i clean; \
	done
