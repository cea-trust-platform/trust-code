include $(TRUST_ENV)/make.$(TRUST_ARCH_CC)$(OPT)
CC=$(TRUST_CC)
name=gmsh
install=$(TRUST_ROOT)/exec/$(name)
build=$(TRUST_ROOT)/build/Outils/Gmsh
cible= $(install)/bin/$(name)
all : $(cible)

$(cible) : Makefile install_gmsh Demo_Gmsh.sh $(TRUST_MED_ROOT)/lib/libmed.a
	make clean
	@./install_gmsh #1>$(install).log 2>&1
	make demo #1>>$(install).log 2>&1

demo:
	./Demo_Gmsh.sh

check:
	./install_gmsh -check

clean:
	rm -f *.o 
	rm -r -f $(install)* fltk gmsh fltk*/ gmsh*/ $(build)
	rm -f *med *log *ssz*geo carre*file *gz *.stop
