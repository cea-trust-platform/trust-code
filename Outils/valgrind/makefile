name=valgrind
version=3.10.1
install=$(TRUST_ROOT)/exec/$(name)
cible=$(install)/bin/$(name)
all:$(cible)
$(cible): Installer_valgrind $(TRUST_cc) makefile
	make clean
	@./Installer_valgrind $(version)
		
clean:
	rm -r -f $(install) $(name)-*.*.*
