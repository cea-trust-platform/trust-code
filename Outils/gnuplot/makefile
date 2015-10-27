name=gnuplot
version=4.6.6
package=$(name)-$(version).tar.gz
install=$(TRUST_ROOT)/exec/$(name)
cible=$(install)/bin/$(name)
all:$(cible)
$(cible): install_gnuplot makefile
	make clean
	@./install_gnuplot $(package)
	
clean:
	rm -r -f $(install)*
