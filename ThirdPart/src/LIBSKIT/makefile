# Lance la configuration et la compilation de SKIT
skit=$(TRUST_ROOT)/lib/libskit.a
all:$(skit)
$(skit):  Installer makefile
	make clean
	./Installer ${MAKE} 

clean:
	rm -r -f SPARSKIT2 $(skit)
