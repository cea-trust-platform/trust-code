
from triou import *



def decoup_cas(file,tmax,nb_max,format="binaire"):
    listclass1=read_file_data(file)
    file0="test0_"+file
    write_file_data(file0,listclass1)
    for cl in listclass1:
        if isinstance(cl,schema_temps_base):
            # on modifie schema
            # modif_schema(cl,nb/2)
            sa_tmax=cl.tmax
	    sa_nb_max=cl.nb_pas_dt_max
            #cl.tmax=tmax
	    cl.nb_pas_dt_max=nb_max
        elif isinstance(cl,Pb_base):
            # on modifie les pbs
            #modif_pb(cl,"xyz")
	    if cl.sauvegarde_simple:
                 cl.sauvegarde_simple=format_file(format,file[:-5]+'_'+cl.name_u+format+'.rep')
	    else:
                 cl.sauvegarde=format_file(format,file[:-5]+'_'+cl.name_u+format+'.rep')
		 pass
            pass
        pass
    file1="test_"+file
    write_file_data(file1,listclass1)
    # deuxiem fichier
    for cl in listclass1:
        if isinstance(cl,schema_temps_base):
            # on modifie schema
            cl.tinit=tmax
	    cl.nb_pas_dt_max=sa_nb_max
	    if (sa_nb_max):
		    cl.nb_pas_dt_max-=nb_max
		    pass
            # print cl.print_py()
            cl.tmax=sa_tmax
            cl.dt_start=dt_calc_dt_calc()
        elif isinstance(cl,Pb_base):
            # on modifie les pbs
            #modif_pb(cl,"xyz")
            cl.sauvegarde=None
            cl.sauvegarde_simple=None
            cl.reprise=format_file(format,file[:-5]+'_'+cl.name_u+format+'.rep')
            pass
        pass
    file2="Reprise_"+file
    write_file_data(file2,listclass1)
    pass
def get_tmax(cas):
    from string import split
    f_dt_ev=split(cas,".")[0]
    f_dt_ev+=".dt_ev"
    dt_ev=open(f_dt_ev,'r')
    lignes=dt_ev.readlines()
    nb=len(lignes)
    decal=0
    if (lignes[0][0]=="#"): decal=1
    nb-=decal
    # print nb,int(1+nb/2),lignes, type(lignes[int(1+nb/2)])
    ligne=lignes[int(decal+nb/2)]
    from string import atof
    tmax=atof(split(ligne," ")[0])
    dt_ev.close()
    print " on coupe le calcul a ",tmax, int(nb/2)
    if (tmax==0.):
	    raise Exception("tmax vaut zero")
    return tmax,int(nb/2)
if __name__=='__main__':
    import sys
    print "decoup_cas.py cas.data [format ]"
    cas=sys.argv[1]
    tmax,nb_max=get_tmax(cas)
    
    if len(sys.argv)==3:
        decoup_cas(cas,tmax,nb_max,sys.argv[2])
    else:
        decoup_cas(cas,tmax,nb_max)
        pass
    pass
