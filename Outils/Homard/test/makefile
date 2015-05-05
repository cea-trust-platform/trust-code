test_clean: clean test
	make clean
test: clean
	cp base/* .
	chmod +w *
	sh ./Test_homard 1> Test_homard.log 2>&1
	echo Homard OK
clean:
	rm  -f `find . -maxdepth 1 -type f  -not -name makefile  ` nonexistant 
