include $(TRUST_ENV)/make.$(TRUST_ARCH_CC)$(OPT)
CC=$(TRUST_CC)
prefix=$(TRUST_ROOT)/exec/gmsh
PROGRAM= $(prefix)/bin/gmsh
all : $(PROGRAM)

$(PROGRAM) : Makefile install_gmsh Demo_Gmsh.ksh $(TRUST_ROOT)/lib/src/LIBMED/MED/lib/libmed.so
	make clean
	./install_gmsh 2>&1 | tee install_gmsh.log
demo:
	./Demo_Gmsh.ksh

check:
	./install_gmsh -check

clean:
	rm -f *.o 
	rm -r -f $(prefix) fltk gmsh fltk*/ gmsh*/

