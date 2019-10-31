#!/usr/bin/python
# -*- coding: latin-1 -*-

from xdata import getXClasses
from triou import dicobases,objet_u
print getXClasses()
#module='triou'

def cree_menu(module,racine,filtre=0):
    dico={}
    for obj in getXClasses():
        #print "DEBUG--> 1:",obj, module, racine
        if issubclass(obj,racine) :
            name_obj = obj.__name__
            #print "DEBUG--> 2:",name_obj
            if not(dico.has_key(name_obj)) :
                dico[name_obj] = []
                pass
            #m_name = obj.__module__
            #print "DEBUG--> 3:",m_name
            #if m_name != module:
            #    continue
            name_pere = obj.__bases__[0].__name__
            #print "DEBUG--> 4:",name_pere
            # name_pere=str(obj.__bases__[0])[lenprefix:-len("'>")]
            if (dico.has_key(name_pere)) :
                l=dico[name_pere]
                l.append(name_obj)
                dico[name_pere]=l
            else:
                dico[name_pere]=[name_obj]
                pass
            pass
        pass


    print dico
    # on a les classes filles on genere les menus:
    for cl in dico.keys():
        if len(dico[cl])>0:
            print cl
            file="menu/menu_"+cl+".py"
            f=open(file,'w')
            f.write('__xdata__name__ = "'+cl+' ..."\n')
            f.write('__xdata__items__ = [\n')
            # f.write('    "'+cl+'",\n')
            stri=""
            f.write('    "'+cl+'",\n')
            lcl=[]
            for cl2 in dico[cl]:
                lcl.append(cl2)
                pass
            lcl.sort()
            for classe_fille in lcl:
                stri=stri+'from '+module+' import '+classe_fille+'\n'
                if len(dico[classe_fille])>0:
                    f.write('    "menu_'+classe_fille+'",\n')
                    pass
                else:
                    f.write('    "'+classe_fille+'",\n')
                    pass
                pass
            f.write('    ]\n')
            f.write('from '+module+' import '+cl+'\n')
            f.write(stri)
            f.close()
            pass
        pass
    pass
import os
os.system("mkdir -p menu")
cree_menu('triou',objet_u)
#cree_menu('aeroproblem',AeroObject)
#cree_menu('aeroproblem',MyPhysicalquantity)
