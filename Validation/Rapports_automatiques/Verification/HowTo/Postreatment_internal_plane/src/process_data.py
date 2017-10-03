#calcul du debit surfacique de reference

import numpy as np
from math import *

file=np.loadtxt('cylindre_tourbillon_pb_Debit.out')
S=pi*0.05**2
debit_ref=file[1]/S

#On stocke le debit de reference dans un fichier
f=open("Debit_ref","w")
f.write(str(debit_ref))
f.close()




