# Lance la configuration et la compilation de Lapack/Blas
#old=$(TRUST_LIB)/liblapack.a LAPACK
lib1=$(TRUST_LIB)/src/LIBLAPACK/libblas.a 
lib2=$(TRUST_LIB)/src/LIBLAPACK/liblapack.a

all: $(lib2) 
$(lib2): Installer makefile
	@make clean
	./Installer

clean:
	rm -r -f *.a *.log  xianyi-OpenBLAS*  $(lib1) $(lib2) lapack-*.tgz
	# Il sera necessaire de recompiler PETSc
	#if [ "$(TRUST_DISABLE_PETSC)" != 1 ]; then \
	#cd ../LIBPETSC;make clean;fi
