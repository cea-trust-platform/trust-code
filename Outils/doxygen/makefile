name=doxygen
version=1.7.4
package=$(name)-$(version).linux.bin.tar.gz
install=$(TRUST_ROOT)/exec/$(name)-$(version)
cible=$(install)/bin/$(name)
all:$(cible)
$(cible): makefile
	make clean
	@tar xfz $(TRUST_ROOT)/externalpackages/$(package) -C $(TRUST_ROOT)/exec
	@touch $(cible)

clean:
	rm -r -f $(install) $(name)-*.*.* $(name)-*.*.*.*
