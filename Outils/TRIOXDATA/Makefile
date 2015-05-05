IHMTRIO.sh: all Installer  Makefile XDATA.sh XTriou/update_xd
	./Installer TRIOU 1>>TRIOXDATA.log 2>&1

XDATA.sh: Installer Makefile 
	./Installer XDATA 1>>TRIOXDATA.log 2>&1
XTriou/update_xd: all
all:	XDATA.sh
	./Install_all 1>>TRIOXDATA.log 2>&1
check: IHMTRIO.sh 
	./check_jdd
clean:
	@echo "cleaning..."
	rm -rf XDATA   TRIOU/dist TRIOU/build XDATA.sh IHMTRIO.sh *.log
	(cd XTriou; make clean)
