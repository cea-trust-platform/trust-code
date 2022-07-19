env_lata2dx=$(TRUST_ROOT)/exec/lata2dx_install/env.sh
SRCS=$(wildcard lata2dx/commun_triou/*.cpp lata2dx/commun_triou/*.h lata2dx/triou_compat/*.cpp lata2dx/triou_compat/*.h)

all: $(env_lata2dx)
 
$(env_lata2dx): lata2dx/CMakeLists.txt lata2dx/new_build.sh Makefile $(TRUST_MEDCOUPLING_ROOT)/lib/libmedcoupling.a $(SRCS)
	(cd lata2dx;./new_build.sh)
	(cd $(TRUST_ROOT)/exec;ln -sf lata2dx_install/bin/lata2dx .)
	(cd $(TRUST_ROOT)/exec;ln -sf lata2dx_install/bin/compare_lata .)
	#cp -f lance_dx lata_get_nb_timesteps.sh clean_trio_data $(TRUST_ROOT)/exec	
	#(cd lata2dx_reactor;./build_lata2dx_reactor.sh)
	#cp -f lata2dx_reactor/build/lata2dx_reactor $(TRUST_ROOT)/exec

clean:
	rm -rf $(TRUST_ROOT)/build/Outils/lata2dx $(TRUST_ROOT)/exec/lata2dx_install  $(TRUST_ROOT)/exec/lata2dx $(TRUST_ROOT)/exec/compare_lata $(TRUST_ROOT)/exec/lata2dx_reactor
	rm -f $(TRUST_ROOT)/exec/lance_dx $(TRUST_ROOT)/exec/lata_get_nb_timesteps.sh $(TRUST_ROOT)/exec/clean_trio_data
