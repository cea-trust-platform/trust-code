from test_cas import test_cas
# same as testall.py but we test prem.data instead of cinits.data, prem.data and trois.data
for file in ['prem']:
    test_cas(file)
    pass
print(file)
from p_prem import sch
sch2=sch
sch2.tinit=3.
print(sch2.print_py())

