# Lance la configuration et la compilation de valgrind
valgrind=$(TRUST_ROOT)/exec/valgrind/bin/valgrind
version=3.10.1
all:$(valgrind)
$(valgrind): Installer_valgrind $(TRUST_cc) makefile
	make clean
	./Installer_valgrind $(version)

clean:
	rm -rf $(TRUST_ROOT)/exec/valgrind $(valgrind) valgrind-?.??.?
test_in_place: 
	$(TRUST_CC) -g test_in_place.cpp -g -o test_in_place.exe
	env VALGRIND=1 exec=$(PWD)/test_in_place.exe trust toto 2 
	rm test_in_place.exe tmp_log*


test_isend: 
	$(TRUST_CC) -g test_isend.cpp -g -o test_isend.exe
	env VALGRIND=1 exec=$(PWD)/test_isend.exe trust toto 2 
	rm test_isend.exe tmp_log*

tests: all test_in_place test_isend

