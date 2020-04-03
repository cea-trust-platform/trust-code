from test_cas import test_cas
#for file in ['Canal_VEF_2D_LongMelange','therce','cinits','prem','trois','Marche','docond']:
#for file in ['therce','cinits','prem','trois','docond']:
for file in ['cinits','prem','trois']:
    test_cas(file)
    pass
print(file)
from p_trois import sch
sch2=sch
sch2.tinit=3.
print(sch2.print_py())

#print cl
