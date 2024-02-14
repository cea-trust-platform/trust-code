archive=$(TRUST_ROOT)/externalpackages/SALOME/packagespy.tar.gz
install_dir=$(TRUST_ROOT)/Outils/trustpy/install
log_file=$(TRUST_ROOT)/Outils/trustpy/trustpy.log
SHELL=/bin/bash -o pipefail   # make sure failures are spotted before tee below:

all : makefile install check

install: $(archive) install.sh clean
	env TRUSTPY_FROM_TRUST=1 ./install.sh $(archive) 2>&1 | tee $(log_file)

doc:
	env TRUSTPY_FROM_TRUST=1 ./gen_doc.sh

check:
	env TRUSTPY_FROM_TRUST=1 ./check_all.sh

clean:
	echo "Cleaning ..."
	rm -rf $(install_dir)

.PHONY: install check clean doc
