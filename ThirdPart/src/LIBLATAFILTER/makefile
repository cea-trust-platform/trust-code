SRC_DIR=$(TRUST_ROOT)/ThirdPart/src/LIBLATAFILTER/src
BUILD_DIR=$(TRUST_ROOT)/build/lib/LIBLATAFILTER
INSTALL_DIR=$(TRUST_ROOT)/lib/src/LIBLATAFILTER
LIB=$(INSTALL_DIR)/liblatafilter.a

OPT=_opt
include $(TRUST_ENV)/make.$(TRUST_ARCH_CC)$(OPT)

CCompiler=$(TRUST_cc)
ifdef TRUST_cc_BASE_EXTP
  CCompiler=$(TRUST_cc_BASE_EXTP)
endif


all: $(LIB)

$(LIB): src/CMakeLists.txt makefile
	rm -rf ../../../lib/src/LIBLATAFILTER/*
	cmake -S $(SRC_DIR) -B $(BUILD_DIR) -DCMAKE_CXX_FLAGS="$(CppFlags)" -DTRUST_DEFINES="$(Defines)" -DCMAKE_C_COMPILER="$(CCompiler)" -DCMAKE_CXX_COMPILER="$(TRUST_CC)"
	$(TRUST_MAKE) -C $(BUILD_DIR) install

clean:
	rm -rf ../../../build/lib/LIBLATAFILTER/ ../../../lib/src/LIBLATAFILTER/

