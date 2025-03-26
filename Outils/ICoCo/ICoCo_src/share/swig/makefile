all: swig_optim CMakeLists.txt install.sh icocoexceptions.i trusticoco.i.in

swig_optim : makefile CMakeLists.txt icocoexceptions.i trusticoco.i.in
	@env BUILD_TYPE=Release ./install.sh

swig_ctest:
	@cd build; ctest --output-on-failure
	
swig_debug: makefile CMakeLists.txt install.sh icocoexceptions.i trusticoco.i.in
	@env BUILD_TYPE=Debug ./install.sh

swig_semi_optim: makefile CMakeLists.txt install.sh icocoexceptions.i trusticoco.i.in
	@env BUILD_TYPE=semi_opt ./install.sh

clean:
	@rm -rf install/ build/

.PHONY: clean swig_optim swig_debug swig_semi_optim swig_ctest all

