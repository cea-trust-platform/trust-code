name=valgrind
version=3.15.0
install=$(TRUST_ROOT)/exec/$(name)
cible=$(install)/bin/$(name)
all:$(cible)
$(cible): Installer_valgrind $(TRUST_cc) makefile
	make clean
	@./Installer_valgrind $(version) #1>$(install).log 2>&1

clean:
	rm -r -f $(install)* $(name)-*.*.*

test_in_place: 
	$(TRUST_CC) -g test_in_place.cpp -g -o test_in_place.exe
	env VALGRIND=1 exec=$(PWD)/test_in_place.exe trust toto 2 
	rm test_in_place.exe tmp_log*

test_isend: 
	$(TRUST_CC) -g test_isend.cpp -g -o test_isend.exe
	env VALGRIND=1 exec=$(PWD)/test_isend.exe trust toto 2 
	rm test_isend.exe tmp_log*

tests: all test_in_place test_isend
