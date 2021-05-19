cd ICoCo_src
[ ! -f ./configure ] && baltik_build_configure
./configure
echo "Compiling ICoCo test"
debug=""
[ -f $exec_debug ]  && debug="module_debug"
make module_optim $debug
status=$?
if [ $status -eq 0 ]
then
   make ctest_optim
   status=$?
else
   echo "Error while compiling modules"
fi

[ $status -ne 0 ] && echo "Error while ctesting ICoCo"

exit $status
	
