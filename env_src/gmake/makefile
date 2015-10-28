name=gmake
version=4.0
package=make-$(version).tar.gz
install=$(TRUST_ROOT)/exec/$(name)
cible==$(install)/bin/$(name)
all:$(cible)
$(cible): makefile
	make clean
	@./Installer $(version)

clean:
	rm -r -f $(install)
