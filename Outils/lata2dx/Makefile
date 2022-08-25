env_lata2dx=$(TRUST_ROOT)/exec/lata2dx_install/env.sh
tools_src=src
SRCS=$(wildcard $(tools_src)/commun_triou/*.cpp $(tools_src)/commun_triou/*.h $(tools_src)/triou_compat/*.cpp $(tools_src)/triou_compat/*.h $(tools_src)/triou_compat/*.tpp)

all: $(env_lata2dx)
 
$(env_lata2dx): $(tools_src)/CMakeLists.txt $(tools_src)/build.sh Makefile $(TRUST_MEDCOUPLING_ROOT)/lib/libmedcoupling.a $(SRCS)
	(cd $(tools_src);./build.sh)
	(cd $(TRUST_ROOT)/exec;ln -sf lata2dx_install/bin/lata2dx .)
	(cd $(TRUST_ROOT)/exec;ln -sf lata2dx_install/bin/compare_lata .)

clean:
	rm -rf $(TRUST_ROOT)/build/Outils/lata2dx $(TRUST_ROOT)/exec/lata2dx_install  $(TRUST_ROOT)/exec/lata2dx $(TRUST_ROOT)/exec/compare_lata