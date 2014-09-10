VERSION 	= 0.1
PACKAGE 	= cdua-sources-$(VERSION)
SUBDIRS 	= data-gen demo file hw mail probe sh-aux sys text tty utils
SUBDIR_MAKEF	= $(patsubst %,%/Makefile,$(SUBDIRS))
DIST_MAKEF	= Mk-dist

BIN = $(filter $(HOME)/bin/%, $(subst :, ,$(PATH)))
ifeq ($(BIN),)
default:
        @echo "error: can't find home-bin directory"
endif

all clean: 
	for i in $(SUBDIRS); do ( cd $$i && $(MAKE) $@ ); done

list install: $(DIST_MAKEF)
	$(MAKE) -f $(DIST_MAKEF) $@

$(DIST_MAKEF): Makefile $(SUBDIR_MAKEF)
	for i in $(SUBDIRS); do \
		( cd $$i && $(MAKE) D=$$i build_mk >>/tmp/smk.wyz ); \
	done
	sed 's/^\([^:][^:]*\):[^;][^;]*;.*CC.*\$\^\(.*\)$$/zz \1 \2/; t; d' \
	  $(SUBDIR_MAKEF) >>/tmp/smk.wyz
	awk '/^xx /{ x[n++] = $$2 } \
	     /^zz /{ for(i=3;i<=NF;i++) z[$$2] = z[$$2] " " $$i } \
	END { \
	  printf "include Common.mk\n\n"; \
	  printf "list: \n\t@echo %ctype make install to install: %c\n",39,39; \
	  for (i=0;i<n;i++) printf "\t@echo %c  %s%c\n",39,x[i],39; \
	  print ""; \
	  printf "install: "; \
	  for (i=0;i<n;i++) { \
	    p=index(x[i],"/"); if (p==0) continue; \
	    s=substr(x[i],p+1); \
	    printf "$(BIN)/%s ",s; \
	  } print "\n"; \
	  for (i=0;i<n;i++) { \
	    p=index(x[i],"/"); if (p==0) continue; \
	    s=substr(x[i],p+1); \
	    printf "$(BIN)/%s: %s.c\n\t$$(CC) $$(XFLAGS) -o $$@ %s.c%s\n\n",\
		    s,x[i],x[i],z[s]; \
	  } \
	}' /tmp/smk.wyz > $(DIST_MAKEF)
	rm -f /tmp/smk.wyz

###

dist: $(PACKAGE).tar.gz

$(PACKAGE).tar.gz : $(PACKAGE).tar 
	gzip $(PACKAGE).tar

$(PACKAGE).tar : 
	rm -f $@
	find . ! -type d -print | sed \
	  -e '/\/RCS\/.*,v$$/d'	\
	  -e '/\/RCS$$/d' \
	  -e 's/^./$(PACKAGE)/' | sort > /tmp/@$$.=
	pwd | (cd .. && ln -s `cat` $(PACKAGE) && \
	  tar cf $(PACKAGE).tar `cat /tmp/@$$.=` && \
	  mv $(PACKAGE).tar $(PACKAGE) && \
	  cd $(PACKAGE) && \
	  rm ../$(PACKAGE) /tmp/@$$.= )
