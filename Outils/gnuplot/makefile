package=gnuplot-4.4.2.tar.gz
install=$(TRUST_ROOT)/exec/gnuplot/
gnuplot=$(install)/bin/gnuplot
all:$(gnuplot)
$(gnuplot): install_gnuplot makefile
	make clean
	@./install_gnuplot $(package)
	
clean:
	rm -r -f gd $(install) $(TRUST_ROOT)/exec/gnuplot*
