PETSC_ROOT=$(TRUST_ROOT)/lib/src/LIBPETSC/petsc
petsc=$(PETSC_ROOT)/$(TRUST_ARCH)_opt/lib/libpetsc.a
mpi=$(MPI_INCLUDE)/mpi.h
path_include=$(PETSC_ROOT)/$(TRUST_ARCH)

all:$(petsc)

$(petsc): install makefile $(mpi)
	make clean;
	./install
	./install check

check:
	./install check

clean:
	rm -r -f $(PETSC_ROOT)/../build $(PETSC_ROOT)
