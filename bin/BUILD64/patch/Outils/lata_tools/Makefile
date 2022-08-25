env_lata_tools=$(TRUST_ROOT)/exec/lata_tools_install/env.sh
tools_src=src
SRCS=$(wildcard $(tools_src)/trust_commun/*.cpp $(tools_src)/trust_commun/*.h $(tools_src)/trust_compat/*.cpp $(tools_src)/trust_compat/*.h $(tools_src)/trust_compat/*.tpp)

all: $(env_lata_tools)
 
$(env_lata_tools): $(tools_src)/CMakeLists.txt $(tools_src)/build.sh Makefile $(TRUST_MEDCOUPLING_ROOT)/lib/libmedcoupling.a $(SRCS)
	(cd $(tools_src);./build.sh)
	(cd $(TRUST_ROOT)/exec;ln -sf lata_tools_install/bin/lata2dx .)
	(cd $(TRUST_ROOT)/exec;ln -sf lata_tools_install/bin/compare_lata .)

clean:
	rm -rf $(TRUST_ROOT)/build/Outils/lata_tools $(TRUST_ROOT)/exec/lata_tools_install  $(TRUST_ROOT)/exec/lata_tools $(TRUST_ROOT)/exec/compare_lata
