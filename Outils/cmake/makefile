name=cmake
old_version=2.8
version=3.0.1
package=$(name)-$(version).tar.gz
install=$(TRUST_ROOT)/exec/$(name)
cible=$(install)/bin/$(name)
all:$(cible)
$(cible): install makefile
	make clean
	@./install $(package) $(install)
	
clean:
	rm -r -f $(install) $(name)-$(old_version).? $(name)-$(version)
