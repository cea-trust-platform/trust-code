all:	Test_ICoCo.sh
	@echo Compiling ICoCo
	./Test_ICoCo.sh 2>&1 1>LOG
	@echo OK
check:	all
	(cd ICoCo_src; make check_optim)
clean:
	(cd ICoCo_src; make dist_clean|| echo dist_clean not possible)
	rm -f LOG
