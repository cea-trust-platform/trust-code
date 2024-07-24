
SRC_DIR=$(TRUST_ROOT)/ThirdPart/src/LIBLATAFILTER/src
BUILD_DIR=$(TRUST_ROOT)/build/lib/LIBLATAFILTER
INSTALL_DIR=$(TRUST_ROOT)/lib/src/LIBLATAFILTER
LIB=$(INSTALL_DIR)/lib/liblatafilter.a

OPT=_opt
include $(TRUST_ENV)/make.$(TRUST_ARCH_CC)$(OPT)

CCompiler=$(TRUST_cc)
ifdef TRUST_cc_BASE_EXTP
  CCompiler=$(TRUST_cc_BASE_EXTP)
endif


# The below is taken from lata_tools 
trust_commun_src=$(TRUST_ROOT)/Outils/lata_tools/src/trust_commun/
trust_kern=$(TRUST_ROOT)/src/Kernel
SRCS=$(wildcard $(trust_commun_src)/*.cpp $(trust_commun_src)/*.h)
trust_ut=$(trust_kern)/Utilitaires
EXTRA_SRCS=$(trust_ut)/Static_Int_Lists.cpp $(trust_ut)/Nom.cpp $(trust_ut)/Noms.cpp $(trust_ut)/Motcle.cpp $(trust_ut)/Entree.cpp $(trust_ut)/EFichier.cpp $(trust_ut)/Entree_Fichier_base.cpp $(trust_ut)/Device.cpp $(trust_ut)/Objet_U.cpp
trust_ge=$(trust_kern)/Geometrie
EXTRA_SRCS+=$(trust_ge)/Octree_Double.cpp $(trust_ge)/Octree_Int.cpp $(trust_ge)/Connectivite_som_elem.cpp 
trust_mt=$(trust_kern)/Math
EXTRA_SRCS+=$(trust_mt)/TRUSTArray.cpp $(trust_mt)/TRUSTVect_tools.cpp $(trust_mt)/Array_base.cpp $(trust_mt)/ArrOfBit.cpp


all: $(LIB)

$(LIB): src/CMakeLists.txt makefile $(SRCS) $(EXTRA_SRCS) $(trust_ut) $(trust_ge) $(trust_mt)
	make clean
	cmake -S $(SRC_DIR) -B $(BUILD_DIR) -DCMAKE_CXX_FLAGS="$(CppFlags)" -DTRUST_DEFINES="$(Defines)" -DCMAKE_C_COMPILER="$(CCompiler)" -DCMAKE_CXX_COMPILER="$(TRUST_CC)"
	$(TRUST_MAKE) -C $(BUILD_DIR) install

clean:
	rm -rf ../../../build/lib/LIBLATAFILTER/ ../../../lib/src/LIBLATAFILTER/

