AMGX_DIR=$(TRUST_ROOT)/lib/src/LIBAMGX/AmgX
AMGX_LIB=$(AMGX_DIR)/lib/libamgx.a
AMGXWRAPPER_DIR=$(TRUST_ROOT)/lib/src/LIBAMGX/AmgXWrapper
AMGXWRAPPER_LIB=$(AMGXWRAPPER_DIR)/lib64/libAmgXWrapper.so
mpi=$(MPI_INCLUDE)/mpi.h

all:$(AMGX_LIB) $(AMGXWRAPPER_LIB)

# ToDo rajouter install et makefile comme dependances
$(AMGX_LIB): $(mpi)
	./install $(AMGX_DIR)

$(AMGXWRAPPER_LIB): $(mpi)
	./install $(AMGXWRAPPER_DIR)
	
clean:
	rm -r -f $(AMGX_DIR)/../*
