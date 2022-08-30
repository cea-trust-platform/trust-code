ROCALUTION_LIB=$(TRUST_ROCALUTION)/lib/librocalution.so
mpi=$(MPI_INCLUDE)/mpi.h

all:$(ROCALUTION_LIB)

$(ROCALUTION_LIB): install makefile $(mpi)
	make clean
	./install
	
clean:
	rm -r -f $(TRUST_ROCALUTION)/*
