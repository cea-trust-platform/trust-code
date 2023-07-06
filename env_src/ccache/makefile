name=ccache
version=3.1.4
package=$(name)-$(version).tar.gz
binary=ccache-4.8.2-$(TRUST_ARCH)-x86_64.tar.gz
install=$(TRUST_ROOT)/exec/$(name)
cible=$(install)/bin/$(name)
all:$(cible)
$(cible): $(TRUST_ROOT)/externalpackages/ccache/$(package) $(TRUST_ROOT)/externalpackages/ccache/$(binary) makefile Installer_ccache
	make clean
	@./Installer_ccache

clean:
	rm -r -f $(install)
