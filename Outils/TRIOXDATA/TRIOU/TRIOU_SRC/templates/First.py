try:
 import rlcompleter, readline
 readline.parse_and_bind('tab:complete')
except:
 pass

import triou
import json
#import for_triou 
def read_template(file):
    ll=triou.read_file_data(file)
    return ll


def find_obj_of_type(aclass,ll):
    for f in ll:
        if isinstance(f,aclass):
            return f
        pass
    return None

    

def find_obj_of_name(name,ll):
    for f in ll:
        if (f.name_u==name):
            return f
        pass
    return None
def change_obj_of_name(name,ll,dis):
    new_t=[]
    trouve=0
    for f in ll:
        if (f.name_u==name):
            dis.name_u=f.name_u
            new_t.append(dis)
            # print "ok",new_t
            trouve=1
        else:
            #print f.name_u,name
            new_t.append(f)
            pass
        pass
    if trouve==0:
        print new_t
        print name
        1/0
    return new_t
            


def trouve_eqn(cl,name0):
    name=name0
    if name=="pressure":
        name="vitesse"
    if name=="velocity":
        name="vitesse"
    if name=="k-epsilon":
        eq_vit=trouve_eqn(cl,"vitesse")
        return eq_vit.modele_turbulence.getTransportKEpsilon()
    for x in cl.__init__xattributes__:
        atr=getattr(cl,x.name)
        if isinstance(atr,triou.eqn_base):
            if (atr.conditions_initiales.condinit.nom==name):
                return atr
            pass
        pass
    raise Exception("No equation for "+name)

def create_cl(eq,inco,clglobal):
    cl=clglobal[inco]
    group=clglobal["group_name"]
    type=clglobal["bc_type"]
    acl=triou.condlimlu(bord=group) 
    if cl=={}:
        # condlim generique
        dico_cl= {"symetry":"symetrie","periodic":"periodic"}
        type_tr=dico_cl[type]
        acl.cl=triou.__dict__[type_tr]()
    elif type=="input":
        if inco=="temperature":
            acl.cl=triou.frontiere_ouverte_temperature_imposee()
        elif inco=="velocity":
            acl.cl=triou.frontiere_ouverte_vitesse_imposee()
        elif inco=="k-epsilon":
            acl.cl=triou.frontiere_ouverte_k_eps_impose()
        else:
            1/0
        acl.cl.setCh(triou.champ_front_uniforme(val=cl))
    elif type=="output":
        if inco=="temperature":
            acl.cl=triou.frontiere_ouverte()
            acl.cl.setVarName("T_ext")
        elif inco=="pressure":
            acl.cl=triou.frontiere_ouverte_pression_imposee()
        elif inco=="k-epsilon":
            acl.cl=triou.frontiere_ouverte()
            acl.cl.setVarName("K_Eps_ext")  
        else:
            print inco
            1/0
        acl.cl.setCh(triou.champ_front_uniforme(val=cl))
        
    else:
        print type,inco,"ici",cl
        # cas particulier
        isdico=1
        try:
            type_cl=cl[type+"_type"]
        except:
            isdico=0
            type_cl=cl
        print "type_tr",type_cl
        dico_cl2={"forced_flux":"paroi_flux_impose","forced_temperature":"paroi_temperature_imposee","adherent":"paroi_fixe","paroi_adiab":"paroi_adiabatique"}
        type_tr=dico_cl2[type_cl]
        acl.cl=triou.__dict__[type_tr]()
        if isdico and "value" in cl.keys():
            acl.cl.setCh(triou.champ_front_uniforme(val=cl["value"]))
        
    return acl





def convert_json_to_jdd(sch,filename):
    from xcontext import setContext,getContext

    #from xmetaclass import resetXClasses,resetXObjects
    #resetXClasses()
    #resetXObjects()
    while (getContext()!="quiet"):
        print "on change de contexte"
        setContext("quiet")
        pass

    
    assert(len(sch.keys())==1)
    type_pb=sch.keys()[0]
    sch=sch[type_pb]
 
    if type_pb=="thermo_hydrau_pb":
        turb_model=sch["turb_model"]
        print "AFAIRE",turb_model
        sch.pop("turb_model")
        if turb_model=="k-epsilon":
            data="templates/upwind_KEPS.data"
        elif turb_model=="Laminar":
            data="templates/upwind.data"
        else:
            raise Exception(not implemented)
    elif type_pb=="solid_pb":
        data="templates/conduc.data"
    else:
        print type_pb, "not supported"
        raise Exception("errror")
        pass
    try:
       f=open(data,"r")
    except:
       import os
       data=os.getenv("TRIOU_ROOT_DIR")+"/"+data
    template=read_template(data)
    
    # print template
    if 0:
        print find_obj_of_type(triou.domaine,template)
        print find_obj_of_name("dom",template)


    pb=find_obj_of_name("pb",template)

    if 0:
        pb2=triou.change_type(pb,triou.pb_hydraulique)
        print "pb2",pb2
        template=change_obj_of_name("pb",template,pb2)
        pb=find_obj_of_name("pb",template)
    
    for cle in sch.keys():
        print "cle",cle
        val=sch[cle]
        # print "val",val
        if cle=="domain":
            dim=find_obj_of_type(triou.dimension,template)
            dim.setDim(val["dimension"])
            fre=find_obj_of_type(triou.readmed,template)
            
            fre.setFile(val["med_file"])
            fre.setNomDomMed(val["mesh_name"])
            geo= val["geo_types"]
            if ((geo==[u'NORM_TETRA4'])or(geo==[u'NORM_TRI3'])):
                dis=triou.vefprep1b()
            elif ((geo==[u'NORM_HEXA8']) or (geo==[u'NORM_QUAD4'])):
                dis=triou.vdf()
            else:
                raise Exception(str(geo)+" not implemented")
            template=change_obj_of_name("dis",template,dis)
            
            #print dis
        elif cle=="medium":
            fluide=find_obj_of_type(triou.milieu_base,template)
            if "preset" in val:
                
                preset=val["preset"]
                if preset=="Water 20 deg":
                    fl={"mu":1e-3,
                        "rho":1000,
                        "cp":4182,
                        "lambda":0.597,
                        "beta_th": 1.8e-4
                        }
                elif preset=="Acier_inconel":
                    fl={
                        "rho":2000,
                        "cp":41,
                        "lambda":13
                        }
                else:
                    raise Exception(str(preset)+" not implemented")
                
            elif "user-defined" in val:
		fl=val["user-defined"]
            else:
                print val
                1/0
            for prop in fl.keys():
                fluide.read_val(["champ_uniforme","1",str(fl[prop])],1,[],prop)
                #print fluide.print_py()
            #fluide.set(triou.uniform_field(val=[mu]))
            #fluide.setRho(triou.uniform_field(val=[rho]))
            # fluide.setCp(triou.uniform_field(val=[cp]))
            #template=change_obj_of_name(fluide.name_u,template,new_fluide)
        elif cle=="init_cond":
            for key in val.keys():
                try:
                    eq=trouve_eqn(pb,key)
                    eq.conditions_initiales.condinit.ch=triou.uniform_field(val=val[key])
                except:
                    print "pb with",key 
        elif cle=="bound_cond":
            first=1
            for cl in val:
                for key in cl.keys():
                    if key=="group_name" or key=="bc_type": continue
                    # print key
                    try:
                        eq=trouve_eqn(pb,key)
                    except:
                        print key,"not found"
                        continue
                        pass
                    if first:
                        eq.conditions_limites=triou.condlims(listobj=[])
                    les_cl=eq.conditions_limites.listobj
                    if 0:
                        print "uu",val
                        print "uuu",key,cl[key]
                    acl=create_cl(eq,key,cl)
                    les_cl.append(acl)
                    
                    # 1/0
                first=0
                pass
        elif cle=="gravity":
            fluide=find_obj_of_type(triou.milieu_base,template)
            name_of_gravity=fluide.gravite
            gravity=find_obj_of_name(name_of_gravity,template)
            gravity.setVal(sch[cle])
            
        else:
            print cle," not implemented"
            pass
        pass
    
    # print template
    triou.write_file_data(filename,template)


if __name__ == '__main__':
    import sys
    file="gauthier.json"
    if len(sys.argv) > 1:
	file=sys.argv[1]
    with open(file) as f:
        sch = json.loads(f.read())  # sch is a Python dic
        convert_json_to_jdd(sch,"essai.data") 
