SHELL=/bin/bash
test:
	cp ../tests_src/* .;chmod +w *
	if [ -d $(TRUST_ROOT)/src/ThSol ] ; then  (export PYTHONPATH=`pwd`:`pwd`/..:${PYTHONPATH};export XDATA_NO_XINTERCEPTOR=1;touch cinits.py p_prem.py p_trois.py;time python ../testall.py) ; fi
	if [ ! -d $(TRUST_ROOT)/src/ThSol ] ; then  (export PYTHONPATH=`pwd`:`pwd`/..:${PYTHONPATH};export XDATA_NO_XINTERCEPTOR=1;touch cinits.py p_prem.py p_trois.py;time python ../testall_Kernel.py) ; fi

clean:
	rm  `find . -maxdepth 1 -type f` ; cp ../tests_src/makefile .; chmod +w makefile
	rm -rf ref


fast:
	mkdir -p ref		
	(cd ref;export PYTHONPATH=`pwd`:${PYTHONPATH};export XDATA_NO_XINTERCEPTOR=1;time python ../../test_test.py -fast `( cd $(TRUST_TESTS)/; find . -name '*.lml.gz' -follow -exec basename {} .lml.gz \; )` > ../err)
	./traite_err
test_ref:
	mkdir -p ref		
	(cd ref;export PYTHONPATH=`pwd`:${PYTHONPATH};export XDATA_NO_XINTERCEPTOR=1;time python ../../test_test.py `( cd $(TRUST_TESTS)/; find . -name '*.lml.gz' -follow -exec basename {} .lml.gz \; )` > ../err)
	./traite_err
test_ref_trace:
	mkdir -p ref		
	(cd ref;export PYTHONPATH=`pwd`:${PYTHONPATH};export TRACE=ON;export XDATA_NO_XINTERCEPTOR=1;time python ../../test_test.py `( cd $(TRUST_TESTS)/; find . -name '*.lml.gz' -follow -exec basename {} .lml.gz \; )` > ../err)
	./traite_err

rate:   
	(cd ..;make)
	mkdir -p ref		
	cp cas_rate ref/rate
	(cd ref;export PYTHONPATH=`pwd`:${PYTHONPATH}; export TRACE="ON";export XDATA_NO_XINTERCEPTOR=1; time python ../../test_test.py `cat rate`  > ../err2)
	./traite_err rate

