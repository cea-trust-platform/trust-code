moduleopt=ICoCo_src/build/src/exec_opt/_TRUSTModule_opt.so
trusticoco_optim=ICoCo_src/share/swig/install/lib/_trusticoco.so
mpi=$(MPI_INCLUDE)/mpi.h

ifneq ("$(wildcard $(exec_debug))","")
	moduledebug=ICoCo_src/build/src/exec/_TRUSTModule.so
endif

all:	$(moduleopt) $(moduledebug) $(trusticoco_optim)

$(moduleopt): Test_ICoCo.sh makefile $(mpi) $(exec_opt)
	@echo Compiling ICoCo
	./Test_ICoCo.sh  #1>LOG 2>&1
	@echo OK

$(moduledebug): Test_ICoCo.sh makefile $(mpi) $(exec_debug)
	@echo Compiling ICoCo
	./Test_ICoCo.sh  #1>LOG 2>&1
	@echo OK

$(trusticoco_optim): ICoCo_src/share/swig/makefile makefile ICoCo_src/share/swig/trusticoco.i.in ICoCo_src/share/swig/icocoexceptions.i ${TRUST_MEDCOUPLING_ROOT}/env.sh $(mpi)
	@echo Compiling ICoCo
	(cd ICoCo_src/; make swig_optim)  #1>LOG 2>&1
	@echo OK

check:	all
	(cd ICoCo_src; make check_optim)
clean:
	(cd ICoCo_src; make dist_clean|| echo dist_clean not possible;rm -f configure)
	(cd ICoCo_src/share/swig; make clean)
	#rm -f LOG
