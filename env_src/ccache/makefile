name=ccache
version=3.1.4
package=$(name)-$(version).tar.gz
install=$(TRUST_ROOT)/exec/$(name)
cible=$(install)/bin/$(name)
all:$(cible)
$(cible): $(TRUST_ROOT)/externalpackages/$(package) makefile Installer_ccache
	make clean
	@./Installer_ccache
clean:
	rm -rf $(install)

