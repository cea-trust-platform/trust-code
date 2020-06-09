cibles=$(TRUST_ROOT)/exec/CURVEPLOT $(TRUST_ROOT)/exec/TRUST_PLOT2D

all: $(cibles)

$(cibles) : Makefile install.sh TRUST_PLOT2D/src/trust_plot2d/TrustFiles.py
	make clean
	./install.sh

clean:
	./clean.sh
