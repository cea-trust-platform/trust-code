name=cmake
version=3.0.1
package=$(name)-$(version).tar.gz
install=$(TRUST_ROOT)/exec/$(name)
cible=$(install)/bin/$(name)
all:$(cible)
$(cible): install makefile
	make clean
	@./install $(package) $(install)
	
clean:
	rm -r -f $(install) cmake-2.8.? cmake-3.0.1
