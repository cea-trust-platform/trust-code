petsc=$(PETSC_ROOT) $(PETSC_ROOT)/$(TRUST_ARCH)_opt/lib/libpetsc.a
# petsc name changed from petsc-X.Y-pZ to petsc-X.Y.Z 
package_petsc=petsc-3.9.3
mpi=$(MPI_INCLUDE)/mpi.h
path_include=$(PETSC_ROOT)/$(TRUST_ARCH)
all:$(petsc) check


$(petsc): install makefile $(mpi)
	make clean; 
	./install $(package_petsc).tar.gz #1>petsc.log 2>&1

check: 
	./install check

clean:
	rm -r -f $(petsc) petsc-3.?-p? petsc-?.?.? petsc-dev cusp thrust tm4 bison flex #petsc.log
	rm -f *.gz *.tgz *bz2 mpi_init.log configure.help configure.log
