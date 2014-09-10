CC	= gcc -Wall -Werror # -pedantic 
LIBS	= 
INCS	= -I../Libs
CFLAGS	= -g -pipe $(LIBS) $(INCS)
XFLAGS	= -O3 -pipe $(LIBS) $(INCS)

PROGS = $(patsubst %.c,%,$(wildcard *.c))

%: %.c
	$(CC) $(CFLAGS) -o $@ $^ 

all: $(PROGS)

build_mk: 
	if test "$(D)" = "" ; then \
	  echo "please, use ancestor makefile for build_mk"; \
	  exit 1; \
	else \
	  for i in / $(patsubst %.c,%,$(TOGO)); do \
	    case $$i in *.c | /) ;; *) echo xx $(D)/$$i ;; esac; \
	  done; true; \
	fi

clean: 
	echo '$(PROGS) ' | fgrep '.c ' >/dev/null || rm -f $(PROGS)
