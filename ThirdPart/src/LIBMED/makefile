MED_INSTALL_BUILDER=./scripts/provided/install_MED_builder.sh
HDF_INSTALL_BUILDER=./scripts/provided/install_HDF_builder.sh
 
MED_INSTALLER=./scripts/generated/install_MED.sh
HDF_INSTALLER=./scripts/generated/install_HDF.sh

MED=$(TRUST_MED_ROOT)/lib/libmed.a
HDF=$(TRUST_MED_ROOT)/lib/libhdf5.a

MED_ARCHIVES=./archives/med-2.3.6.tar.gz ./archives/med-3.0.7.tar.gz
HDF_ARCHIVES=./archives/hdf5-1.6.9.tar.gz ./archives/hdf5-1.8.13.tar.gz

MED_ADDITIONNAL_FILES=./data/med++.h.in  ./data/nomed++.h.in

HDF_CONFIG=scripts/generated/install_HDF.sh
MED_CONFIG=scripts/generated/install_MED.sh	

all : $(HDF_CONFIG) $(MED_CONFIG) $(HDF) $(MED) ./scripts/provided/MED_installers.sh scripts/provided/installers.sh 

archives:
	@ln -sf $(TRUST_ROOT)/externalpackages/MED archives
$(MED_CONFIG) : $(MED_INSTALL_BUILDER) $(HDF_INSTALLER) archives
	@sh $(MED_INSTALL_BUILDER)

$(HDF_CONFIG) : $(HDF_INSTALL_BUILDER) makefile
	@sh $(HDF_INSTALL_BUILDER)

$(MED) : $(HDF) $(MED_ARCHIVES) $(MED_ADDITIONNAL_FILES) makefile
	@sh $(MED_INSTALLER)

$(HDF) : $(HDF_INSTALLER) $(HDF_ARCHIVES) makefile
	@sh $(HDF_INSTALLER)


clean :
	@rm -rf build
	@rm -rf $(TRUST_MED_ROOT) MED
	@rm -rf ./install
