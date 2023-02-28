BUILD_DIR=$(TRUST_ROOT)/build/lib/LIBLATAFILTER
SRC_DIR=$(TRUST_ROOT)/ThirdPart/src/LIBLATAFILTER/src
OPT=_opt
include $(TRUST_ENV)/make.$(TRUST_ARCH_CC)$(OPT)
latafilter=$(TRUST_LATAFILTER)/lib/liblatafilter.a

all:
	(mkdir -p $(BUILD_DIR); cd $(BUILD_DIR) ; cmake $(SRC_DIR) -DCMAKE_CXX_FLAGS="$(CppFlags)" -DTRUST_DEFINES="$(Defines)" ; $(TRUST_MAKE) install)

clean:
	rm -rf ../../../lib/src/LIBLATAFILTER ../../../build/lib/LIBLATAFILTER


