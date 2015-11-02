cd ICoCo_src
[ ! -f ./configure ] && baltik_build_configure
./configure
echo "Compiling ICoCo test"
debug=""
[ -f $exec_debug ]  && debug="module_debug"
make module_optim $debug
status=$?
[ $status -ne 0 ]  && echo Error
exit $status
	
