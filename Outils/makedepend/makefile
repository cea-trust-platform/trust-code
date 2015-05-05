name=makedepend
version=1.0.5
package=$(name)-$(version).tar.bz2
install=$(TRUST_ROOT)/exec/$(name)
cible=$(install)/bin/$(name)
all:$(cible)
$(cible): install makefile
	make clean
	@./install $(package) $(install)
	
clean:
	rm -r -f $(install)
