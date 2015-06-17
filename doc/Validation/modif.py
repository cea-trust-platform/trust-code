from triou import *
def get_const(cl,name):
    mu=getattr(cl,name)
    if (not mu):
        return None
    if (mu):
        return getattr(mu,"val")[0]
    print name,"ici"
    return mu

def modif_fluide(cl,l):
    #print cl.print_lu()
  
    rho=get_const(cl,"rho")
    cp=get_const(cl,"cp")
    lambda_u=get_const(cl,"lambda_u")
    betath=get_const(cl,"beta_th")
    if betath:
        betath=uniform_field(val=betath*rho)
        pass
    Tref=300
    if not cp:
        cp=1000
        pass
    if not lambda_u:
        prdt=1
    else:
        mu=get_const(cl,"mu")
        prdt=mu*cp/lambda_u
        pass
    gam=1.4
    p=rho*Tref*cp*(1.-1./gam)
    if 0:
        flqc=change_type(cl,fluide_quasi_compressible)
        flqc.beta_th=None
        flqc.rho=None
        
        flqc.loi_etat=perfect_gaz(Prandtl=prdt,Cp=cp,gamma=gam,rho_constant_pour_debug=cl.rho)
        flqc.traitement_rho_gravite="moins_rho_moyen"
        flqc.traitement_pth="conservation_masse"
        pass
    if 1:
        flqc=fluide_quasi_compressible(name_u=cl.name_u,
                                       mu=cl.mu,
                                       pression=p,
                                       indice=cl.indice,
                                       kappa=cl.kappa,
                                       #  beta_th=betath,
                                       loi_etat=perfect_gaz(Prandtl=prdt,Cp=cp,gamma=gam,rho_constant_pour_debug=cl.rho),
                                       traitement_rho_gravite="moins_rho_moyen",
                                       traitement_pth="conservation_masse")
        pass
    #print flqc.print_lu()
    return flqc

def modif_keps(eq1,rho):
    print "coucou"
    mod=eq1.modele_turbulence
    if (isinstance(mod,k_epsilon)):
        eq_t=mod.transport_k_epsilon
        ci=mod.transport_k_epsilon.conditions_initiales.condinit
        ci.ch.val[0]*=rho
        ci.ch.val[1]*=rho
        mod.transport_k_epsilon.conditions_initiales.condinit=ci
        cls=mod.transport_k_epsilon.conditions_limites.listobj
        for cla in cls:
            cl=cla.cl
            try:
                cl.ch.val[0]*=rho
                cl.ch.val[1]*=rho
            except:
                print "CL sur K_eps non multiplie par rho ",cl.name_trio_,cla.bord
                pass
            pass
        
        pass
    pass


def cl_thermo(eq_hydro):
    cl_th=[]
    cl_hy=eq_hydro.conditions_limites.listobj
    for cl_li in cl_hy:
        type_cl=cl_li.cl
        if (isinstance(type_cl,symetrie)) or (isinstance(type_cl,periodic)):
            cl_th.append(cl_li)
            pass
        elif isinstance(type_cl,paroi_fixe) or isinstance(type_cl,paroi_defilante):
            cl_th.append(condlimlu(cl=paroi_adiabatique(),bord=cl_li.bord))
        else:
            newcl=condlimlu(cl=frontiere_ouverte(var_name="T_ext",ch=champ_front_uniforme(val=[300.0])),bord=cl_li.bord)
            cl_th.append(newcl)
            pass
        pass
    return cl_th
def changer_seuil_solveur_et_efstab(eq_hydro,rho):
    solv=eq_hydro.solveur_pression
    try:
        solv.seuil*=rho
        print "on modifie le seuil de pression, maintenant: ", solv.seuil , "multiplication par rho",rho
        
    except:
        pass
    op_conv=eq_hydro.convection.operateur
    if isinstance(op_conv,convection_ef_stab):
        op_conv.tdivu='yes'
        pass
    pass
def modif(l):
    new_ll=[]
    for cl in l:
        if (isinstance(cl,fluide_incompressible)):
            print cl,"fluide"
            flqc=modif_fluide(cl,l)
	    new_ll.append(flqc)
            rho=get_const(cl,"rho") # pour calculer Pimp
            pass
        elif  (isinstance(cl,pb_hydraulique) or isinstance(cl,pb_thermohydraulique) ):
            print cl,"pb"
	    eq_hydro=cl.navier_stokes_standard
            cl_hy=eq_hydro.conditions_limites.listobj
            for cl_li in cl_hy:
                if (isinstance(cl_li.cl,frontiere_ouverte_pression_imposee)):
                    # cl_li.cl.ch.val[0]*=rho
                    flqc.traitement_pth="constant"
                    pass
	    eq1=change_type(eq_hydro,navier_stokes_qc)
            changer_seuil_solveur_et_efstab(eq1,rho)
            tutu=change_type(cl,pb_thermohydraulique_qc)
            tutu.navier_stokes_qc=eq1
	    new_ll.append(tutu)
            if isinstance(cl,pb_thermohydraulique) :
                eq_thermo=cl.convection_diffusion_temperature
                eq2=change_type(eq_thermo,convection_diffusion_chaleur_qc)
                
            else:
                # il faut creer l'equation de temp
                eq2=convection_diffusion_chaleur_qc(convection=bloc_convection(aco='{',operateur=convection_negligeable(),acof='}'),diffusion=bloc_diffusion(aco='{',operateur=diffusion_negligeable(),acof='}'))
                eq2.conditions_initiales=condinits(aco='{',condinit=condinit(nom='temperature',ch=uniform_field(val=[300.0])),acof='}')
                cl_th=cl_thermo(eq_hydro)
                
                eq2.conditions_limites=condlims(listobj=cl_th)
                pass
            tutu.convection_diffusion_chaleur_qc=eq2
            pass
        elif  (isinstance(cl,pb_hydraulique_turbulent) or isinstance(cl,pb_thermohydraulique_turbulent) ):
            print cl,"pb"
	    eq_hydro=cl.navier_stokes_turbulent
            cl_hy=eq_hydro.conditions_limites.listobj
            for cl_li in cl_hy:
                if (isinstance(cl_li.cl,frontiere_ouverte_pression_imposee)):
                    cl_li.cl.ch.val[0]*=rho
                    flqc.traitement_pth="constant"
                    pass
	    eq1=change_type(eq_hydro,navier_stokes_turbulent_qc)
            changer_seuil_solveur_et_efstab(eq1,rho)
	    tutu=change_type(cl,pb_thermohydraulique_turbulent_qc)
            tutu.navier_stokes_turbulent_qc=eq1
            # le modele de turbulence K_eps doit etre modifiee
            modif_keps(eq1,rho)
	    new_ll.append(tutu)
            if isinstance(cl,pb_thermohydraulique_turbulent) :
                eq_thermo=cl.convection_diffusion_temperature_turbulent
                eq2=change_type(eq_thermo,convection_diffusion_chaleur_turbulent_qc)
                
            else:
                # il faut creer l'equation de temp
                eq2=convection_diffusion_chaleur_turbulent_qc(modele_turbulence=prandtl(turbulence_paroi=negligeable_scalaire()),convection=bloc_convection(aco='{',operateur=convection_negligeable(),acof='}'),diffusion=bloc_diffusion(aco='{',operateur=diffusion_negligeable(),acof='}'))
                eq2.conditions_initiales=condinits(listobj=[condinit(nom='temperature',ch=uniform_field(val=[300.0]))])
                cl_th=cl_thermo(eq_hydro)
                
                eq2.conditions_limites=condlims(listobj=cl_th)
                pass
            tutu.convection_diffusion_chaleur_turbulent_qc=eq2
            pass
	else:
	    new_ll.append(cl)
	    pass
        pass
    return new_ll

def modif_incomp(ll):
    for cl in ll:
        if  (isinstance(cl,Pb_base)):
            # on regarde si dans le post on a la pression (si oui on met en _pa)
            if 1:
	    
                list_a_enlever=[]
                list_post=[]
                if cl.postraitement!=None:
                    list_post=[cl.postraitement]
                    pass
                if cl.postraitements!=None:
                    for o in  cl.postraitements.listobj:
                        list_post.append(o.post)
                for post in list_post:
                    if post.champs!=None and post.champs.champs!=None:
                        for champ in post.champs.champs.listobj:
                            champ_champ=lower(champ.champ)
                            if champ_champ == "pression":
                                champ.champ="pression_pa"
				print "on modifie la ",champ_champ," en ",champ.champ
                                pass
                            if champ_champ == "k" or \
			       champ_champ == "eps" or \
			       champ_champ == "viscosite_turbulente" or  \
			       champ_champ == "y_plus" or \
			       champ_champ == "pression_pa":
                                print "on retire ", champ.champ
                                list_a_enlever.append(champ)
                                pass
                            pass
                        pass
                    # list_a_enlever=[]
                    for faux in list_a_enlever:
                        post.champs.champs.listobj.remove(faux)
                        pass
                    pass
                pass
            else:
                pass
            name= cl.__init__xattributes__[0].name
            eq1=getattr(cl,name)
            if eq1:
                # print name,eq1
                if hasattr(eq1,"sources") and eq1.sources:
                    print dir(eq1.sources.listobj)
                    for s in eq1.sources.listobj:
                        if isinstance(s,boussinesq_temperature):
                            eq1.sources.listobj.remove(s)
                            print "on retire le terme de boussinesq"
                            pass
                        pass
                    pass
                pass
            pass
        elif isinstance(cl,schema_euler_implicite):
            if isinstance(cl.solveur,simpler):
                print "on passe en piso"
                cl.solveur=change_type(cl.solveur,piso)
                pass
            pass
        
        pass
    return ll
def test_cas(nom):
    nom2=nom+'.data'
    ll0=read_file_data(nom2)
    ll=modif_incomp(ll0)
    write_file_data('mod_'+nom2,ll)
    new_ll=modif(ll)
    # print new_ll
    write_file_data('QC_'+nom2,new_ll)

if __name__ == '__main__':
    import sys
    test_cas(sys.argv[1])
    pass
