archive_gtest=$(TRUST_ROOT)/externalpackages/utils/googletest-1.15.2.tar.gz 
gtest=$(TRUST_ROOT)/lib/src/LIBGTEST/lib64/libgtest.a

all:$(gtest)

$(gtest) : makefile install.sh $(archive_gtest)
	./install.sh $(archive_gtest)

clean:
	rm -rf $(TRUST_ROOT)/build/lib/gtest* $(TRUST_ROOT)/lib/src/LIBGTEST

