MED_INSTALL_BUILDER=./scripts/provided/install_MED_builder.sh
HDF_INSTALL_BUILDER=./scripts/provided/install_HDF_builder.sh
 
MED_INSTALLER=./scripts/generated/install_MED.sh
HDF_INSTALLER=./scripts/generated/install_HDF.sh

MED=$(TRUST_MED_ROOT)/lib/libmed.a
HDF=$(TRUST_MED_ROOT)/lib/libhdf5.a

MED_ARCHIVES= ./archives/med-3.2.1.tar.gz
HDF_ARCHIVES= ./archives/hdf5-1.8.14.tar.gz


HDF_CONFIG=scripts/generated/install_HDF.sh
MED_CONFIG=scripts/generated/install_MED.sh	

all : $(HDF_CONFIG) $(MED_CONFIG) $(HDF) $(MED)

archives:
	@ln -sf $(TRUST_ROOT)/externalpackages/MED archives
$(MED_CONFIG) : $(MED_INSTALL_BUILDER) $(HDF_INSTALLER) archives
	@sh $(MED_INSTALL_BUILDER)

$(HDF_CONFIG) : $(HDF_INSTALL_BUILDER) makefile ./scripts/provided/HDF_installers.sh ./scripts/provided/MED_installers.sh ./scripts/provided/checkers.sh ./scripts/provided/exceptions.sh ./scripts/provided/install_HDF_builder.sh ./scripts/provided/install_MED_builder.sh ./scripts/provided/installers.sh ./scripts/provided/utils.sh
	@sh $(HDF_INSTALL_BUILDER)

$(MED) : $(HDF) $(MED_ARCHIVES) makefile
	@sh $(MED_INSTALLER)
	@rm -f archives 

$(HDF) : $(HDF_INSTALLER) $(HDF_ARCHIVES) makefile
	@sh $(HDF_INSTALLER)


clean :
	@rm -rf build
	@rm -rf $(TRUST_MED_ROOT)/../install
	@rm -rf $(TRUST_MED_ROOT)/../MED
	@rm -f archives scripts/generated/*.sh
