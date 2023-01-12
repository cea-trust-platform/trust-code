pkg=$(TRUST_ROOT)/externalpackages/osqp-0.6.0.tar.gz
src=$(TRUST_ROOT)/build/lib/osqp-0.6.0
lib=$(TRUST_ROOT)/lib/src/LIBOSQP/include/osqp/osqp_configure.h

TRUST_INT64_="OFF"
ifeq ($(TRUST_INT64),1)
        TRUST_INT64_="ON"
endif
ifdef TRUST_cc_BASE_EXTP
    cc=$(TRUST_cc_BASE_EXTP)
    CC=$(TRUST_CC_BASE_EXTP)
else
    cc=$(TRUST_cc_BASE)
    CC=$(TRUST_CC_BASE)
endif

all: $(lib)

$(lib): makefile $(pkg)
	(mkdir -p $(TRUST_ROOT)/build/lib;cd $(TRUST_ROOT)/build/lib; tar xzf $(pkg))
	(cd $(src); cmake -DCMAKE_INSTALL_PREFIX=$(TRUST_ROOT)/lib/src/LIBOSQP -DCMAKE_C_COMPILER=$(cc) -DCMAKE_C_FLAGS=-fPIE -DCMAKE_CXX_COMPILER=$(CC) -DCMAKE_C_FLAGS=-fPIC -DUNITTESTS=OFF -DPRINTING=OFF -DPROFILING=OFF -DDLONG=$(TRUST_INT64_) -DCMAKE_BUILD_TYPE=Release)
	(cd $(src); $(TRUST_MAKE) install && rm -f $(TRUST_ROOT)/lib/src/LIBOSQP/lib*/libosqp.{so,dylib} && rm -r -f $(src))

clean:
	rm -rf $(src) $(TRUST_ROOT)/lib/src/LIBOSQP
