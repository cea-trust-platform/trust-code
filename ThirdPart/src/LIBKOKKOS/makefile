# Kokkos
lib=$(TRUST_LIB)/src/LIBKOKKOS/lib64/libkokkoscore.a

all: $(lib) 
$(lib): install_kokkos.sh makefile
	@make clean
	sh ./install_kokkos.sh

clean:
	rm -rf $(TRUST_ROOT)/build/kokkos $(TRUST_LIB)/src/LIBKOKKOS
