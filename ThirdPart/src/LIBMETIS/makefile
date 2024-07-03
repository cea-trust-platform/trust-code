lib=../../../lib/src/LIBMETIS/lib/libmetis.a
metis=metis-5.1.0-p12
tar=$(metis).tar.gz
all:$(lib)
ifdef TRUST_cc_BASE_EXTP
    cc=$(TRUST_cc_BASE_EXTP)
    CC=$(TRUST_CC_BASE_EXTP)
    CPATH="" # Vu sur JeanZay
else
    cc=$(TRUST_cc_BASE)
    CC=$(TRUST_CC_BASE)
endif

$(lib):  makefile Install
	make clean;
	./Install $(metis);
	
clean:
	rm -r -f $(metis) ../../../lib/src/LIBMETIS/bin ../../../lib/src/LIBMETIS/include ../../../lib/src/LIBMETIS/lib
