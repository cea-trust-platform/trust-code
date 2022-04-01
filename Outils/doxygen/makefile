name=doxygen
version=1.9.3
install=$(TRUST_ROOT)/exec/$(name)-$(version)
cible=$(install)/bin/$(name)
all:$(cible)
$(cible): makefile install.sh
	make clean
	@./install.sh #tar xfz $(TRUST_ROOT)/externalpackages/$(package) -C $(TRUST_ROOT)/exec
	#@touch $(cible)

clean:
	rm -r -f $(install)
