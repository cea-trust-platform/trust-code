name=astyle
install=$(TRUST_ROOT)/exec/$(name)
cible=$(install)/bin/$(name)
all : $(cible)
$(cible) : makefile install
	make clean
	@./install $(install)
	
clean:
	rm -r -f $(install)
