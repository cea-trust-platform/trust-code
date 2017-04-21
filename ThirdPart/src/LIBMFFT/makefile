LIB=$(TRUST_ROOT)/lib/libmfft.a
$(LIB): makefile Install.sh
	make clean
	./Install.sh $(LIB)
	make clean_build
all: $(LIB)
clean_build:
	rm -rf exec* 
clean: clean_build
	rm -f $(LIB)
