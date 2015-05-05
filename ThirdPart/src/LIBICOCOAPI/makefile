# include TRUST build flags
include $(TRUST_ENV)/make.$(TRUST_ARCH_CC)$(OPT)

# Add -w cause -Wshadow will force to change the ICoCo API:
.SUFFIXES: .cpp
.cpp.o:
	$(TRUST_CC) -Iinclude -o $@ -c $< $(CppFlags) -w

MALIB=$(TRUST_ICOCOAPI)/lib/libicocoapi.a

OBJS= \
	src/Exceptions.o \
	src/ICoCoTrioField.o \
	src/Problem.o \
	src/ICoCoField.o
install: $(MALIB) ./install_include.sh
	@./install_include.sh
all: $(MALIB)

$(MALIB) : $(OBJS) makefile include/Exceptions.h include/ICoCoTrioField.h include/ICoCoField.h include/Problem.h $(SRCS)
	@mkdir -p $(TRUST_ICOCOAPI)/lib
	@ar cruv $(MALIB) $(OBJS)
	@echo "$(MALIB) est a jour".
	
clean:
	@rm -rf $(MALIB) $(OBJS) $(TRUST_LIB)/src/LIBICOCOAPI
	
SRCS= \
	src/Exceptions.cpp \
	src/ICoCoTrioField.cpp \
	src/Problem.cpp \
	src/ICoCoField.cpp
	
