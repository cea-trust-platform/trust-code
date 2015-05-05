hwloc=$(TRUST_ROOT)/exec/hwloc/bin/lstopo
all:$(hwloc)
$(hwloc): Installer_hwloc makefile
	make clean
	./Installer_hwloc

clean:
	rm -r -f hwloc hwloc-?.?.?
