archive_gtest=$(TRUST_ROOT)/externalpackages/utils/googletest-1.15.2.tar.gz 
gtest=$(TRUST_ROOT)/lib/src/LIBGTEST/lib/libgtest.a

all:$(gtest)

$(gtest) : makefile install.sh $(archive_gtest)
	make clean
	./install.sh $(archive_gtest)

clean:
	rm -rf $(TRUST_ROOT)/build/lib/gtest* $(TRUST_ROOT)/lib/src/LIBGTEST $(TRUST_ROOT)/exec/unit_gtest $(TRUST_ROOT)/exec/unit_gtest_mpi

