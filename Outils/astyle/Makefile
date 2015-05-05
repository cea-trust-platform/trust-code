prefix=$(TRUST_ROOT)/exec/astyle
PROGRAM=$(prefix)/bin/astyle
all : $(PROGRAM)

$(PROGRAM) : Makefile install
	make clean
	./install $(prefix)
	
clean:
	rm -r -f astyle $(prefix)
	
