include $(TRUST_ENV)/make.$(TRUST_ARCH_CC)$(OPT)
LIBDIR = $(TRUST_ROOT)/lib
CC = $(TRUST_cc)
INCLUDES = -I. 

#DEBUGFLAGS = -DMETISLIB 
#EXTRADEFINES =  $(DEBUGFLAGS) 
#CFLAGS = $(ccFLAGS) $(EXTRADEFINES) $(INCLUDES)
CFLAGS = $(ccFLAGS) $(INCLUDES)


AR = ar r
RANLIB = ranlib
all: $(LIBDIR)/libmfft_c.a

MLEVEL_OBJS =   jmccfft3d.o  jmccm1d5.o      jmcsfft2d.o  jmdecoup3.o  \
		jmgetmessage.o    jmgetstop.o  jmscm1d.o      jmtable.o \
		jmccfftm.o   jmccm1dp.o      jmcsfft3d.o  jmerreur1.o \
		jmgetnwork.o      jmrfftmlt.o  jmscm1dxy.o    jmtransp.o \
		jmccm1d.o    jmcctranspcs.o  jmcsfftm.o   jmerreur2.o \
		jmfftf2c.o     jmgetsetcode.o    jmscfft.o    jmsetcode.o \
		ipow.o          jmccm1d2.o   jmcfftmlt.o     jmcsm1d.o \
		jmerreur3.o  jmfftfax.o     jmgetseterreur.o  jmscfft2d.o  jmseterreur.o \
		jmccfft.o       jmccm1d3.o   jmcftfax.o      jmcsm1dxy.o \
		jmerreur4.o  jmgetcode.o    jmgetsetnwork.o   jmscfft3d.o  jmsetnwork.o \
		jmccfft2d.o     jmccm1d4.o   jmcsfft.o       jmdecoup.o \
		jmfact.o     jmgeterreur.o  jmgetsetstop.o    jmscfftm.o   jmsetstop.o


$(LIBDIR)/libmfft_c.a: $(MLEVEL_OBJS)
	@$(AR) $@ $(MLEVEL_OBJS) 


clean:
	rm -f $(LIBDIR)/libmfft_c.a;rm -f *.o


checkin:
	@for file in *.[c,h]; \
	  do \
	  ci -u -m'Maintance' $$file;\
	  done 

checkin2:
	@for file in *.[c,h]; \
	  do \
	  ci $$file;\
	  rcs -U $$file;\
	  co $$file;\
	  done 

realclean:
	rm -f *.o $(LIBDIR)/libmfft_c.a
