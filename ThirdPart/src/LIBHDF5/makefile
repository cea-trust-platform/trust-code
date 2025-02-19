ext_pkg=$(TRUST_ROOT)/externalpackages/MED
mpi=$(MPI_INCLUDE)/mpi.h
HDF5_ARCHIVE=$(ext_pkg)/hdf5-1.14.2.tar.gz

HDF5=$(TRUST_HDF5_ROOT)/lib/libhdf5.a

install_dir=$(TRUST_HDF5_ROOT)

all : $(HDF5) 

$(HDF5) : $(HDF5_ARCHIVE) makefile install_hdf5.sh  $(mpi)
	make clean_hdf
	@rm -f `readlink -f $(HDF5)`
	@./install_hdf5.sh $(HDF5_ARCHIVE)
	
 
hdf5 : $(HDF5) 

clean_hdf:
	rm -rf $(TRUST_ROOT)/build/lib/hdf5*

clean :
	make clean_hdf
	rm -rf $(install_dir)
