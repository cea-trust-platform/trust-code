include $(TRUST_ENV)/make.$(TRUST_ARCH_CC)$(OPT)
CC=$(TRUST_CC)
name=gmsh
install=$(TRUST_ROOT)/exec/$(name)
cible= $(install)/bin/$(name)
all : $(cible)

$(cible) : Makefile install_gmsh Demo_Gmsh.ksh $(TRUST_ROOT)/lib/src/LIBMED/MED/lib/libmed.so
	make clean
	@./install_gmsh #1>$(install).log 2>&1
	make demo #1>>$(install).log 2>&1
demo:
	./Demo_Gmsh.ksh

check:
	./install_gmsh -check

clean:
	rm -f *.o 
	rm -r -f $(install)* fltk gmsh fltk*/ gmsh*/

