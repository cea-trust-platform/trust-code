trg=$(TRUST_ROOT)/exec/lata_tools/bin/compare_lata
tools_src=src
trust_kern=$(TRUST_ROOT)/src/Kernel

# All the sources for lata tools ... TODO - is there a better way using CMake computed dpys?
SRCS=$(wildcard $(tools_src)/trust_commun/*.cpp $(tools_src)/trust_commun/*.h $(tools_src)/tools/*.i $(tools_src)/tools/*.h $(tools_src)/tools/*.cpp)
trust_ut=$(trust_kern)/Utilitaires
EXTRA_SRCS=$(trust_ut)/Static_Int_Lists.cpp $(trust_ut)/Nom.cpp $(trust_ut)/Noms.cpp $(trust_ut)/Motcle.cpp $(trust_ut)/Entree.cpp $(trust_ut)/EFichier.cpp $(trust_ut)/Entree_Fichier_base.cpp $(trust_ut)/Device.cpp $(trust_ut)/Objet_U.cpp
trust_ge=$(trust_kern)/Geometrie
EXTRA_SRCS+=$(trust_ge)/Octree_Double.cpp $(trust_ge)/Octree_Int.cpp $(trust_ge)/Connectivite_som_elem.cpp 
trust_mt=$(trust_kern)/Math
EXTRA_SRCS+=$(trust_mt)/TRUSTArray.cpp $(trust_mt)/TRUSTVect_tools.cpp $(trust_mt)/Array_base.cpp $(trust_mt)/ArrOfBit.cpp

all: $(trg)

# Retrieve TRUST_DEFINES variable:
OPT=_opt
include $(TRUST_ENV)/make.$(TRUST_ARCH_CC)$(OPT)
 
$(trg): $(tools_src)/CMakeLists.txt $(tools_src)/tools/CMakeLists.txt $(tools_src)/scripts/CMakeLists.txt $(tools_src)/build.sh Makefile $(TRUST_MEDCOUPLING_ROOT)/lib/libmedcoupling.a $(SRCS)
	(cd $(tools_src);env TRUST_DEFINES="$(Defines)" EXTRA_SRCS="$(EXTRA_SRCS)" ./build.sh)
	(cd $(TRUST_ROOT)/exec;ln -sf lata_tools/bin/lata_analyzer .)
	(cd $(TRUST_ROOT)/exec;ln -sf lata_tools/bin/compare_lata .)
	(cd $(TRUST_ROOT)/exec/lata_tools/;ln -sf ../../Outils/lata_tools/src/tools/env.sh .)

clean:
	rm -rf $(TRUST_ROOT)/build/Outils/lata_tools $(TRUST_ROOT)/exec/lata_tools $(TRUST_ROOT)/exec/lata_analyzer $(TRUST_ROOT)/exec/compare_lata
