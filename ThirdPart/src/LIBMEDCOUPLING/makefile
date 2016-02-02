$(TRUST_MEDCOUPLING_ROOT)/env.sh : makefile install.sh MED_SRC.tgz $(TRUST_MED_ROOT)/include/med++.h
	./install.sh
MED_SRC.tgz:
	ln -sf $(TRUST_ROOT)/externalpackages/MEDCoupling_Standalone_770.tgz MED_SRC.tgz

test:
	./test.sh
clean_build:
	rm -rf build install MED_SRC *~ $(TRUST_MEDCOUPLING_ROOT)
clean: clean_build
	rm -rf  MED_SRC.tgz
