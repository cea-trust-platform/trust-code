trg=$(TRUST_ROOT)/exec/lata_tools/bin/compare_lata
env_lata_tools=$(TRUST_ROOT)/exec/lata_tools/env.sh
env_lata_tools_src=$(TRUST_ROOT)/Outils/lata_tools/src/tools/env.sh
tools_src=src

SRCS=$(wildcard $(tools_src)/trust_commun/*.cpp $(tools_src)/trust_commun/*.h $(tools_src)/trust_compat/*.cpp $(tools_src)/trust_compat/*.h $(tools_src)/trust_compat/*.tpp)

all: $(trg)
 
$(trg): $(tools_src)/CMakeLists.txt $(tools_src)/tools/CMakeLists.txt $(tools_src)/scripts/CMakeLists.txt $(tools_src)/build.sh Makefile $(TRUST_MEDCOUPLING_ROOT)/lib/libmedcoupling.a $(SRCS)
	(cd $(tools_src);./build.sh)
	(cd $(TRUST_ROOT)/exec;ln -sf lata_tools/bin/lata_analyzer .)
	(cd $(TRUST_ROOT)/exec;ln -sf $(trg) .)
	(ln -sf $(env_lata_tools_src) $(env_lata_tools))

clean:
	rm -rf $(TRUST_ROOT)/build/Outils/lata_tools $(TRUST_ROOT)/exec/lata_tools $(TRUST_ROOT)/exec/lata_analyzer $(TRUST_ROOT)/exec/compare_lata
