# include TRUST build flags
include $(TRUST_ENV)/make.$(TRUST_ARCH_CC)$(OPT)

# Add -w cause -Wshadow will force to change the ICoCo API:
.SUFFIXES: .cpp
.cpp.o:
	$(TRUST_CC) -Iinclude -o $@ -c $< $(CppFlags) -w

MALIB=$(TRUST_ICOCOAPI)/lib/libicocoapi.a

OBJS= \
	src/ICoCoExceptions.o \
	src/ICoCoTrioField.o \
	src/ICoCoProblem.o \
	src/ICoCoField.o
install: $(MALIB) ./install_include.sh
	@./install_include.sh
all: $(MALIB)

$(MALIB) : makefile include/ICoCoExceptions.* include/ICoCoTrioField.* include/ICoCoField.* include/ICoCoProblem.* include/ICoCoMEDDoubleField.* include/ICoCoMEDIntField.* $(SRCS)
	@make clean 
	@make $(OBJS)
	@mkdir -p $(TRUST_ICOCOAPI)/lib
	@ar cruv $(MALIB) $(OBJS)
	@echo "$(MALIB) updated"
	@rm -f $(OBJS)	
clean:
	@rm -rf $(MALIB) $(OBJS) $(TRUST_LIB)/src/LIBICOCOAPI
	
SRCS= \
	src/ICoCoExceptions.cpp \
	src/ICoCoTrioField.cpp \
	src/ICoCoProblem.cpp \
	src/ICoCoField.cpp
	
