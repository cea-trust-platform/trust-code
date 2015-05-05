# Lance la configuration et la compilation de valgrind
valgrind=$(TRUST_ROOT)/exec/valgrind/bin/valgrind
version=3.9.0
all:$(valgrind)
$(valgrind): Installer_valgrind $(TRUST_cc) makefile
	make clean
	./Installer_valgrind $(version)
		
clean:
	rm -rf $(TRUST_ROOT)/exec/valgrind $(valgrind) valgrind-?.?.?

