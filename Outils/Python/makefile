package=Python-2.7.6.tgz
install=$(TRUST_ROOT)/exec/python
cible=$(install)/bin/python
all:$(cible)
$(cible): install makefile
	make clean
	@./install $(package) $(install)
	
clean:
	rm -r -f $(install)
