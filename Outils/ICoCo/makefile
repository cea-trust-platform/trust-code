moduleopt=ICoCo_src/build/src/exec_opt/_TRUSTModule_opt.so
trusticoco=ICoCo_src/share/swig/install/lib/_trusticoco.so
mpi=$(MPI_INCLUDE)/mpi.h

ifneq ("$(wildcard $(exec_debug))","")
	moduledebug=ICoCo_src/build/src/exec/_TRUSTModule.so
endif

all:	$(moduleopt) $(moduledebug) $(trusticoco)

$(moduleopt): Test_ICoCo.sh makefile $(mpi)
	@echo Compiling ICoCo
	./Test_ICoCo.sh  #1>LOG 2>&1
	@echo OK

$(moduledebug): Test_ICoCo.sh makefile $(mpi)
	@echo Compiling ICoCo
	./Test_ICoCo.sh  #1>LOG 2>&1
	@echo OK

$(trusticoco): ICoCo_src/share/swig/install.sh makefile ICoCo_src/share/swig/trusticoco.i ICoCo_src/share/swig/icocoexceptions.i ${TRUST_MEDCOUPLING_ROOT}/env.sh $(mpi)
	@echo Compiling ICoCo
	(cd ICoCo_src/share/swig; ./install.sh)  #1>LOG 2>&1
	@echo OK

check:	all
	(cd ICoCo_src; make check_optim)
clean:
	(cd ICoCo_src; make dist_clean|| echo dist_clean not possible;rm -f configure)
	(cd ICoCo_src/share/swig; rm -rf build install)
	#rm -f LOG
