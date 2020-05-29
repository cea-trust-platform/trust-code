petsc=$(PETSC_ROOT)/$(TRUST_ARCH)_opt/lib/libpetsc.a
package_petsc=petsc-3.12.2
mpi=$(MPI_INCLUDE)/mpi.h
path_include=$(PETSC_ROOT)/$(TRUST_ARCH)

all:$(petsc) check

$(petsc): install makefile $(mpi)
	make clean;
	./install $(package_petsc).tar.gz

check:
	./install check

clean:
	rm -r -f $(PETSC_ROOT)/../build/* $(PETSC_ROOT)/*/lib/*
