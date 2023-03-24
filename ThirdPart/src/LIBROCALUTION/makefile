mpi=$(MPI_INCLUDE)/mpi.h
lib=$(TRUST_ROCALUTION)/include/rocalution/rocalution.hpp

all:$(lib)

$(lib): install makefile $(mpi) $(lib)
	make clean
	./install
	
clean:
	rm -r -f ../../../lib/src/LIBROCALUTION
