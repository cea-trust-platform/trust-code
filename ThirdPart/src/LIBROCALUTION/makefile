ROCALUTION_INC=${TRUST_ROCALUTION}/include/rocalution_for_kernel.h
mpi=$(MPI_INCLUDE)/mpi.h

all:$(ROCALUTION_INC)

$(ROCALUTION_INC): install makefile $(mpi)
	make clean
	./install
	
clean:
	rm -r -f $(TRUST_ROCALUTION)/*
