# Lance la configuration et la compilation de ccache
ccache=$(TRUST_ROOT)/exec/ccache/bin/ccache
all:$(ccache)
$(ccache): $(TRUST_ROOT)/externalpackages/ccache-3.1.4.tar.gz makefile Installer_ccache
	make clean
	./Installer_ccache
clean:
	rm -rf $(TRUST_ROOT)/exec/ccache

