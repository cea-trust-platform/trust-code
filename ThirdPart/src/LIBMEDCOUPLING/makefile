$(TRUST_MEDCOUPLING_ROOT)/env.sh : makefile install.sh MED_SRC.tgz
	./install.sh
MED_SRC.tgz:
	ln -sf $(TRUST_ROOT)/externalpackages/MEDCoupling_Standalone_715.tgz MED_SRC.tgz

test:
	./test.sh
clean:
	rm -rf build install MED_SRC *~ $(TRUST_MEDCOUPLING_ROOT) 
