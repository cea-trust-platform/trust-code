echo "dimension -2 fin" > test_assert.data

echo "we wait ....  Aborted                 (core dumped) $exec_debug test_assert ...."
$exec_debug test_assert 1>debug.log 2>&1 
grep Assertion debug.log -q || (echo no assert with debug && echo "ASSERT=KO" && exit -1)
$exec test_assert 1>optim.log 2>&1 
grep Assertion optim.log -q && echo assert with optim && echo "ASSERT=KO" && exit -1
echo "ASSERT=OK"

