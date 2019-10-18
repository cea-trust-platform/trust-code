name=gnuplot
version=5.2.7
package=$(name)-$(version).tar.gz
install=$(TRUST_ROOT)/exec/$(name)
cible=$(install)/bin/$(name)
all:$(cible)
$(cible): install_gnuplot makefile
	make clean
	@./install_gnuplot $(package) #1>$(install).log 2>&1
	
clean:
	rm -r -f $(install)* $(TRUST_ROOT)/build/Outils/gnuplot* *.gz
