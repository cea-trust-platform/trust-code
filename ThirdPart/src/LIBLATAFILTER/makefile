SRC_DIR=$(TRUST_ROOT)/ThirdPart/src/LIBLATAFILTER/src
BUILD_DIR=$(TRUST_ROOT)/build/lib/LIBLATAFILTER
INSTALL_DIR=$(TRUST_ROOT)/lib/src/LIBLATAFILTER

OPT=_opt
include $(TRUST_ENV)/make.$(TRUST_ARCH_CC)$(OPT)

all:
	cmake -S $(SRC_DIR) -B $(BUILD_DIR) -DCMAKE_CXX_FLAGS="$(CppFlags)" -DTRUST_DEFINES="$(Defines)"
	$(TRUST_MAKE) -C $(BUILD_DIR) install

clean:
	rm -rf ../../../build/lib/LIBLATAFILTER/ ../../../lib/src/LIBLATAFILTER/


