archive=$(TRUST_ROOT)/externalpackages/SALOME/packagespy.tar.gz
install_dir=$(TRUST_ROOT)/Outils/trustpy/install
log_file=$(TRUST_ROOT)/Outils/trustpy/trustpy.log
SHELL=/bin/bash -o pipefail   # make sure failures are spotted before tee below:

all : makefile install check

install: $(archive) install.sh clean
	./install.sh $(archive) 2>&1 | tee $(log_file)

doc:
	./gen_doc.sh

check:
	./check_all.sh

clean:
	rm -rf $(install_dir)

.PHONY: install check clean doc
