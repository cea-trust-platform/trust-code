pkg=$(TRUST_ROOT)/externalpackages/CGNS/CGNS.tar.gz
lib=$(TRUST_ROOT)/lib/src/LIBCGNS/lib/libcgns.a

all : $(lib)

$(lib) : makefile $(pkg)
	make clean
	@./install

clean:
	rm -rf $(TRUST_ROOT)/lib/src/LIBCGNS $(TRUST_ROOT)/include/CGNS
