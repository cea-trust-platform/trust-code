# Lance la configuration et la compilation de Lapack/Blas
lib1=$(TRUST_LIB)/src/LIBLAPACK/libblas.a 
lib2=$(TRUST_LIB)/src/LIBLAPACK/liblapack.a

all: $(lib2) 
$(lib2): Installer makefile
	@make clean
	./Installer

clean:
	rm -r -f *.a OpenBLAS* $(TRUST_LIB)/src/LIBLAPACK/* lapack-*
	# Il sera necessaire de recompiler PETSc
	if [ "$(TRUST_DISABLE_PETSC)" != 1 ]; then \
	cd ../LIBPETSC;make clean;fi
