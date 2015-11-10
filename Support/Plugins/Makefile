SUBDIRS = Basic Field Generator Invert MultiBundle Retimer Tester Transition

all: subdirs

.PHONY: subdirs clean $(SUBDIRS)

subdirs: $(SUBDIRS)

$(SUBDIRS):
	(cd $@ && $(MAKE))

clean :
	@for i in $(SUBDIRS) ; do \
	  echo "(cd $$i && $(MAKE) $@)"; \
	  (cd $$i && $(MAKE) $@); \
	done
