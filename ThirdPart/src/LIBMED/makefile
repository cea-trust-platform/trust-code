ext_pkg=$(TRUST_ROOT)/externalpackages/MED

HDF5_ARCHIVE=$(ext_pkg)/hdf5-1.10.3.tar.gz
MED_ARCHIVE=$(ext_pkg)/med-4.1.1.tar.gz

MED=$(TRUST_MED_ROOT)/lib/libmed.a
HDF5=$(TRUST_MED_ROOT)/lib/libhdf5.a

install_dir=$(TRUST_MED_ROOT)

all : $(HDF5) $(MED)

$(MED) : $(HDF5) $(MED_ARCHIVE) makefile install_med.sh
	make clean_med
	@rm -f `readlink -f $(MED)`
	@./install_med.sh $(MED_ARCHIVE)

$(HDF5) : $(HDF5_ARCHIVE) makefile install_hdf5.sh
	make clean_hdf
	@rm -f `readlink -f $(HDF5)`
	@./install_hdf5.sh $(HDF5_ARCHIVE)
	
	
 
hdf5 : $(HDF5) 

med : $(HDF5) $(MED)

clean_hdf:
	rm -rf $(TRUST_ROOT)/build/lib/hdf5*
clean_med:
	rm -rf $(TRUST_ROOT)/build/lib/med*
clean :
	make clean_hdf
	make clean_med
	rm -rf $(install_dir)
