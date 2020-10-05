test:
	[ ! -d prov_sol ] && echo detruire prov_sol rm -rf prov_sol
	 mkdir prov_sol
	(cp `find . -type f -maxdepth 1` prov_sol && cd prov_sol &&  ln -s testP testP_S &&	ln -s testP testP_VEF&& ln -s testP testP_S_VEF&&	 ./testP $(exec) &&	 ./testP_S $(exec) &&	 ./testP_VEF $(exec) && ./testP_S_VEF $(exec) )
