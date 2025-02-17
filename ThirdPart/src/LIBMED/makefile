ext_pkg=$(TRUST_ROOT)/externalpackages/MED
mpi=$(MPI_INCLUDE)/mpi.h
MED_ARCHIVE=$(ext_pkg)/med-4.1.1.tar.gz

MED=$(TRUST_MED_ROOT)/lib/libmed.a
HDF5=$(TRUST_HDF5_ROOT)/lib/libhdf5.a

install_dir=$(TRUST_MED_ROOT)

all : $(MED)

$(MED) : $(HDF5) $(MED_ARCHIVE) makefile install_med.sh $(mpi)
	make clean_med
	@rm -f `readlink -f $(MED)`
	@./install_med.sh $(MED_ARCHIVE)

med : $(HDF5) $(MED)

clean_med:
	rm -rf $(TRUST_ROOT)/build/lib/med*
clean :
	make clean_med
	rm -rf $(install_dir)
