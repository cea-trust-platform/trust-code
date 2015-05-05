# Lance la configuration et la compilation de gmake
gmake=$(TRUST_ROOT)/exec/gmake/bin/gmake
version=make-4.0.tar.gz
all:$(gmake)
$(gmake): makefile
	make clean
	./Installer $(version)
clean:
	rm -rf $(TRUST_ROOT)/exec/gmake

