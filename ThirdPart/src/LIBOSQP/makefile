pkg=$(TRUST_ROOT)/externalpackages/osqp.tar.gz
src=$(TRUST_ROOT)/build/lib/osqp
lib=$(TRUST_ROOT)/lib/src/LIBOSQP/lib64/libosqp.a

all: $(lib)

$(lib): makefile $(pkg)
	(mkdir -p $(TRUST_ROOT)/build/lib;cd $(TRUST_ROOT)/build/lib; tar xzf $(pkg))
	(cd $(src); cmake -DCMAKE_INSTALL_PREFIX=$(TRUST_ROOT)/lib/src/LIBOSQP -DCMAKE_C_COMPILER=$(TRUST_cc) -DCMAKE_CXX_COMPILER=$(TRUST_CC) -DUNITTESTS=OFF -DPRINTING=OFF -DDLONG=OFF -DCMAKE_BUILD_TYPE=Release)
	(cd $(src); $(TRUST_MAKE) install; rm -f $(TRUST_ROOT)/lib/src/LIBOSQP/lib/libosqp.{so,dylib}; rm -r -f $(src))

clean:
	rm -rf $(src) $(TRUST_ROOT)/lib/src/LIBOSQP
