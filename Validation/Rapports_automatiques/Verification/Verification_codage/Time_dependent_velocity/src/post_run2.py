import numpy as np

lines = np.loadtxt("test_VITESSE_U.son", comments="#", delimiter=" ", unpack=False)

time=[]
uan=[]
err=[]
for i in range(len(lines)):
    time.append(0)
    uan.append(0)
    err.append(0)

for n in range(0,len(lines)):
    time[n]=lines[n,0]
    uan[n]=2*lines[n,0]*lines[n,0]+0.1
    err[n]=abs((lines[n,2]-uan[n])/uan[n])

print(n, time[len(lines)-1], max(err[:]))
s=[(n, time[len(lines)-1], max(err[:]))]


with open('max.txt', 'w+') as datafile_:
    np.savetxt(datafile_, s, fmt=['%d','%1.2f','%1.1e'])
