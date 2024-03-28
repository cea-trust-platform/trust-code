pkg=$(TRUST_ROOT)/externalpackages/CGNS/CGNS.tar.gz
lib=$(TRUST_ROOT)/lib/src/LIBCGNS/lib/libcgns.a

all : $(lib)

$(lib) : makefile $(pkg) install
	make clean
	@./install

clean:
	rm -rf $(TRUST_ROOT)/lib/src/LIBCGNS $(TRUST_ROOT)/include/CGNS $(TRUST_ROOT)/exec/cgns*
