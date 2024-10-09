pkg=pdi-1.7.1
lib=$(TRUST_ROOT)/lib/src/LIBPDI/lib64/libpdi.so

all : $(lib)

$(lib) : makefile install
	make clean
	@./install $(pkg)

clean:
	rm -rf $(TRUST_ROOT)/lib/src/LIBPDI 
