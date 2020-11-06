from math import log, sqrt, fabs
import sys

f = open(sys.argv[1], 'r')
decal = 0
if (len(sys.argv) > 2):
    decal = eval(sys.argv[2])
nbv = 0
a = {}
# print dir(f)
while (1):
    l = f.readline()
    if l == "": break
    l = l.split()
#    print i,l

    a[nbv] = [eval(l[0]) * 1., eval(l[1]) * 1.]
    nbv += 1
    pass

f0 = a[decal+0][1]
f1 = a[decal+1][1]
f2 = a[decal+2][1]
h0 = a[decal+0][0]
h1 = a[decal+1][0]
h2 = a[decal+2][0]
#print f1,f2,h1,h2

def test_b(h0,h1,h2,f0,f1,f2,e0,e2,x):
    if (x<0) or (x==1):
        return []
    # r^b= x
    b=log(x)/log(h2/h1)
    #print x,b
    #f1-f2=A*h1^b*(1-e2*x)
    A=(f1-f2)/(h1**b-e2*h2**b)
    sol=f1-A*h1**b
    test=fabs(f0-sol)-fabs(e0*A*h0**b)
    # print e0,e2,b,test, sol,A,e0*A*h0**b
    if test!=0:
        return []
    return [sol,A,b,e0,e2]
# f2-f1 = A h1^b *((h2/h1)^b -1)
# f0 ...

# (f2-f1)/(f0-f1)=((h2/h1)^b -1)/ ((h0/h1)^b -1)
#






print(sqrt(h2/h0))


'''
f0-sol=e0*A*h0^b
f1-sol=A*h1^b
f2-sol=e2*A*h2^b

e0=+-1

h0=h1/r
h2=h1*r
x=r^b


f1-f0=A*h1^b*(1-e0/x)
f1-f2=A*h1^b*(1-e2*x)

(f1-f0)/(f1-f2)=(1-e0/x)/(1-e2*x)

-> 2 solutions au + pour x ....

on regarde combin de solution x>0

puis on calcule A par  f1-f2
puis sol par f1-sol

puis on on verifie que f0-sol=e0*A*h0^b

on, fait cela pour e0 -+1 et e2 +-1
'''
for e0 in [-1,1]:
    for e2 in [-1,1]:
        det=(4*f1**2*e0*e2+(-(4*f1))*e0*e2*f0+(-(4*f1))*e0*e2*f2+4*e0*e2*f0*f2+f0**2+(-(2*f0))*f2+f2**2)

        if (det>=0):
            det=sqrt(det)
            inr=[-1,1]
            if (det==0): inr=[1]
            for i in inr:
                b=(-f0+f2+i*det)/(2*f1*e2+(-(2*e2))*f0)
                sol=test_b(h0,h1,h2,f0,f1,f2,e0,e2,b)
                if len(sol):
                    print('#',sol[0],'+(',sol[1],')*x**(',sol[2],')', " E0 ",e0," E2", e2)
                pass
            pass
        pass
    pass
pass


'''
)/(2*f1*e2+(-(2*e2))*f0),(-f0+f2-sqrt(4*f1^2*e0*e2+(-(4*f1))*e0*e2*f0+(-(4*f1))*e0*e2*f2+4*e0*e2*f0*f2+f0^2+(-(2*f0))*f2+f2^2))/(2*f1*e2+(-(2*e2))*f0)
sol:(-f0+f2+sqrt(4*f1^2*e0*e2+(-(4*f1))*e0*e2*f0+(-(4*f1))*e0*e2*f2+4*e0*e2*f0*f2+f0^2+(-(2*f0))*f2+f2^2))/(2*f1*e2+(-(2*e2))*f0)

,(-f0+f2-sqrt(4*f1^2*e0*e2+(-(4*f1))*e0*e2*f0+(-(4*f1))*e0*e2*f2+4*e0*e2*f0*f2+f0^2+(-(2*f0))*f2+f2^2))/(2*f1*e2+(-(2*e2))*f0)

(
'''
#print 'oiii',(f2-f1)/(f0-f1),
b=log(fabs(-(f2-f1)/(f0-f1)))/log(sqrt(h2/h0))
print(b)
print((f2+f0), (h2**b+h0**b))
if ((f2-f1)*(f1-f0))>0:
    A=(f2-f0)/(h2**b-h0**b)
    sol=f1-A*h1**b
else:
    A=(f2-f1)/(h2**b+h1**b)
    sol=f1-A*h1**b
    print("iiiii",f1,A*h1**b,sol)
print("A",A)

#print f2-f1, A*h2**b-A*h1**b
#print f0-f1, A*h0**b-A*h1**b


print("#sol",sol,"+",A ,"*x**(",b ,")")
if 1:
    for j in range(nbv):
        inter=sol+A*(a[j][0])**b
        print(a[j][0],a[j][1],inter,a[j][1]-inter,1-inter/a[j][1])
