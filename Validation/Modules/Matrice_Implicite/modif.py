from triou import *
# on ajoute porosites_champ
def modif(l,cas):
    new_ll=[]
    for cl in l:
        new_ll.append(cl)
        if (0) and(isinstance(cl,schema_temps_base)and not(isinstance(cl,schema_euler_implicite))):
            new_ll.remove(cl)
            new_sch=change_type(cl,schema_euler_implicite)
            new_sch.solveur=piso(seuil_convergence_solveur=1e-8)
            new_ll.append(new_sch)
            pass
        if (isinstance(cl,Pb_base)):
            for x in cl.__init__xattributes__:
                atr=getattr(cl,x.name)
                if isinstance(atr,eqn_base):
                    #print atr.conditions_limites
                    # print atr.convection.operateur
                    try:
                        conv=atr.convection.operateur
                        if not((isinstance(conv,convection_negligeable)) or (conv.name_trio_=='amont') ):

                            print("on change la convection de ",conv,"  amont")
                            atr.convection.operateur=convection_amont()
                            pass
                        pass
                    except:
                        print("pas de convection ?")
                        pass
                    # print atr.convection.operateur
                    try:
                        diff=atr.diffusion.operateur
                        print(diff)
                        if not((isinstance(diff,diffusion_negligeable)) or (diff==None)):
                            print("on change la diffusion de ",diff,"  a { }")
                            atr.diffusion.operateur=None
                            pass
                        pass
                    except:
                        print("pas de difusion ?")
                        pass


                    pass
                pass
            pass
        pass
    testbidon=system(cmd="\"gunzip -c %s.lml.gz > %s.lml\""%(cas,cas))
    new_ll.insert(len(new_ll)-1,testbidon)
    testbidon=system(cmd="\"cp -f %s.lml  PAR_%s.lml\""%(cas,cas))
    new_ll.insert(len(new_ll)-1,testbidon)
    return new_ll

def test_cas(nom):
    nom2=nom+'.data'
    ll0=read_file_data(nom2)
    write_file_data('trad_'+nom2,ll0)
    new_ll=modif(ll0,nom)
    # print new_ll
    write_file_data('mod_'+nom2,new_ll)

if __name__ == '__main__':
    import sys
    test_cas(sys.argv[1])
    pass
