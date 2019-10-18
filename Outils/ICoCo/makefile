all:	Test_ICoCo.sh
	@echo Compiling ICoCo
	./Test_ICoCo.sh  #1>LOG 2>&1
	@echo OK
check:	all
	(cd ICoCo_src; make check_optim)
clean:
	(cd ICoCo_src; make dist_clean|| echo dist_clean not possible;rm -f configure)
	#rm -f LOG
