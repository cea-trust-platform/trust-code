OPT=_opt
include $(TRUST_ENV)/make.$(TRUST_ARCH_CC)$(OPT)
include $(TRUST_ENV)/make.lib$(LIB_EXTENSION)
include $(TRUST_ENV)/make.all_Includes.TRUST

LIB=$(TRUST_ROOT)/lib/libmfft_c.a

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
	ipow.o \
	jmccfft.o \
	jmccfft2d.o \
	jmccfft3d.o \
	jmccfftm.o \
	jmccm1d.o \
	jmccm1d2.o \
	jmccm1d3.o \
	jmccm1d4.o \
	jmccm1d5.o \
	jmccm1dp.o \
	jmcctranspcs.o \
	jmcfftmlt.o \
	jmcftfax.o \
	jmcsfft.o \
	jmcsfft2d.o \
	jmcsfft3d.o \
	jmcsfftm.o \
	jmcsm1d.o \
	jmcsm1dxy.o \
	jmdecoup.o \
	jmdecoup3.o \
	jmerreur1.o \
	jmerreur2.o \
	jmerreur3.o \
	jmerreur4.o \
	jmfact.o \
	jmfftf2c.o \
	jmfftfax.o \
	jmgetcode.o \
	jmgeterreur.o \
	jmgetmessage.o \
	jmgetnwork.o \
	jmgetsetcode.o \
	jmgetseterreur.o \
	jmgetsetnwork.o \
	jmgetsetstop.o \
	jmgetstop.o \
	jmrfftmlt.o \
	jmscfft.o \
	jmscfft2d.o \
	jmscfft3d.o \
	jmscfftm.o \
	jmscm1d.o \
	jmscm1dxy.o \
	jmsetcode.o \
	jmseterreur.o \
	jmsetnwork.o \
	jmsetstop.o \
	jmtable.o \
	jmtransp.o

$(LIB): makefile $(SRCS)
	make lib
	
lib:	$(OBJS)
	ar cru $(LIB) $(OBJS)
	rm -f *.o

clean:
	rm -f $(LIB)

SRCS= \
	ipow.c \
	jmccfft.c \
	jmccfft2d.c \
	jmccfft3d.c \
	jmccfftm.c \
	jmccm1d.c \
	jmccm1d2.c \
	jmccm1d3.c \
	jmccm1d4.c \
	jmccm1d5.c \
	jmccm1dp.c \
	jmcctranspcs.c \
	jmcfftmlt.c \
	jmcftfax.c \
	jmcsfft.c \
	jmcsfft2d.c \
	jmcsfft3d.c \
	jmcsfftm.c \
	jmcsm1d.c \
	jmcsm1dxy.c \
	jmdecoup.c \
	jmdecoup3.c \
	jmerreur1.c \
	jmerreur2.c \
	jmerreur3.c \
	jmerreur4.c \
	jmfact.c \
	jmfftf2c.c \
	jmfftfax.c \
	jmgetcode.c \
	jmgeterreur.c \
	jmgetmessage.c \
	jmgetnwork.c \
	jmgetsetcode.c \
	jmgetseterreur.c \
	jmgetsetnwork.c \
	jmgetsetstop.c \
	jmgetstop.c \
	jmrfftmlt.c \
	jmscfft.c \
	jmscfft2d.c \
	jmscfft3d.c \
	jmscfftm.c \
	jmscm1d.c \
	jmscm1dxy.c \
	jmsetcode.c \
	jmseterreur.c \
	jmsetnwork.c \
	jmsetstop.c \
	jmtable.c \
	jmtransp.c

$(OBJS): $(TRUST_ENV)/make.$(TRUST_ARCH_CC)$(OPT)

