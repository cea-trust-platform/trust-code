archive_mc=$(TRUST_ROOT)/externalpackages/SALOME/medcoupling-9.4.0.tar.gz
archive_conf=$(TRUST_ROOT)/externalpackages/SALOME/configuration-9.4.0.tar.gz

all : $(TRUST_MEDCOUPLING_ROOT)/env.sh

$(TRUST_MEDCOUPLING_ROOT)/env.sh : makefile install.sh $(archive_mc) $(archive_conf) $(TRUST_MED_ROOT)/include/med.h
	make clean
	rm -f $(TRUST_MEDCOUPLING_ROOT)/env.sh
	./install.sh $(archive_mc) $(archive_conf)

test:
	./test.sh

clean:
	rm -rf $(TRUST_ROOT)/build/lib/medcoupling* $(TRUST_ROOT)/lib/src/LIBMEDCOUPLING

