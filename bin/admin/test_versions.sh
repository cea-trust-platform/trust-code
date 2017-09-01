echo "dimension -2 fin" > test_assert.data

echo "we wait ....  Aborted                 (core dumped) $exec_debug test_assert ...."
USE_MPIRUN=1 exec=$exec_debug trust test_assert 1>debug.log 2>&1 
grep Assertion debug.log -q || (echo no assert with debug && echo "ASSERT=KO" && exit -1)
USE_MPIRUN=1 exec=$exec trust test_assert 1>optim.log 2>&1 
grep Assertion optim.log -q && echo assert with optim && echo "ASSERT=KO" && exit -1
echo "ASSERT=OK"

