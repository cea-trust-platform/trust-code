archive=$(TRUST_ROOT)/externalpackages/medcoupling-7.8.0.tar.gz
$(TRUST_MEDCOUPLING_ROOT)/env.sh : makefile install.sh $(archvive) $(TRUST_MED_ROOT)/include/med.h
	./install.sh $(archive)

test:
	./test.sh
clean_build:
	rm -rf build install $(medcoupling) *~ $(TRUST_MEDCOUPLING_ROOT)
clean: clean_build
