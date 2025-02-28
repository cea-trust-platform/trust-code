ext_pkg=$(TRUST_ROOT)/externalpackages/PDI
PDI_ARCHIVE=$(ext_pkg)/pdi-1.7.1.tar.gz
PDI=$(TRUST_ROOT)/lib/src/LIBPDI/lib64/libpdi.so
HDF5=$(TRUST_HDF5_ROOT)/lib/libhdf5.a

all : $(PDI)

$(PDI) : $(HDF5) $(PDI_ARCHIVE) makefile install
	make clean
	@./install $(PDI_ARCHIVE)

clean:
	rm -rf $(TRUST_ROOT)/lib/src/LIBPDI 
