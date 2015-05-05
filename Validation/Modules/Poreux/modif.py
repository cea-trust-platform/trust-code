from triou import *
# on ajoute porosites_champ 
def modif(l):
    new_ll=[]
    for cl in l:
	new_ll.append(cl)
	if (isinstance(cl,schema_temps_base)):
            if cl.seuil_statio:
                cl.seuil_statio/=0.5
                pass
            pass
        if (isinstance(cl,Pb_base)):
	    porosite_champ=porosites_champ(pb=cl.name_u,ch=uniform_field(val=0.5))
	    new_ll.append(porosite_champ)
            for x in cl.__init__xattributes__:
                atr=getattr(cl,x.name)
                if isinstance(atr,eqn_base):
                    #print atr.conditions_limites
                    for condl in atr.conditions_limites.listobj:
                        if isinstance(condl.cl,paroi_flux_impose):
                            print "On modifie la cl ",condl.bord
                            condl.cl.ch.val[0]= condl.cl.ch.val[0]*0.5
                            pass
                        pass
                    pass
                pass
            pass
        pass
    return new_ll

def test_cas(nom):
    nom2=nom+'.data'
    ll0=read_file_data(nom2)
    write_file_data('trad_'+nom2,ll0)
    new_ll=modif(ll0)
    # print new_ll
    write_file_data('mod_'+nom2,new_ll)

if __name__ == '__main__':
    import sys
    test_cas(sys.argv[1])
    pass
