archive_mc=$(TRUST_ROOT)/externalpackages/SALOME/medcoupling-9.4.0.tar.gz
archive_conf=$(TRUST_ROOT)/externalpackages/SALOME/configuration-9.4.0.tar.gz

$(TRUST_MEDCOUPLING_ROOT)/env.sh : makefile install.sh $(archvive_mc) $(archive_conf) $(TRUST_MED_ROOT)/include/med.h #$(patchs)
	./install.sh $(archive_mc) $(archive_conf)

test:
	./test.sh
clean_build:
	rm -rf build install $(medcoupling) *~ $(TRUST_MEDCOUPLING_ROOT)
clean: clean_build
