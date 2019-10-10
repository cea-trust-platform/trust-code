archive=$(TRUST_ROOT)/externalpackages/medcoupling-8.5.0.tar.gz
patchs=DisjointDEC.cxx DisjointDEC.hxx MEDCouplingFieldDouble.hxx MEDCouplingMemArray.cxx MEDCouplingSkyLineArray.cxx
$(TRUST_MEDCOUPLING_ROOT)/env.sh : makefile install.sh $(archvive) $(TRUST_MED_ROOT)/include/med.h $(patchs)
	./install.sh $(archive)

test:
	./test.sh
clean_build:
	rm -rf build install $(medcoupling) *~ $(TRUST_MEDCOUPLING_ROOT)
clean: clean_build
