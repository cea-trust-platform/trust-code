d=[0,1,2,3]
n=len(d)

def fac(n):
    r=1
    for i in range(n):
        r=r*(i+1)
        pass
    return r
def inv(d,i,n):
    a=d[i]
    j=i+1
    if (j==n):
        j=0
    d[i]=d[j]
    d[j]=a
    return d
for i in range(fac(n)):
    j=i-(i//(n))*(n)
    inv(d,j,n)
 #   print d
    pass
for j1 in range(n):

    for j2 in range(n):
        if (j2!=j1):
            for j3 in range(n):
                if (j3!=j1) and (j3!=j2) :
                    for j4 in range(n):
                        if (j4!=j1) and (j4!=j2) and (j4!=j3):
                            cmd="sed \"s/0 1 2 3 / "+str(j1)+" "+str(j2)+" "+str(j3)+" "+str(j4)+"  /\" mod2.geom  >mod2b.geom;rm -f mod2o.geom; $exec mod2>out 2>&1; [ $? != 0 ] && echo pb ;x=`grep -n Quadrangle mod2o.geom | awk -F: '{print $1}'`; xx=`awk 'BEGIN {print '$x'+5}'`; head -$xx mod2o.geom | tail  -1;"
                            print(cmd)
                            pass
                        pass
                    pass
                pass
            pass
        pass
    pass
