AMGX_DIR=$(TRUST_ROOT)/lib/src/LIBAMGX/AmgX
AMGX_LIB=$(AMGX_DIR)/lib/libamgx.so
AMGXWRAPPER_DIR=$(TRUST_ROOT)/lib/src/LIBAMGX/AmgXWrapper
AMGXWRAPPER_LIB=$(AMGXWRAPPER_DIR)/lib64/libAmgXWrapper.so
mpi=$(MPI_INCLUDE)/mpi.h

all:$(AMGX_LIB) $(AMGXWRAPPER_LIB)

$(AMGX_LIB): install makefile $(mpi)
	./install $(AMGX_DIR)

$(AMGXWRAPPER_LIB): install makefile $(mpi)
	./install $(AMGXWRAPPER_DIR)
	
clean:
	rm -r -f $(AMGX_DIR)/../*
