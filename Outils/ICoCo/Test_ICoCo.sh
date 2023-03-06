cd ICoCo_src
[ ! -f ./configure ] && baltik_build_configure
./configure
echo "Compiling ICoCo test"
debug=""
[ -f $exec_debug ] && debug="debug module_debug"
#[ -f $exec_semi_opt ] && semi_opt="semi_optim module_semi_optim"
make optim module_optim $debug #$semi_opt
status=$?

exit $status
	
