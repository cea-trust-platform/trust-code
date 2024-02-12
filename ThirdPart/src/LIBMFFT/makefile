SRC=$(TRUST_ROOT)/ThirdPart/src/LIBMFFT/src
LIB=$(TRUST_ROOT)/lib/libmfft.a

all: $(LIB)

$(LIB): makefile install.sh $(SRC)/*
	./install.sh

clean:
	rm -rf $(TRUST_ROOT)/build/lib/LIBMFFT
	rm -rf $(LIB)

.PHONY: clean

