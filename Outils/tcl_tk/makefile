name=tcl_tk
old_version=8.4.14
version=8.5.13
install=$(TRUST_ROOT)/exec/$(name)
cible=$(install)/bin/wish
all:$(cible)
$(cible):  install_tk
	make clean
	@./install_tk $(TRUST_ROOT) $(version)
	
clean:
	rm -r -f $(install) tcl$(old_version) tk$(old_version) tcl$(version) tk$(version)
