name=hwloc
install=$(TRUST_ROOT)/exec/$(name)
cible=$(install)/bin/lstopo
all:$(cible)
$(cible): Installer_hwloc makefile
	make clean
	@./Installer_hwloc

clean:
	rm -r -f $(install) $(name)-*.*.*
