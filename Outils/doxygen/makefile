name=doxygen
install=$(TRUST_ROOT)/exec/$(name)
cible=$(install)/bin/$(name)
all:$(cible)
$(cible): makefile install.sh
	make clean
	@./install.sh

clean:
	rm -r -f $(TRUST_ROOT)/exec/doxygen*
