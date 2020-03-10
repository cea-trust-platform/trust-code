OPT=_opt
include $(TRUST_ENV)/make.$(TRUST_ARCH_CC)$(OPT)
include $(TRUST_ENV)/make.lib$(LIB_EXTENSION)
include $(TRUST_ENV)/make.all_Includes.TRUST

LIB=$(TRUST_ROOT)/lib/libmfft.a

.SUFFIXES: .cpp .C .c .f .o
.cpp.o:
	@echo $< $(Defines); $(TRUST_CC) -c $< $(CppFlags)
.C.o:
	@echo $< $(Defines); $(TRUST_cc) -c $< $(ccFLAGS)
.c.o:
	@echo $< $(Defines); $(TRUST_cc) -c $< $(ccFLAGS)
.f.o:
	@echo $< $(Defines); $(TRUST_F77) -c $< $(F77Flags)

all:	$(LIB)

OBJS= \
	c2fft.o \
	c3fft.o \
	c4fft.o \
	fft_test.o \
	mffta4.o \
	mffta5.o \
	mffta6.o \
	mffta7.o \
	mffta8.o \
	mffta9.o \
	mfftb4.o \
	mfftb5.o \
	mfftb6.o \
	mfftb7.o \
	mfftb8.o \
	mfftb9.o \
	mfftc4.o \
	mfftc5.o \
	mfftc6.o \
	mfftc7.o \
	mfftc8.o \
	mfftc9.o \
	mfftdm.o \
	mfftds.o \
	mfftdv.o \
	mfftim.o \
	mfftis.o \
	mfftiv.o \
	mfftom.o \
	mfftov.o \
	mfftp.o \
	mfftp1.o \
	mfftp2.o \
	mfftp3.o \
	mfftp4.o \
	mfftrd.o \
	mfftri.o \
	mfftrp.o \
	mfftz0.o \
	r2fft.o \
	r3fft.o \
	r4fft.o

$(LIB): makefile $(SRCS)
	make lib
	
lib:	$(OBJS)
	ar cru $(LIB) $(OBJS)
	rm -f *.o

clean:
	rm -f $(LIB)

SRCS= \
	c2fft.f \
	c3fft.f \
	c4fft.f \
	fft_test.f \
	mffta4.f \
	mffta5.f \
	mffta6.f \
	mffta7.f \
	mffta8.f \
	mffta9.f \
	mfftb4.f \
	mfftb5.f \
	mfftb6.f \
	mfftb7.f \
	mfftb8.f \
	mfftb9.f \
	mfftc4.f \
	mfftc5.f \
	mfftc6.f \
	mfftc7.f \
	mfftc8.f \
	mfftc9.f \
	mfftdm.f \
	mfftds.f \
	mfftdv.f \
	mfftim.f \
	mfftis.f \
	mfftiv.f \
	mfftom.f \
	mfftov.f \
	mfftp.f \
	mfftp1.f \
	mfftp2.f \
	mfftp3.f \
	mfftp4.f \
	mfftrd.f \
	mfftri.f \
	mfftrp.f \
	mfftz0.f \
	r2fft.f \
	r3fft.f \
	r4fft.f

$(OBJS): $(TRUST_ENV)/make.$(TRUST_ARCH_CC)$(OPT)

