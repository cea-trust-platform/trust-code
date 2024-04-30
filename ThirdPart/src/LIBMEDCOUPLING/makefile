archive_mc=$(TRUST_ROOT)/externalpackages/SALOME/medcoupling-9.12.0.tar.gz
archive_conf=$(TRUST_ROOT)/externalpackages/SALOME/configuration-9.12.0.tar.gz

env_file=$(TRUST_MEDCOUPLING_ROOT)/env.sh

all : $(env_file)

$(env_file) : makefile install.sh $(archive_mc) $(archive_conf) $(TRUST_MED_ROOT)/include/med.h medcoupling_9.10.0_gnu_485.diff tetra_mc.patch
	make clean
	rm -f $(env_file)
	./install.sh $(archive_mc) $(archive_conf)

test:
	./test.sh

clean:
	rm -rf $(TRUST_ROOT)/build/lib/medcoupling* $(TRUST_ROOT)/lib/src/LIBMEDCOUPLING $(TRUST_ROOT)/build/lib/configuration*

