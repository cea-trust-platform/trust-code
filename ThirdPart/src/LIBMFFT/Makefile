include $(TRUST_ENV)/make.$(TRUST_ARCH_CC)$(OPT)
# Makefile for MFFT Fast Fourier Transform routines
# =================================================
# $Header$
.SUFFIXES: .o .f .a

# F77 is the command to use to compile FORTRAN source code
# into object code.  On Unix machines with the f77 complier
# make this 'f77 -c', while in Unicos you would say cft77
#F77= cft77


# F77_FLAGS are the flags to be passed to the FORTRAN compiler.
# The original code came with 
# F77_FLAGS= -w -g -C
# However, there is a possible, meaningless error due to the -C option.
# The simpler version is given here, and you should convert this to the
# appropriate form for your compiler with a higher level of optimization
#F77_FLAGS= -g
F77_FLAGS= ${F77Flags}
F77=$(TRUST_F77)
# INSTALL is the command used to install the library
INSTALL=mv

# LIBDIR is the directory into which the library is to be installed.
# The most likely place for this is /usr/local/lib
#LIBDIR=/usr/local/lib


# ----------
# MFFT - Multidimensional Fast Fourier Transform library
LIBMMFT = $(TRUST_ROOT)/lib/libmfft.a
all: $(LIBMMFT)

OBJ = c2fft.o c3fft.o c4fft.o r2fft.o r3fft.o r4fft.o \
	mffta4.o mffta5.o mffta6.o mffta7.o mffta8.o mffta9.o mfftb4.o \
	mfftb5.o mfftb6.o mfftb7.o mfftb8.o mfftb9.o mfftc4.o mfftc5.o \
	mfftc6.o mfftc7.o mfftc8.o mfftc9.o mfftdm.o mfftds.o mfftdv.o \
	mfftim.o mfftis.o mfftiv.o mfftom.o mfftov.o mfftp.o mfftp1.o \
	mfftp2.o mfftp3.o mfftp4.o mfftrd.o mfftri.o mfftrp.o mfftz0.o


$(LIBMMFT): $(OBJ) 
	ar ru $(LIBMMFT) $(OBJ)
#	ranlib libmfft.a

install:  $(LIBMMFT)
	$(INSTALL) libmfft.a $(LIBDIR)

clean:
	rm -f $(LIBMMFT);rm -f *.o

source:	mfft.for

# ---------- How to make objects from F77 code on the ENCORE and SUN

.f.o:   ; $(F77) -c $(F77_FLAGS) $*.f

# ---------- ignore RCS versions, just do nothing 
.f~.f: ;

# ----------
# SOURCE CODE - one long file of all the routines

SOURCE = c2fft.f c3fft.f c4fft.f r2fft.f r3fft.f r4fft.f \
	mffta4.f mffta5.f mffta6.f mffta7.f mffta8.f mffta9.f mfftb4.f \
	mfftb5.f mfftb6.f mfftb7.f mfftb8.f mfftb9.f mfftc4.f mfftc5.f \
	mfftc6.f mfftc7.f mfftc8.f mfftc9.f mfftdm.f mfftds.f mfftdv.f \
	mfftim.f mfftis.f mfftiv.f mfftom.f mfftov.f mfftp.f mfftp1.f \
	mfftp2.f mfftp3.f mfftp4.f mfftrd.f mfftri.f mfftrp.f mfftz0.f

mfft.for: mfft.h $(SOURCE) 
	cat mfft.h $(SOURCE) > mfft.for

