cibles=$(TRUST_ROOT)/exec/CURVEPLOT $(TRUST_ROOT)/exec/TRUST_PLOT2D
tools_src=TRUST_PLOT2D/src
tools_ressources=TRUST_PLOT2D/resources
SRCS=$(wildcard $(tools_src)/*.py $(tools_src)/trust_plot2d/*.py $(tools_src)/test/*.py $(tools_ressources)/*.* TRUST_PLOT2D/*.* )

all: $(cibles)

$(cibles) : Makefile install.sh $(SRCS)
	make clean
	./install.sh

clean:
	./clean.sh
