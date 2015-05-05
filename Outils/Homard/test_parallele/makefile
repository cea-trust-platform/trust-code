test_clean: clean test
	make clean
test: clean
	sh ./test_parallele 1> Test_homard.log 2>&1
	echo Homard // OK
clean:
	rm  -rf Canal_VEF_3D_SCOTTI Test_homard.log
