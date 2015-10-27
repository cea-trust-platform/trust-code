all:compil

compil:
	#(cd lata2dx;./build_lata2dx.sh)	
	(cd lata2dx;./new_build.sh) || exit -1
	cp -f lata2dx/build/lata2dx $(TRUST_ROOT)/exec
	cp -f lata2dx/build/compare_lata $(TRUST_ROOT)/exec
	cp -f lance_dx lata_get_nb_timesteps.sh clean_trio_data $(TRUST_ROOT)/exec	
	rm -r -f lata2dx/build lata2dx/install
	(cd lata2dx_reactor;./build_lata2dx_reactor.sh) || exit -1
	cp -f lata2dx_reactor/build/lata2dx_reactor $(TRUST_ROOT)/exec
	rm -r -f lata2dx_reactor/build

debug: 
	(cd lata2dx;./build_lata2dx.sh debug)
	cp -f lata2dx/build/lata2dx $(TRUST_ROOT)/exec
	cp -f lance_dx lata_get_nb_timesteps.sh clean_trio_data $(TRUST_ROOT)/exec
	(cd lata2dx_reactor;./build_lata2dx_reactor.sh debug)
	cp -f lata2dx_reactor/build/lata2dx_reactor $(TRUST_ROOT)/exec

clean:
	cd lata2dx ; rm -rf build install $(TRUST_ROOT)/exec/lata2dx $(TRUST_ROOT)/exec/compare_lata
	rm -f $(TRUST_ROOT)/exec/lance_dx $(TRUST_ROOT)/exec/lata_get_nb_timesteps.sh $(TRUST_ROOT)/exec/clean_trio_data
	cd lata2dx_reactor ; rm -rf build test/build $(TRUST_ROOT)/exec/lata2dx_reactor
