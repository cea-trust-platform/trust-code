trg=$(TRUST_ROOT)/exec/lata_tools/bin/compare_lata
tools_src=src
SRCS=$(wildcard $(tools_src)/trust_commun/*.cpp $(tools_src)/trust_commun/*.h $(tools_src)/trust_compat/*.cpp $(tools_src)/trust_compat/*.h $(tools_src)/trust_compat/*.tpp)

all: $(trg)
 
$(trg): $(tools_src)/CMakeLists.txt $(tools_src)/tools/CMakeLists.txt $(tools_src)/scripts/CMakeLists.txt $(tools_src)/build.sh Makefile $(TRUST_MEDCOUPLING_ROOT)/lib/libmedcoupling.a $(SRCS)
	(cd $(tools_src);./build.sh)
	(cd $(TRUST_ROOT)/exec;ln -sf lata_tools/bin/lata_analyzer .)
	(cd $(TRUST_ROOT)/exec;ln -sf lata_tools/bin/compare_lata .)
	(cd $(TRUST_ROOT)/exec/lata_tools/;ln -sf ../../Outils/lata_tools/src/tools/env.sh .)

clean:
	rm -rf $(TRUST_ROOT)/build/Outils/lata_tools $(TRUST_ROOT)/exec/lata_tools $(TRUST_ROOT)/exec/lata_analyzer $(TRUST_ROOT)/exec/compare_lata
