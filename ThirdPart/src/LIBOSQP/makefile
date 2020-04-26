# Lance la configuration et la compilation de Lapack/Blas
#old=$(TRUST_LIB)/liblapack.a LAPACK
pkg=$(TRUST_ROOT)/externalpackages/osqp.tar.gz
src=$(TRUST_ROOT)/build/lib/osqp
lib=$(TRUST_ROOT)/lib/src/LIBOSQP/lib/libosqp.a

all: $(lib)

$(lib): $(src)/osqp-config.cmake
	(cd $(src); $(TRUST_MAKE) install; rm -f $(TRUST_ROOT)/lib/src/LIBOSQP/lib/libosqp.{so,dylib})

$(src)/osqp-config.cmake: $(src)/CMakeLists.txt
	(cd $(src); cmake -DCMAKE_INSTALL_PREFIX=$(TRUST_ROOT)/lib/src/LIBOSQP -DCMAKE_C_COMPILER=$(TRUST_cc) -DCMAKE_CXX_COMPILER=$(TRUST_CC) -DUNITTESTS=OFF -DPRINTING=OFF -DDLONG=OFF -DCMAKE_BUILD_TYPE=Release)

$(src)/CMakeLists.txt: $(pkg)
	(cd $(TRUST_ROOT)/build/lib; tar xzf $(pkg)) 

clean:
	rm -rf $(src) $(TRUST_ROOT)/lib/src/LIBOSQP
