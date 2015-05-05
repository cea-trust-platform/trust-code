#-----------------------------------------------------------------------
#                     S P A R S K I T  makefile 
#-----------------------------------------------------------------------
#
# There are three types of source files in SPARSKIT:
# 1. library source files
# 2. non-library and unsupported source files
# 3. test programs
#
# Simply type "make" to make the SPARSKIT library.
#
# "make all" will additionally make all the unsupported objects.
#
# To make the test programs, use the makefiles in each subdirectory,
# or see the dotests script.
# There are no references to test programs in this makefile.
# Some test programs use the non-library objects from other directories,
# so do a "make all" before making test programs.
#
# Last Updated:  May 9, 1994.
include $(TRUST_ENV)/make.$(TRUST_ARCH_CC)$(OPT)
SHELL=/bin/sh
AR = ar rc

# Cray options
F77 = $(TRUST_F77)
#AR = bld rv
#OPT = -c -Wf"-dp" 

#
# library objects
#
OBJ =	BLASSM/blassm.o		\
	BLASSM/matvec.o		\
	FORMATS/formats.o	\
	FORMATS/unary.o		\
	INFO/infofun.o		\
	INOUT/inout.o		\
	ITSOL/ilut.o		\
	ITSOL/iters.o		\
	MATGEN/FDIF/genmat.o	\
	MATGEN/FEM/elmtlib2.o	\
	MATGEN/FEM/femgen.o	\
	MATGEN/FEM/meshes.o	\
	MATGEN/MISC/sobel.o	\
	MATGEN/MISC/zlatev.o	\
	ORDERINGS/ccn.o		\
	ORDERINGS/color.o	\
	ORDERINGS/dsepart.o

#
# non-library and unsupported objects
#
OBJ2 =	ITSOL/itaux.o		\
	MATGEN/FDIF/functns.o	\
	MATGEN/FEM/functns2.o	\
	UNSUPP/BLAS1/blas1.o	\
	UNSUPP/MATEXP/exppro.o	\
	UNSUPP/MATEXP/phipro.o	\
	UNSUPP/PLOTS/psgrd.o	\
	UNSUPP/PLOTS/texgrid1.o	\
	UNSUPP/PLOTS/texplt1.o

DIRS =	.			\
	BLASSM			\
	FORMATS			\
	INFO			\
	INOUT			\
	ITSOL			\
	MATGEN/FDIF		\
	MATGEN/FEM		\
	MATGEN/MISC		\
	ORDERINGS		\
	UNSUPP/BLAS1		\
	UNSUPP/MATEXP		\
	UNSUPP/PLOTS

lib libskit.a: $(OBJ) 
	@$(AR) $(TRUST_ROOT)/lib/libskit.a  $(OBJ) 
# do not ranlib on some architectures

clean: 
	@rm -f $(TRUST_ROOT)/lib/libskit.a ;\
	for dir in $(DIRS) ;\
          do \
          echo cleaning $$dir ;\
          (cd $$dir; rm -f *~ *.o *.ex .,* fort.* ftn?? *.mat *.pic *.ps *.trace core) ;\
          done

tarit: 
	(cd ..; tar cvf - SPARSKIT2 | gzip -c > SPARSKIT2.tar.gz) 

all: $(OBJ) $(OBJ2) libskit.a

BLASSM/blassm.o: BLASSM/blassm.f
	(cd BLASSM ; $(F77)  $(F77Flags) -c blassm.f)
BLASSM/matvec.o: BLASSM/matvec.f
	(cd BLASSM ; $(F77)  $(F77Flags) -c matvec.f)
FORMATS/formats.o: FORMATS/formats.f
	(cd FORMATS ; $(F77)  $(F77Flags) -c formats.f)
FORMATS/unary.o: FORMATS/unary.f
	(cd FORMATS ; $(F77)  $(F77Flags) -c unary.f)
INFO/infofun.o: INFO/infofun.f
	(cd INFO ; $(F77)  $(F77Flags) -c infofun.f)
INOUT/inout.o: INOUT/inout.f
	(cd INOUT; $(F77)  $(F77Flags) -c inout.f)
ITSOL/ilut.o: ITSOL/ilut.f
	(cd ITSOL; $(F77)  $(F77Flags) -D$(TRUST_ARCH) -c ilut.f)
ITSOL/iters.o: ITSOL/iters.f
	(cd ITSOL; $(F77)  $(F77Flags) -c iters.f)
ITSOL/itaux.o: ITSOL/itaux.f
	(cd ITSOL; $(F77)  $(F77Flags) -c itaux.f)
MATGEN/FDIF/genmat.o: MATGEN/FDIF/genmat.f
	(cd MATGEN/FDIF ; $(F77)  $(F77Flags) -c genmat.f)
MATGEN/FDIF/functns.o: MATGEN/FDIF/functns.f
	(cd MATGEN/FDIF ; $(F77)  $(F77Flags) -c functns.f)
MATGEN/FEM/elmtlib2.o: MATGEN/FEM/elmtlib2.f
	(cd MATGEN/FEM ; $(F77)  $(F77Flags) -c elmtlib2.f)
MATGEN/FEM/femgen.o: MATGEN/FEM/femgen.f
	(cd MATGEN/FEM ; $(F77)  $(F77Flags) -c femgen.f)
MATGEN/FEM/functns2.o : MATGEN/FEM/functns2.f 
	(cd MATGEN/FEM ; $(F77)  $(F77Flags) -c functns2.f)
MATGEN/FEM/meshes.o: MATGEN/FEM/meshes.f
	(cd MATGEN/FEM ; $(F77)  $(F77Flags) -c meshes.f)
MATGEN/MISC/sobel.o: MATGEN/MISC/sobel.f
	(cd MATGEN/MISC ; $(F77)  $(F77Flags) -c sobel.f)
MATGEN/MISC/zlatev.o: MATGEN/MISC/zlatev.f
	(cd MATGEN/MISC ; $(F77)  $(F77Flags) -c zlatev.f)
ORDERINGS/ccn.o: ORDERINGS/ccn.f
	(cd ORDERINGS ; $(F77)  $(F77Flags) -c ccn.f)
ORDERINGS/color.o: ORDERINGS/color.f
	(cd ORDERINGS ; $(F77)  $(F77Flags) -c color.f)
ORDERINGS/dsepart.o: ORDERINGS/dsepart.f
	(cd ORDERINGS ; $(F77)  $(F77Flags) -c dsepart.f)
UNSUPP/BLAS1/blas1.o: UNSUPP/BLAS1/blas1.f
	(cd UNSUPP/BLAS1 ; $(F77)  $(F77Flags) -c blas1.f)
UNSUPP/MATEXP/exppro.o: UNSUPP/MATEXP/exppro.f
	(cd UNSUPP/MATEXP ; $(F77)  $(F77Flags) -c exppro.f)
UNSUPP/MATEXP/phipro.o: UNSUPP/MATEXP/phipro.f
	(cd UNSUPP/MATEXP ; $(F77)  $(F77Flags) -c phipro.f)
UNSUPP/PLOTS/psgrd.o : UNSUPP/PLOTS/psgrd.f 
	(cd UNSUPP/PLOTS ; $(F77) $(F77Flags) -c psgrd.f)
UNSUPP/PLOTS/texgrid1.o : UNSUPP/PLOTS/texgrid1.f 
	(cd UNSUPP/PLOTS ; $(F77) $(F77Flags) -c texgrid1.f)
UNSUPP/PLOTS/texplt1.o : UNSUPP/PLOTS/texplt1.f 
	(cd UNSUPP/PLOTS ; $(F77) $(F77Flags) -c texplt1.f)
