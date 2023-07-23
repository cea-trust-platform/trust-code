AMGX_DIR=$(TRUST_ROOT)/lib/src/LIBAMGX/AmgX
AMGX_LIB=$(AMGX_DIR)/lib/libamgxsh.so
AMGXWRAPPER_DIR=$(TRUST_ROOT)/lib/src/LIBAMGX/AmgXWrapper
AMGXWRAPPER_LIB=$(AMGXWRAPPER_DIR)/lib64/libAmgXWrapper.a
mpi=$(MPI_INCLUDE)/mpi.h

all:$(AMGX_LIB) $(AMGXWRAPPER_LIB)

$(AMGX_LIB): install makefile $(mpi)
	rm -r -f $(AMGX_DIR)
	./install $(AMGX_DIR)

$(AMGXWRAPPER_LIB): install makefile $(mpi)
	rm -r -f $(AMGXWRAPPER_DIR)
	./install $(AMGXWRAPPER_DIR)
	
clean:
	rm -r -f $(AMGX_DIR) $(AMGXWRAPPER_DIR)
