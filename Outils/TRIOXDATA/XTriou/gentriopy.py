from xdata import *

import xdata as xd
#print(dir(xd))

sortie=open('triouint.py','w')
trace=open('trace','w')
sortie2=open('sedfile','w')
sortie3=open('TRAD_1.ver','w')
#sortie.write('from objet_u import *\n')
from string import *
from types import *
#dicoclass={'lire':'read','rien':'rien','floattant':'floattant','entier':'entier','chaine':'chaine'}
#dicoclass={'rien':'rien','floattant':'floattant','entier':'entier','chaine':'chaine'}
dicoclass={'listobj_impl':'listobj_impl'}
listbases=[]
listderives={}

def type_trio_vers_xdata(type_0):
    " conversion des types definis dans les fichiers en type xdata "
    il=type_0.find("default=")
    if (il<0):
        typ=type_0
    else:
        if (type_0[il-1]=="("):
            typ=type_0[:il-1]
        else:
            typ=type_0[:il-1]+')'
            pass
        print("yyyyyyyyy", typ , type_0)
        pass

    i=typ.find("(")
    if i<0:
        if (typ=='floattant'):
            return 'My_Float'
        if (typ=='suppress_param'):
            return 'My_Suppress'
        elif (typ=='entier'):
            return 'My_Int'
        elif (typ=='long'):
            return 'My_Int'
        elif  (typ=='chaine'):
            return 'My_String'
        elif (typ=='list'):
            return 'Listfloat'
        elif (typ=='listchaine'):
            return 'Listchaine'
        elif (typ=='rien'):
            return 'rien'
        elif (typ=="listf"):
            return 'Listfloat_f'
        elif (typ=="listentierf"):
            return 'Listentier_f'
        elif (typ=="listentier"):
            return 'Listentier'
        elif (typ=="listchainef"):
            return 'Listchaine_f'
        else:
            # print typ,typ[:4]
            if (typ[:4]=="ref_"):
                return 'XMulTypes(XNone(),XString(),XInstance(\"triou.'+typ[4:]+'\"))'
            else:
                return 'XInstance('+typ+')'
            pass
            # return 'XInstance(\"triou.'+typ+'\")'
        pass
    else:
        #print "typ special",typ
        typ2=typ[:i]
        if (typ2=='floattant'):
            str='XFloat'
        elif (typ2=='entier'):
            str='XInt'
        elif (typ2=='long'):
            str='XInt'
        elif  (typ2=='chaine'):
            str='XString'
        else :
            print("pas code ",typ,typ2,typ[i:])
            Exception("rate")
            pass
        return str+typ[i:]
        pass

    pass

def gen_class(**args):
    " genere la classe python a partir des arguments"
    print("genclass",args['name'])
    for i in list(range(len(args)))*0:
        print(list(args.keys())[i],list(args.values())[i],type(list(args.values())[i]))
        pass
    class O: pass
    o = O()
        
    o.readacc_=-1
    o.pere_impl_="not_set"
    o.name_u_="not_set"
    o.name_trio_="not_set"
    o.list_ref_=[]
    o.nb_eq_="not_set"
    o.type_="not_set"
    o.class_type_="not_set"
    o.virgule_=-1
    o.descr_="not_set"
    for i in range(len(args)):
        cle=list(args.keys())[i]
        val=list(args.values())[i]
        if type(val) == type("a"):
            cmd="o." + str(cle)+"_=\""+val+"\""
        else:
            cmd="o." +str(cle)+"_="+str(val)
            pass
        exec(cmd, {'o': o})
        pass
    if o.name_trio_=='not_set':
        o.name_trio_=o.name_
        pass
    if o.name_[-6:]=='_deriv' or o.pere_=='class_generic':
        listderives[o.name_]=o.name_
    if o.pere_[-6:]=='_deriv':
        # print "ici", listderives
        pass
    if o.pere_ in list(listderives.keys()):
        listderives[o.name_]=listderives[o.pere_]
        entree=listderives[o.name_].lower()+'___'+o.name_trio_.lower()
        dicoclass[entree]=o.name_
        entree=o.name_.lower()
    elif o.name_trio_!='':
        entree=o.name_trio_.lower()
    else:
        entree=o.name_.lower()
        pass
    if (entree in list(dicoclass.keys())):
        msg=entree+"("+o.name_+") deja dans le dico " +dicoclass[entree]
        raise Exception(msg)
    dicoclass[entree]=o.name_
    # pass
    # print "pere_impl" ,pere_impl_
    list_mot_ex=[]
    list_type_ex=[]
    list_mot_trio_ex=[]
    list_optional_ex=[]
    list_descr_ex=[]

    for mot in o.list_mot_:
        if isinstance(mot, list):
            list_mot_ex.append(mot[0])
            list_type_ex.append(mot[1])

            list_mot_trio_ex.append(mot[2])
            if (len(mot)==3):
                list_optional_ex.append(1)
                list_descr_ex.append('not_set')
            else:
                list_optional_ex.append(mot[3])
                list_descr_ex.append(mot[4])
                pass
        else:
            raise Exception("list mot doit etre une liste")
        pass

    # print list_mot_ex
    if o.name_trio_!='tititiiti':
        name_trio_s=o.name_trio_
        if o.name_trio_=='':
            name_trio_s='nul'
            pass
        descr_s=o.descr_.replace("'","\\'").replace("\t","\\t")
        if (o.pere_=='deriv'):
            sortie3.write(o.name_+' '+o.pere_+' '+name_trio_s+' '+str(o.readacc_)+' '+o.type_+' '+descr_s+'\n')
        elif (o.pere_=='listobj'):
            sortie3.write(o.name_+' '+o.pere_+' '+name_trio_s+' '+str(o.readacc_)+' '+o.class_type_+' '+str(o.virgule_)+' '+descr_s+'\n')
        else:
            sortie3.write(o.name_+' '+o.pere_+' '+name_trio_s+' '+str(o.readacc_)+' '+descr_s+'\n')
            pass
        i=0
        for mot in o.list_mot_:
            sortie3.write('  attr '+list_mot_trio_ex[i]+' '+mot[1]+' '+list_mot_ex[i]+' '+str(list_optional_ex[i])+' '+list_descr_ex[i].replace("'","\\'")+'\n')

            i=i+1
            pass
        # pass
        if (len(o.list_ref_)>0):
            for ref in o.list_ref_:
                sortie3.write('  ref '+ref[0]+' '+ref[1]+'\n')
                pass
            pass
        pass
    o.attdiscr_=-1
    if (o.readacc_<-1):
        o.readacc_=-2-o.readacc_
        o.attdiscr_=1
        pass

    if (o.pere_=='objet_u'):
        listbases.append(o.name_)
        pass
    if o.pere_impl_=="not_set":
        sortie.write('class '+o.name_+'('+o.pere_+'):\n')
    else:
        sortie.write('class '+o.name_+'('+o.pere_impl_+','+o.pere_+'):\n')
        pass
    sortie.write('    name_=\''+o.name_+'\'\n')
    sortie.write('    name_trio_=\''+o.name_trio_+'\'\n')
    sortie.write('    descr_=\"'+o.descr_+'\"\n')
    if o.readacc_!=-1:
        sortie.write('    readacc_='+str(o.readacc_)+'\n')
        pass
    if o.attdiscr_!=-1:
        sortie.write('    attdiscr='+str(o.attdiscr_)+'\n')
    if o.class_type_!="not_set":
        sortie.write('    class_type=\''+o.class_type_+'\'\n')
        if o.virgule_!=-1 and o.virgule_!="-1" :
            if o.virgule_=="0" or  o.virgule_=="1":
                sortie.write('    virgule='+str(o.virgule_)+'\n')
            else:
                sortie.write('    virgule="'+str(o.virgule_)+'"\n')
                pass
            pass
        pass
    trace.write(o.name_+' __init__\n')
    if (o.name_[-5:]=='_base'):
        # sortie.write('    __xabstract__ = 1\n')
        pass
    if ((1) and (len(list_mot_ex)) or (len(o.list_ref_))or (o.pere_=='listobj')):
        sortie.write('    __init__xattributes__ =  ajout_attribute([\n')
        #sortie.write('    __init__xattributes__ =  [\n')
        # pour chaque mot on construit la Tr_Attribut
        for i in range(len(list_mot_ex)):
            trace.write(o.name_+' '+ list_mot_trio_ex[i].lower()+'\n')
            tri='        Tr_Attribute(\"'+list_mot_ex[i]+'\",'
            tri=tri+'xtype='+type_trio_vers_xdata(list_type_ex[i])+','
            mots_trio=list_mot_trio_ex[i].split('|')
            tri=tri+'name_trio=\"'+mots_trio[-1]+'\",'
            tri=tri+'optional='+str(list_optional_ex[i])+','
            tri=tri+'descr_=\"'+list_descr_ex[i]+'\",'
            if (len(mots_trio)>1):
                print(str(mots_trio[:-1]))
                tri+="syno="+str(mots_trio[:-1])+','

            defaultv="None"
            # on regarde si il y a un into et si il est de longueur 1
            if (list_optional_ex[i]==0) and (list_type_ex[i].rfind("into")>-1):
                # print "coucou",list_type_ex[i],list_type_ex[i].rfind("into")
                type_into=list_type_ex[i]
                tt=type_into[type_into.rfind('['):type_into.rfind(']')+1]
                #exec("l=len("+tt+")")  # Py2
                # tt is a string representing a list, so the following should to the same
                l = tt.count(",")+1

                if l==1:
                    print("attribut non optionnel avec une seule valeur possible",list_mot_ex[i],tt)
                    defaultv=tt[1:-1]
                    pass
                pass
            debut_defaut=list_type_ex[i].rfind("default=")
            if (debut_defaut>-1):
                defaultv=list_type_ex[i][debut_defaut+8:-1]
                print("defaultv",defaultv)
                pass
            # tri=tri+'default_value=None),\n'
            tri=tri+'default_value='+defaultv+'),\n'
            sortie.write(tri)
            pass


        if (o.pere_=='listobj'):
            tri='Tr_Attribute(\"listobj\",xtype=XList(sequence=XInstance('+o.class_type_+')),default_value=None),\n'
            # tri='Tr_Attribute(\"listobj\",xtype=XList(sequence=XInstance(\"triou.'+class_type_+'\")),default_value=None),\n'
            sortie.write(tri)
            pass
        sortie.write('        ] , '+o.pere_+'.__init__xattributes__')
        if (len(o.list_ref_)>0):
            sortie.write(' ,[');
            for i  in range(len(o.list_ref_)):
                tri='        Ref_Tr_Attribute(name=\"'+o.list_ref_[i][0]+'\",'
                tri=tri+'type_ref=\''+o.list_ref_[i][1]+'\','
                tri=tri+'xtype='+type_trio_vers_xdata('ref_'+o.list_ref_[i][1])+','
                #tri=tri+'xtype=XMulTypes(XInstance(\'triou.'+list_ref_[i][1]+'\'),'+type_trio_vers_xdata("chaine")+'),'
                tri=tri+'default_value=None),\n'
                #tri=tri+'),\n'
                sortie.write(tri)
                pass
            sortie.write('        ])\n')
        else:
            sortie.write(')\n')
            pass

    # else:
        # sortie.write('    __init__xattributes__ = '+pere_+'.__init__xattributes__\n')
        pass
    sortie.write('    def __init__(self,**args):\n')
    sortie.write('        self.__init2__()\n')
    sortie.write('        return\n')
    sortie.write('    def __init2__(self):\n')
    if o.pere_impl_!="not_set":
        sortie.write('        self.__init_impl__(args)\n')
        pass
    sortie.write('        '+o.pere_+'.__init2__(self)\n')
    #sortie.write('        self.name_=\''+name_+'\'\n')
    #sortie.write('        self.name_trio_=\''+name_trio_+'\'\n')
    sortie.write('        ajout_test(\''+o.name_+'\')\n')
    if o.name_u_!="not_set" and name_u_!="not_used":
        raise Exception ("code a retirer")
        sortie.write('        self.name_u_=\''+o.name_u_+'\'\n')
        pass
    #  if descr_!="not_set" :
#   sortie.write('        self.descr_=\''+descr_+'\'\n')
    #      pass
    if o.nb_eq_!="not_set":
        sortie.write('        self.nb_eq_='+str(o.nb_eq_)+'\n')
        pass
    if o.type_!="not_set":
        sortie.write('        self.type_=\''+o.type_+'\'\n')
        pass
    if o.class_type_!="not_set" and 0:
        sortie.write('        self.class_type=\''+o.class_type_+'\'\n')
        if o.virgule_!=-1:
            sortie.write('        self.virgule='+str(o.virgule_)+'\n')
            pass
        pass
    if ((len(o.list_ref_)>0)and (0)):
        for ref in o.list_ref_:
            if len(ref)==2:
                ref.append('not_set')
                pass
            pass
        sortie.write('        self.list_ref_='+str(o.list_ref_)+'+self.list_ref_\n')
        pass
    if 0:
        sortie.write('        self.list_type_='+str(list_type_ex)+'+self.list_type_\n')
        sortie.write('        self.list_mot_='+str(list_mot_ex)+'+self.list_mot_\n')
        sortie.write('        self.list_mot_trio_='+str(list_mot_trio_ex)+'+self.list_mot_trio_\n')
        # sortie.write('        print name_,\' __init__ \',self.list_mot_, self.list_type_\n')
        sortie.write('        self.set_args(args)\n')
        pass
    sortie.write('        return\n')
    sortie.write('    pass\n\n')
    return
old=0
import sys
args=sys.argv


if (len(args)==2):
    old=atoi(args[1])
    pass

if (old):
    gen_class(name='interprete',pere='objet_u',list_mot=[],readacc=0)
    gen_class(name='listobj',list_mot=[],pere='listobj_impl')
    gen_class(name='deuxmots',name_trio='',pere='objet_u',list_mot=[['format','chaine'],['file','chaine']],readacc=0)
    # sauvegarde
    gen_class(name='Pb_base_impl',pere='objet_u',list_mot=[['postraitement','postraitement'],['postraitements','postraitements'],['sauvegarde','deuxmots'],['reprise','deuxmots']],list_ref=[['domaine','domaine'],['scheme','schema_temps_base'],['milieu','milieu_base']])
    gen_class(name='Pb_base',pere='objet_u',list_mot=[['postraitement','postraitement'],['postraitements','postraitements'],['sauvegarde','deuxmots'],['reprise','deuxmots']],list_ref=[['domaine','domaine'],['scheme','schema_temps_base'],['milieu','milieu_base']])
    # gen_class(name='Pb_base_impl',pere='objet_u',list_mot=[['postraitement','postraitement'],['postraitements','postraitements'],['sauvegarde','chaine'],['reprise','deuxmots']],list_ref=[['domaine','domaine'],['scheme','schema_temps_base'],['milieu','milieu_base']])
    gen_class(name='discrstd',pere='interprete',list_mot=[['pb','chaine'],['dis','chaine']])
    gen_class(name='condinits',pere='listobj',class_type='condinit',list_mot=[],virgule=0)
    gen_class(name='condlims',pere='listobj',class_type='condlimlu',list_mot=[],virgule=0)
    gen_class(name='sources',pere='listobj',class_type='source',list_mot=[])
    gen_class(name='postraitements',pere='listobj',class_type='un_postraitement',list_mot=[])

    # gen_class(name='objet_lecture',pere='objet_u',list_mot=[])
    pass
sortie.close()
# on met apres les objets qui dependent de objet_u_spec
sortie=open('triouint2.py','w')
if (old):
    exec(compile(open('genclold.py', "rb").read(), 'genclold.py', 'exec'))
else:
    #exec(compile(open('gencl.py', "rb").read(), 'gencl.py', 'exec'))
    exec(open("gencl.py").read())
    pass
if (old):
    gen_class(name='bloc_lecture',pere='objet_lecture',name_trio='',list_mot=[['data','chaine']],readacc=0)
    gen_class(name='champs_posts',name_trio='',pere='objet_lecture',list_mot=[['mot','chaine'],['dt_post','floattant'],['data','chaine']],readacc=0,name_u='not_used')
    gen_class(name='postraitement',name_trio='',pere='objet_lecture',list_mot=[['sondes','chaine'],['format','chaine'],['champs','champs_posts'],['statistiques','champs_posts'],['fichier','chaine'],['domaine','chaine'],['statistiques_en_serie','champs_posts'],['interfaces','champs_posts']],name_u='not_used')
    gen_class(name='un_postraitement',name_trio='',pere='objet_lecture',list_mot=[['nom','chaine'],['post','postraitement']],name_u='not_used',readacc=0)
    gen_class(name='condinit',name_trio='',pere='objet_lecture',list_mot=[['nom','chaine'],['ch','field']],readacc=0,name_u='not_used')

    gen_class(name='condlimlu',name_trio='',pere='objet_lecture',list_mot=[['nom_bord','chaine'],['cl','condlim']],readacc=0,name_u='not_used')
    pass
sortie.write('dicoclass=')
#print dicoclass
sortie.write(str(dicoclass))
sortie.write('\n')
#sortie.write('listbases='+str(listbases)+'\n')
sortie.close()
import os
os.system("cat objet_u.py syno.py triouint.py triouint2.py objet_u_spec.py   > triou.py")

sortie2.close()
# on genere dico bases
from triou import *
import types
ll=dir()
#print type(ll)
ll2=[]
#listbases.remove('Pb_base_impl')
#listbases.append('Pb_base')
listbases.append('listobj_impl')
for b in listbases:
    ll2.append([])
    pass
for obj in ll:
    #print obj,type(ll)#eval(triou.obj)
    cmd="es="+obj
    exec(cmd)
    #if type(es) is types.ClassType:
    ob=0
    try:
        ob=issubclass(es,objet_u)
    except:
        pass
    if ob:
        # print es, type(es),
        if ob:
            i=0
            ok=0
            for b in listbases:
                cmd="b2="+b
                exec(cmd)
                if issubclass(es,b2):
                    ll2[i].append(obj)
                    ok=1
                    break
                i=i+1
                pass
            if ok==0:
                print(obj," non classe")
                pass
            pass
        pass
    pass
#print ll2
dico={}
for i in range(len(listbases)):
    cmd="dico1={\'"+listbases[i]+"\':"+str(ll2[i])+"}"
    # cmd=cmd[:len(cmd)-1]
    # cmd=cmd+"}"
    exec(cmd)
    dico.update(dico1)
    pass
#i=dico.keys().index('Pb_base_impl')
#dico.keys[i]='Pb_base'
ll=dico['interprete']
if 'associer' in ll:
    ll.remove('associate')
    ll.remove('read')
    pass
dico['interprete']=ll
#print dico
e=open('dico.py','w')
e.write('dicobases='+str(dico)+'\n\n')
e.close()
# on attend une seconde pour etre sur que triou.py sera correct
os.system("sleep 1")
# classes_utilisateurs.py No such file
#os.system("cat dico.py classes_utilisateurs.py >> triou.py")
os.system("cat dico.py >> triou.py")

cmd="sed \"s/ //g\" sedfile| sed \"s/\[/\\\\\[/g\" | sed \"s/\]/\\\\\]/g\" > sed2"
print(cmd)
os.system(cmd)
os.system("sed -f sed2 gentriopy.py > gentriopy.py2")
print("fingen",old)

sortie3.write('\n')
sortie3.close()
trace.close()
print("FIN gen",old)
