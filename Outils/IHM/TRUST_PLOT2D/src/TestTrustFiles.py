try:
    import rlcompleter, readline
    readline.parse_and_bind('tab:complete')
except:
    pass


def norme_L2(x):
    invn=1./x.size
    from math import sqrt
    nx=sqrt((x*x).sum()*invn)
    return nx

def ecart2(aa,ref,fout=None):
    x1=aa[0]
    x2=ref[0]
    import numpy
    c=numpy.hstack((x1,x2))
    # c.sort() inutil fait par unique
    new_x= numpy.unique(c)
    from numpy import interp
    a=interp(new_x,aa[0],aa[1])
    b=interp(new_x,ref[0],ref[1])

    na=norme_L2(a)
    nb=norme_L2(b)
    delta=abs(b-a)
    nd=norme_L2(delta)

    print("Ecart max",delta.max(),"Ecart max relatif au delta",delta.max()/ (0.5*(b.max()-b.min()+a.max()-a.min())+1e-10))
    print("norme L2 ecart",nd,"ecart relatif en L2",(nd)/(0.5/(na+nb+1e-10)))
    if fout is not None:
        print("Wrinting in ",fout," x, new_inetr,ref_inter")
        numpy.savetxt(fout,numpy.column_stack([new_x,a,b])) # ,header="essai")
        pass
    pass

if __name__=='__main__':
    from trust_plot2d import TrustFiles
    ref=TrustFiles.CSVFile("Cas_1_Phi",2)
    #print ref.getEntries()
    refv=ref.getValues("c1")

    f=TrustFiles.SonSEGFile("prem_0_PHI.son",None)
    # print dir(f)
    res=f.getValues(f.getEntriesSeg()[0])
    ecart2(res,refv,"test3_0")
