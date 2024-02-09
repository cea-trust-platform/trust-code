archive=$(TRUST_ROOT)/externalpackages/SALOME/packagespy.tar.gz
install_dir=$(TRUST_ROOT)/Outils/trustpy/install
log_file=$(TRUST_ROOT)/Outils/trustpy/trustpy.log
SHELL=/bin/bash -o pipefail   # make sure failures are spotted before tee below:

all : Makefile $(archive) install.sh
	make clean
	./install.sh $(archive) 2>&1 | tee $(log_file)
	
clean:
	rm -rf $(install_dir)
