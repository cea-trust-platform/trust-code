# on doit tourner le domaine, les CI et les CL en vitesse , la gravite...

def get_dim(listdata):
    from triou import dimension
    for cl in listdata:
        if isinstance(cl,dimension):
            return cl.dim
        pass
    raise ("Dimension pas trouve dans "+str(listdata))
    pass

from math import cos,sin,asin

from string import lower

angle=asin(1)*2.
trans_x="cos("+str(angle)+")*x-sin("+str(angle)+")*y"
trans_y="cos("+str(angle)+")*y+sin("+str(angle)+")*x"

trans_z="z"

inv_trans_x=str(cos(angle))+"*x-"+str(sin(-angle))+"*y"

inv_trans_y=str(cos(angle))+"*y+"+str(sin(-angle))+"*x"

inv_trans_z="z"

def generate_roration(theta,phi,alpha):
    # Transformation 
    # 1. Rotation autour de l'axe Oz d'un angle theta
    # 2. Rotation autour de l'axe Oy d'un angle phi
    # 3. Rotation autour de l'axe Ox d'un angle alpha
    global trans_x,trans_y,trans_z, inv_trans_x,inv_trans_y,inv_trans_z
    trans_x="("+str(cos(phi)*cos(theta))+")*x+("+str(cos(alpha)*sin(theta)-sin(alpha)*sin(phi)*cos(theta))+")*y+("+str(sin(alpha)*sin(theta)+cos(alpha)*sin(phi)*cos(theta))+")*z"
    trans_y="("+str(-cos(phi)*sin(theta))+")*x+("+str(cos(alpha)*cos(theta)+sin(alpha)*sin(phi)*sin(theta))+")*y+("+str(sin(alpha)*cos(theta)-cos(alpha)*sin(phi)*sin(theta))+")*z"
    trans_z="("+str(-sin(phi))+")*x-("+str(sin(alpha)*cos(phi))+")*y+("+str(cos(alpha)*cos(phi))+")*z"
    inv_trans_x="("+str(cos(phi)*cos(theta))+")*x-("+str(cos(phi)*sin(theta))+")*y-("+str(sin(phi))+")*z"
    inv_trans_y="("+str(cos(alpha)*sin(theta)-sin(alpha)*sin(phi)*cos(theta))+")*x+("+str(cos(alpha)*cos(theta)+sin(alpha)*sin(phi)*sin(theta))+")*y-("+str(sin(alpha)*cos(phi))+")*z"
    inv_trans_z="("+str(sin(alpha)*sin(theta)+cos(alpha)*sin(phi)*cos(theta))+")*x+("+str(sin(alpha)*cos(theta)-cos(alpha)*sin(phi)*sin(theta))+")*y+("+str(cos(alpha)*cos(phi))+")*z"
    pass
def modifie_champ_unif(ch):
    try:
        newval=[0]
        x=ch.val[0]
	y=0
        z=0
        if (len(ch.val)>=2):
            y=ch.val[1]
            newval.append(0)
            pass
        if (len(ch.val)==3):
            z=ch.val[2]
            newval.append(0)
            pass
        newval[0]=eval(trans_x)
        if (len(ch.val)>=2):
            newval[1]=eval(trans_y)
	    pass
        if (len(ch.val)==3):
            newval[2]=eval(trans_z)
            pass
        ch.val=newval
        return ch.print_lu()
    except:
        return modifie_champ_non_unif(ch)
    pass
def modifie_champ_non_unif(ch):
    newval=[0]
    # str()) pour champ_xyz
    x="("+str(ch.val[0])+")"
    y="(0)"
    z="(0)"
    if (len(ch.val)>=2):
        y="("+str(ch.val[1])+")"
        newval.append(0)
        pass
    if (len(ch.val)==3):
        z="("+str(ch.val[2])+")"
        newval.append(0)
        pass
    print x,y,z
    from string import replace
    newval[0]=replace(replace(replace(trans_x,"x",x),"y",y),"z",z)
    if (len(ch.val)>=2):
        newval[1]=replace(replace(replace(trans_y,"x",x),"y",y),"z",z)
        pass
    if (len(ch.val)==3):
        newval[2]=replace(replace(replace(trans_y,"x",x),"y",y),"z",z)
        pass
    ch.val=newval
    return ch.print_lu()
def rotate_dom(new_ll,dom):
    from triou import transformer,writemed,readmed
    la_formule=[trans_x,trans_y]
    if (get_dim(new_ll)==3):
        la_formule.append(trans_z)
    tr=transformer(domain_name=dom.name_u,formule=la_formule)
    # on passe par l'ecriture d'un fichier med pour le VDF
    ecr=writemed(nom_dom=dom.name_u,file=dom.name_u+".med")
    lire=readmed(nom_dom=dom.name_u,nom_dom_med=dom.name_u,file=dom.name_u+".med")
    return tr,ecr,lire

def modif(l,nom):
    from triou import domaine,Pb_base,frontiere_ouverte_vitesse_imposee,eqn_base,milieu_base,discretize,comment
    grav_fait=0
    idis=-1
    iscatt=-1
    j=0
    new_ll=[]
    list_rot=[]
    for cl in l:
        j+=1
       
	new_ll.append(cl)
        if (isinstance(cl,discretize)):
            idis=j
            pass
        if (isinstance(cl,comment)):
            print cl.comm[-8:]
            if cl.comm[-12:]=="FIN MAILLAGE":
                iscatt=j
               
            
	if (isinstance(cl,domaine)):
            list_rot.extend(rotate_dom(new_ll,cl))
            pass
        if (isinstance(cl,milieu_base)):
            if (cl.gravite) and (grav_fait==0):
                from triou import trouve_class_list
                gravite=trouve_class_list(cl.gravite,l)
                modifie_champ_unif(gravite)
		grav_fait=1
                pass
            pass

        if (isinstance(cl,Pb_base)):
            for x in cl.__init__xattributes__:
                atr=getattr(cl,x.name)
                if isinstance(atr,eqn_base):
                    #print atr.conditions_limites
		    
                    cond=atr.conditions_initiales.condinit
                    if (lower(cond.nom)=="vitesse"):
                            modifie_champ_unif(cond.ch)
                            pass
                    for condl in atr.conditions_limites.listobj:
                        if isinstance(condl.cl,frontiere_ouverte_vitesse_imposee):
                            print "On modifie la cl ",condl.bord
                            print condl.cl.ch, condl.cl.ch.print_lu()
                            print modifie_champ_unif(condl.cl.ch)
                            pass
                        pass
                    pass
                pass
            
            cl.postraitement.format="lata"
	    if cl.postraitement.champs:
	      cl.postraitement.champs.format="formatte"
	    if cl.postraitement.statistiques:
		    cl.postraitement.statistiques.format="formatte"

            cl.postraitement.fichier="org"
            pass
        pass
    i=0
    if (iscatt!=-1): idis=iscatt
    for tr in list_rot:
        new_ll.insert(idis-1+i,tr)
        i=i+1
        
    from triou import system,fin
    # au cas ou
    if (isinstance(new_ll[-1],fin)):
        new_ll.remove(new_ll[-1])
    csy=system(cmd='"./Rotate_lata org.lata '+nom+'.lata"')
    from triou import lata_to_other
    latato=lata_to_other(format="lml",file=nom+".lata",file_post="NOM_DU_CAS")
    new_ll.append(csy)
    new_ll.append(latato)
    new_ll.append(fin())
    return new_ll

def test_cas(nom):
    from triou import read_file_data,write_file_data
    nom2=nom+'.data'
    ll0=read_file_data(nom2)
    write_file_data('trad_'+nom2,ll0)

    from triou import vdf
    is_vdf=0
    for cl in ll0:
        if (isinstance(cl,vdf)): is_vdf=1
    
    theta=asin(1.)/3.

    #if is_vdf:
    if 1:
        theta=asin(1)
    phi=theta
    alpha=theta
    if (get_dim(ll0)==2):
        phi=0
        alpha=0
        pass
    generate_roration(theta,phi,alpha)

    
    new_ll=modif(ll0,nom)
    # print new_ll
    write_file_data('mod_'+nom2,new_ll)
    # on ecrit la rotation
    f=open("rotate_xyz.py","w")
    f.write('''def corr(x):   
    if (abs(x)<1e-14): x=0
    return x
''')
    f.write('dimension='+str(get_dim(new_ll))+'\n')
    f.write('def rotate(x,y,z):\n')
    f.write('  # theta='+str(theta)+'\n')
    f.write('  # phi='+str(phi)+'\n')
    f.write('  # alpha='+str(alpha)+'\n')
    f.write('  X=corr('+inv_trans_x+')\n')
    f.write('  Y=corr('+inv_trans_y+')\n')
    f.write('  Z=corr('+inv_trans_z+')\n')
    f.write('  return X,Y,Z\n')
    f.close() 
if __name__ == '__main__':
    import sys
    test_cas(sys.argv[1])
    pass
