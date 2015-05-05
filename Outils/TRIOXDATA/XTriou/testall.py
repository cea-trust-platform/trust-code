from test_cas import test_cas
for file in ['Canal_VEF_2D_LongMelange','therce','cinits','prem','trois','Marche','docond']:
	test_cas(file)
	pass
print file
from p_docond import sch
sch2=sch
sch2.tinit=3.
print sch2.print_py()

#print cl
