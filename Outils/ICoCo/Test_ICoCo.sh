cd ICoCo_src
[ ! -f ./configure ] && baltik_build_configure
./configure
echo "Compiling ICoCo test"
make module_optim module_debug  
status=$?
[ $status -ne 0 ]  && echo Error
exit $status
	
