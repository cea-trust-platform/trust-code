name=python
version=2.7.6
package=Python-$(version).tgz
install=$(TRUST_ROOT)/exec/$(name)
cible=$(install)/bin/$(name)
all:$(cible)
$(cible): install makefile
	make clean
	@./install $(package) $(install)
	
clean:
	rm -r -f $(install) Python*gz
