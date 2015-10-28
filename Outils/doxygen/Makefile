version=1.7.4
package=doxygen-$(version).linux.bin.tar.gz
install=$(TRUST_ROOT)/Outils/doxygen/doxygen-$(version)
cible=$(install)/bin/doxygen

all:$(cible)
$(cible): Makefile
	@make clean
	@tar xfz $(TRUST_ROOT)/externalpackages/$(package)
	@touch $(cible)
	
clean:
	rm -r -f doxygen-?.?.? doxygen-?.?.?.?
