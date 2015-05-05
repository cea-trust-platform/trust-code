# On compile en optimise car il s'avere qu'en debug
# plusieurs cas tests (eg: FTD_Colonne_VDF_2D et
# plus generalement tous les cas avec particules) 
# donnent des ecarts 
include $(TRUST_ENV)/make.$(TRUST_ARCH_CC)_opt
PROGRAM=$(TRUST_ROOT)/exec/lance_test_seq_par
SONDE=$(TRUST_ROOT)/exec/compare_sonde
all: $(PROGRAM) $(SONDE)
$(PROGRAM): lance_test_seq_par.cpp
	$(TRUST_CC) -g $(CppFlags) -c lance_test_seq_par.cpp
	$(TRUST_CC) -g -o $(PROGRAM) lance_test_seq_par.o $(SYSLIBS)
$(SONDE): compare_sonde.cpp
	$(TRUST_CC) -g $(CppFlags) -c compare_sonde.cpp
	$(TRUST_CC) -g -o $(SONDE) compare_sonde.o $(SYSLIBS)
clean:
	rm -f $(PROGRAM) $(SONDE) *.o
