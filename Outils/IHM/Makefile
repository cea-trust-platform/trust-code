cibles=$(TRUST_ROOT)/exec/CURVEPLOT $(TRUST_ROOT)/exec/TRUST_PLOT2D

all: $(cibles)

$(cibles) : Makefile install.sh
	make clean
	./install.sh

clean:
	./clean.sh
