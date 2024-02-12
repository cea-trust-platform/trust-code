SRC=$(TRUST_ROOT)/ThirdPart/src/LIBMFFT_C/src
LIB=$(TRUST_ROOT)/lib/libmfft_c.a

all: $(LIB)

$(LIB): makefile install.sh $(SRC)/*
	./install.sh

clean:
	rm -rf $(TRUST_ROOT)/build/lib/LIBMFFT_C
	rm -rf $(LIB)

.PHONY: clean

