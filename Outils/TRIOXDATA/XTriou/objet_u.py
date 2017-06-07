#!/usr/bin/python
# -*- coding: latin-1 -*-


# Verifier que les mots ajoutes ne sont pas dans le dictionnaire !!!!
# Sinon provoquer une erreur
# voir a ecriture pour mettre les synonymes mais lequel !!!

from xtree import XTree
#
# Redefinition of saveAs xtree -> ajout de la liste des instances
#
XTree.saveAs_org=XTree.saveAs
def mySave(self,fileName):
    " pour surcharger save de l'ihm xdata, ajout de list_instance"
    from Menu_solver import linkref
    linkref(1)
    self.saveAs_org(fileName)
    from xtree import getMainXTree
    list=getMainXTree().children
    str='list_instance=['
    from xobject import XObject
    for l in list:
        if isinstance(l.node, XObject):
            str+=l.name+','
            pass
        pass
    str+=']\n'
    #print str
    stream = file(fileName, "aw")
    stream.write(str)
    stream.close()
    pass
XTree.saveAs=mySave
#
#
#
# Modification de exit_code -> creation du fichier __sa__.py
#
#

file_name_import=""
is_import_file=0

try:
    from xpythongui import XGUIQMainWindow
    XGUIQMainWindow.exit_Org=XGUIQMainWindow.exit_code
    def myexit(self,*args,**kargs):
        global file_name_import
        fileName="__sa__"+file_name_import
        try:
            xtree=self.xtree
            print "sauvegarde de secours"
            self.xtree.saveAs(fileName)
        except:
            pass
        self.exit_Org()
        return
    XGUIQMainWindow.exit_code=myexit
    # print "ok exit changer"
    from xdatagui import XGUIQProgressDialog
    XGUIQProgressDialog.importPythonFile_Org=XGUIQProgressDialog.importPythonFile
    def myImport(self,*args,**kargs):
        global is_import_file,file_name_import
        is_import_file=0
        from xcontext import setContext,getContext
        context=getContext()
        while (getContext()!="quiet"):
           # print "on change de contexte",getContext()
           setContext("quiet")
	   pass
        file_name_import=args[0]
        self.importPythonFile_Org(*args,**kargs)
        is_import_file=0
        setContext(context)
        from Menu_solver import verifyihm
        verifyihm()
        return
    XGUIQProgressDialog.importPythonFile=myImport
    # print "ok open"
    pass
except:
    pass



def get_dimension(list_cas):
    for i in range(len(list_cas)-1,-1,-1):
        cl=list_cas[i]
        if (isinstance(cl,dimension)):
            dim=cl.dim
            break
        pass
    return dim
#
#
#
#  Modification de ObjectEplorerItem -> name_u pas affiche 
#  si pas a la racine ou si on a un interprete
#    pas affiche non plus les attributs optionnonels a None
#
try:
    from xdataobjectexplorer import ObjectExplorerItem
    ObjectExplorerItem.initsa=ObjectExplorerItem.__init__
    def myinit(self,parent,after,xtree):
        # print "myinit"
        
        if xtree.name=="name_u":
            #print "ok"
            if (parent.xtree.parent.parent is not None) or isinstance(parent.node,interprete) or isinstance(parent.node,comment):
                # print "on ne veut pas de name_u"
                self.xtree=None
                return
                pass
            # print xtree.name
            pass
        not_ok=0
        if xtree.node is None:
            cls=parent.node.__class__
            # print cls
            xatt=cls.getAllInitXAttributes()
            attr=None
            for attr in xatt:
                if attr.name==xtree.name:
                    break
                pass
            if isinstance(attr,Tr_Attribute) and (attr.optional==0):
                not_ok=1 # self.xtree=xtree
               
            else:
                self.xtree=None
                return
                pass
            pass
        self.initsa(parent,after,xtree)
        if isinstance(xtree.node,objet_u):
            self.status=xtree.node.status
        else:
            if isinstance(xtree.parent.node,objet_u):
                if xtree.name in xtree.parent.node.test_valid.keys():
                    if (xtree.parent.node.test_valid[xtree.name]!="Valid"):
                        not_ok=1
                        pass
                    pass
                else:
                    # print "PB",xtree.name,xtree.parent.node
                    pass
            if not_ok:
                self.status="NoValid"
                pass
            pass
        return
    
    ObjectExplorerItem.__init__=myinit

    # surcharge de ObjectExplorer  verificationihm appele
    # print "surcharge de ObjectExplorerOK"
    
    
    from xmainwindow import getMainWindow
    cls = getMainWindow().__class__
    updateObjectBrowser_sa=cls.updateObjectBrowser
    def my_updateObjectBrowser(self):
        from Menu_solver import verifyihm
        from xcontext import getContext
 
        if (getContext()!="quiet"): 
            verifyihm()
        return updateObjectBrowser_sa(self)
    cls.updateObjectBrowser=my_updateObjectBrowser
    

except:
    pass
from xdata import XMulTypes,XNone,XObject,XList,XInt,XFloat,XString,XAttribute,XInstance,getXClasses
from string import replace,split,lower,atoi
from types import InstanceType,StringType
from xutilities import value2text

class suppress_param(XObject):
    name_='suppress_param'
    pass
My_Suppress=XMulTypes(XNone(),XInstance(suppress_param))



def ajout_attribute(list_attr,list_pere,list_ref=[]):
    # on renvoie une liste contenant les attr de list_attr de liste_pere (sans ceux ayant un nom de list_attr et de liste_ref) et de liste_ref
    list_ret=list_attr
    list_name_attr_ref=[]
    list_sup=[]
    
    for attr in list_attr:
        list_name_attr_ref.append(attr.name)
        if attr.xtype==My_Suppress:
            list_sup.append(attr)
        pass
    for ref in list_ref:
        list_name_attr_ref.append(ref.name)
        pass
    for attr in list_pere:
        if attr.name not in list_name_attr_ref:
            list_ret.append(attr)
        else:
            # print attr.name ,"deja dans ", list_name_attr_ref
            pass
        pass
    for sup in list_sup:
        # print sup.name," remove"
        list_ret.remove(sup)
    list_ret+=list_ref
    return list_ret

def test_accolade(mot):
    " appele par read_data"
    if (mot!='{') and (mot!='}'):
        message="On attendait { ou } et non "+mot
        print message
        raise Exception(message)
    pass

# Affichage des traces si $TRACE=ON
trace_active=0
import  os
res=os.getenv("TRACE")
if res=="ON":
	trace_active=1
	pass

    
def traite_trace(cls,mottest,mot):
    "traite les traces"
    if (trace_active): 
        if hasattr(cls.__bases__[0],mot):
            traite_trace(cls.__bases__[0],mottest,mot)
        else:
	    if cls.readacc_==0:
                print "read_val",cls.name_,lower(mottest)
	    else:
		print "read_val_spec",cls.name_,lower(mottest)
		pass

            pass
        pass
    pass

lutil=[]
# marquage des objets utilises
def ajout_test(titi):
    if (trace_active):
        print "read_val",titi,'__init__'
        pass
    if not titi in lutil:
        lutil.append(titi)
        pass
    pass



###################

# definition des types de base ppour Trio

Listfloat = XMulTypes(XNone(), XList(sequence=XFloat()))
Listchaine = XMulTypes(XNone(), XList(sequence=XString()))
My_String=XMulTypes(XNone(), XString())
My_Int=XMulTypes(XNone(), XInt())
My_Float=XMulTypes(XNone(), XFloat())

rien=XMulTypes(XNone(), XString(into=["yes"]))

Listfloat_f = XMulTypes(XNone(), XList(sequence=XFloat(),len_min=2,len_max=3))
Listchaine_f = XMulTypes(XNone(), XList(sequence=XString(),len_min=2,len_max=3))
Listentier_f = XMulTypes(XNone(), XList(sequence=XInt(),len_min=2,len_max=3))
Listentier = XMulTypes(XNone(), XList(sequence=XInt()))


class Tr_Attribute(XAttribute):
    __init__xattributes__ = XAttribute.__init__xattributes__ + [
         XAttribute("optional", xtype=XInt(into=[0,1]),default_value=1),
      #  XAttribute("comment", xtype=XString(), default_value=""),
        XAttribute("descr_", xtype=XString(), default_value="not_set"),
        XAttribute("name_trio", xtype=My_String, default_value=""),
        XAttribute("syno", xtype=Listchaine, default_value=[]),
       
        ]

class Ref_Tr_Attribute(XAttribute):
    __init__xattributes__ = XAttribute.__init__xattributes__ +  [
        #XAttribute("name", xtype=XString()),
        XAttribute("descr_", xtype=XString(), default_value="not_set"),
        XAttribute("type_ref", xtype=My_String,default_value=None),
        ]
        
    pass



#############

def verify_ref(val,attr,listclass):
    status="Valid"
    # on verifie le type de l'objet reference
    if type(val) is StringType:
        try:
            val2=trouve_class_list(val,listclass)
            x=attr.xtype
            x(val2)
        except:
            msg="Pb avec la ref "+attr.name+" de nom "+val
            val.status="NoValid"
            status="NoValid"
            raise Exception(msg)
        
        pass
    #setattr(self,attr.name,val2)
    #print self,val,val2
    #setattr(self,attr.name,val)
    return status


def decoupe_str(str):
    nstr=str
    i=1	
    taille=len(str)
    bloc=81
    deb=0
    i=1
    while (( taille-deb>bloc)and (i==1)):
        point=0
        for j in range(deb+1,deb+bloc):
            if nstr[j]=='\n':
                deb=j
                point=1
                break
            pass
        if point==0:
    
            i=0
            for j in range(deb+bloc,taille):
                if nstr[j]==" ":
                    nstr=nstr[0:j]+'\n'+nstr[j+1:taille]
                    i=1
                    deb=j
                    j=taille
                    break
                pass
            pass
        pass
    return nstr


def isAttributeLink(attr):
    try:
        type_val=attr.xtype
        if (len(type_val.xtypes)==3)and isinstance(type_val.xtypes[1],XString):
            return 1
        else:
            return 0
        pass
    except:
        return 0
    pass

def getNameEn(self):
    return  self.__class__.name_
def getNameTrio(self):
    return  self.__class__.name_trio_

# synonyme={'uniform_field':'champ_uniforme'}
def get_class_name(mot):
    m=lower(mot)
    try:
        return dicoclass[m]
    except KeyError:
        return get_class_name(synonyme[m])


class objet_u(XObject):
    def __new__(cls,*args,**kwargs):
        self=XObject.__new__(cls) # ,*args,**kwargs)
        self.status="Valid"
        self.test_valid={}
	ajout_test('objet_u')
        return self
    attdiscr=0
    name_='objet_u'
    name_trio_='objet_u'
    descr_="not_set"
    readacc_=1
    __init__xattributes__=[
        XAttribute("name_u", xtype=My_String, default_value="not_set"),
        XAttribute("is_read", xtype=XInt(), default_value=0),
        ]
    def _getToolTip(cls, target):
        try:
            tr=target.descr_
	    tr=tr.replace("NL1",'\n')
	    tr=tr.replace("NL2",'\n')
	    tr=tr.replace('. ','.\n')
	    tr=tr.replace('\n\n','\n')
            if (tr!='not_set'):
                tr=decoupe_str(tr)
                return tr
        except:
            pass
        try:
            tr=target.name_
        except:
            tr=target.name
            pass
        return tr
    getToolTip = classmethod(_getToolTip)
    def _show_item(cls,name,object_to_edit):
	""" definit les couleurs quand on edit """
        xatt=cls.getAllInitXAttributes()
        attr=None
        for attr in xatt:
            if attr.name==name:
                break
            pass
	# ok == 1 si tout va bien 0 , si optionnel ou si objet non valid
	# a faire le pb des ref 

	ok=1
        val=None
        if object_to_edit:
            val=getattr(object_to_edit,name)
            if (isinstance(attr,Tr_Attribute)):
                if attr.optional==0:
                    if val is None:
                        ok=0
                        pass
                    pass
		# print val,dir(val)
                if (isinstance(val,objet_u)):
                    if val.status!="Valid":
                        ok=0
                pass
            pass
        if object_to_edit:
            if name in object_to_edit.test_valid.keys():
                if object_to_edit.test_valid[name]!="Valid":
                    ok=0
                    pass
                pass
            pass
        if ok:
            return 1,None
        else:
            return 1,"Red"
        pass
    show_item = classmethod(_show_item)
   
    def myCustomPopup(cls, parent, popup, selection):
        if len(selection) > 1: return
        self = selection[0]
        from changetotypeqpopupmenu import ChangeToTypeQPopupMenu
        instance_popup_menu = ChangeToTypeQPopupMenu(parent, popup, cls, self)
        popup.insertItem("Change to type", instance_popup_menu)
        return
    
    customPopup = classmethod(myCustomPopup)
    
    def __init2__(self):
        self.status="Valid"
        #self.name_u="not_set"
        self.mode_ecr=1
        # self.is_read=0 
	self.listattr=None
        # print "ici args",args,self.__class__
        # print self,"cree"
        return
    def get_list_ref(self):
        xatt=self.__class__.getAllInitXAttributes()
        list=[]
        for xprop in xatt:
            if isinstance(xprop,Ref_Tr_Attribute):
                list.append(xprop)
                pass
            pass
        return list
    def get_list_attr(self):
	if self.listattr==None:
            xatt=self.__class__.getAllInitXAttributes()
            list=[]
            for xprop in xatt:
                if isinstance(xprop,Tr_Attribute):
                    list.append(xprop)
                    pass
                pass
            self.listattr=list
            pass
        return self.listattr
    def get_list_mot_trio(self):
        list=[ x.name_trio for x in self.get_list_attr() ]
        return list
    def get_list_mot(self):
        list=[ x.name for x in self.get_list_attr() ]
    def read_val(self,chaine2,niveau,listdata,mottest):
        # print "read_val",self.__class__.name_,mottest
       
        listattr=self.get_list_attr()
        attr=None
	lmottest=lower(mottest)
        for attr2 in listattr:
            if lmottest==lower(attr2.name_trio):
                attr=attr2
                break
            pass
        if (attr==None):
            for attr2 in listattr:
                for syno in attr2.syno:
                    if lmottest==lower(syno):
                        attr=attr2
                        pass
                    pass
                pass
            pass
        if (attr==None):
            msg= mottest+" pas compris par "+getNameEn(self) + " mots compris "+str([ x.name_trio for x in listattr ])
	    print msg
            raise Exception("dans read_val "+msg)
        mot=attr.name
        traite_trace(self.__class__,mottest,mot)
        # print "ici",mot
        type_val=attr.xtype
        val=getattr(self,mot)
        #print dir(type_val)
        try:
            if (len(type_val.xtypes)>1):
                type_val2=type_val.xtypes[1]
            else:
                type_val2=type_val.xtypes[0]
                pass
        except:
            type_val2=type_val
            pass
        ####print "tttttttttt",type_val,attr
        if type_val==rien:
            setattr(self,mot,"yes")
            return chaine2
        elif type_val==My_String or (isinstance(type_val2,XString)):
	    is_acc_o=0
	    try:
		chaine_into=type_val2.into
		if (chaine_into==["{"]):
                    is_acc_o=1
                    pass
		pass
            except:
                pass
            if chaine2[0]== '{' and is_acc_o==0:
                bloc=read_bloc(chaine2)
                setattr(self,mot,bloc)
                if mot!='bloc_lecture':
                    print "a modifier", self.__class__,mottest,mot
                    pass
                #cmd="self."+self.list_mot_[i]+"_=bloc"
                # exec(cmd)
               
	    elif chaine2[0][0]=='"':
		bloc=read_string(chaine2)
                setattr(self,mot,bloc)
            else:
                #cmd="self."+self.list_mot_[i]+"_=chaine[0]"
                # print "cmd",cmd
                # exec(cmd)
                #self.list_val_[i]=chaine[0]
                setattr(self,mot,chaine2[0])
                if len(chaine2): chaine2.pop(0)
                pass
            return chaine2
        elif (type_val==My_Int) or (type_val==My_Float) or (isinstance(type_val2,(XFloat,XInt))):
            setattr(self,mot,chaine2[0])
            if len(chaine2): chaine2.pop(0)
            return chaine2
        elif (type_val==Listfloat) or (type_val==Listfloat_f) or (type_val==Listentier_f) or (type_val==Listentier):
            if (type_val==Listfloat) or (type_val==Listentier):
		x=XInt()
                cmd="n="+chaine2[0]
                exec(cmd)
		x(n)
                if len(chaine2): chaine2.pop(0)
            else:
                n=get_dimension(listdata)
              
                pass
            l=[]
            for i in range(n):
                cmd="v="+chaine2[0]
                exec(cmd)
                if len(chaine2): chaine2.pop(0)
                l.append(v)
                pass
            setattr(self,mot,l)
            return chaine2
        elif (type_val==Listchaine) or (type_val==Listchaine_f):
            if  (type_val==Listchaine):
                x=XInt()
                cmd="n="+chaine2[0]
                exec(cmd)
                x(n)
                if len(chaine2): chaine2.pop(0)
            else:
                n=get_dimension(listdata)
                
                pass
            l=[]
            for i in range(n):
                #cmd="v="+chaine2[0]
                #exec(cmd)
                l.append(chaine2[0])
                if len(chaine2): chaine2.pop(0)
                
                pass
            setattr(self,mot,l)
            return chaine2
        
        type=type_val2.classes[0]
	typem=split(str(type),".")[-1]
	type2=split(typem,"'" )[0]
    
        if (val==None):
            #print "icit", type,typem
            #pr=type.__new__(type)
            #pr.__init2__()
            #type2=pr.name_
            
            cmd="self."+mot+"="+type2+"()"
            try:
                exec(cmd)
            except:
                raise Exception(cmd)
            pass
            #setattr(self,mot,pr)
            pass
        name_type=getattr(self,mot).name_
        if (type2[-5:]=='_base') and not(issubclass(type,class_generic)):
            # on va lire un derive
            type2_=get_class_name(chaine2[0])
            cmd="self."+mot+"="+type2_+"()"
            if len(chaine2): chaine2.pop(0)
            try:
                exec(cmd)
            except:
                raise Exception(cmd)
            #setattr(self,mot,pr)
            pass
        elif (type2[-6:]=='_deriv') or issubclass(type,class_generic):
            # on va lire un derive
            mot0=type2+'___'
            mot1=lower(chaine2[0]);
            try:
                type2_=get_class_name(mot0+synonyme[mot1])
            except KeyError:
                type2_=get_class_name(mot0+mot1)
                pass
            cmd="self."+mot+"="+type2_+"()"
            if len(chaine2): chaine2.pop(0)
            try:
                exec(cmd)
            except:
                raise Exception(cmd)
            #setattr(self,mot,pr)
            pass
        if (name_type=='defbord') or (name_type=='circleinnn'):
            dim=get_dimension(listdata)
            
            cmd="self."+mot+"="+name_type+"_"+str(dim)+'()'
            exec(cmd)
            #setattr(self,mot,pr)
            pass
        pr=getattr(self,mot)
        if 0:
            print "non"
        else:
            # try:
            # pr=self.list_val_[i]
            if pr==None:
                cmd="pr="+self.list_type_[i]+"()"
                try:
                    exec(cmd)
                except:
                    raise Exception(cmd)
                pass
            # print "in read data +chaine",self.list_type_[i],chaine
            # print "ici" ,pr.name_u
            pr.read_data(chaine2,niveau,listdata)
            # print "LA1 ", cmd
            # print "C",chaine
            # print "PY",pr.print_py()
            # print "LU",pr.print_lu(),i,self.list_mot_
            # cmd="self."+self.list_mot_[i]+"_=pr"
            
            # print "LA2"
            # try:
            # self.list_val_[i]=pr
            setattr(self,mot,pr)
            # exec(cmd)
            # except:
            #   raise Exception(cmd)
            #   print "ici",pr.print_lu(),cmd
            pass
        return chaine2
    def read_data(self,chaine,niveau,listdata=[]):
        
        # print "read_data",self,niveau
        if (getNameEn(self)[-5:]=='_base'):
            if 1:
                raise Exception("ici")
            # on va typer
            
            type2_=get_class_name(chaine[0])
            # print "ici",name_,type2_,chaine[0]
            cmd="cl1="+type2_
            exec(cmd)
            cmd="cl2="+name_
            exec(cmd)
            if not issubclass(cl1,cl2):
                print "Pb ",cl1," not asubclass of ",cl2
                raise Exception("erreur de type")
            cmd="self="+type2_+"()"
            exec(cmd)
            print "ERR",self.print_py(),self
            
            if len(chaine): chaine.pop(0)
            chsa=chaine
         
            self.read_data(chaine,niveau,listdata)
            chaine=chsa
            print "ERR",self.print_py()
            pass
        niveau=niveau+1
        listmot=(self.get_list_attr())
        if isinstance(self,interprete)and (0) :
            for attr in listmot:
                mottest=attr.name_trio
                self.read_val(chaine,niveau,listdata,mottest)
                pass
            pass
        # print "ici",chaine[0]
        else:
            if self.name_u=="not_set" and niveau==1 and not isinstance(self,interprete) :
                self.name_u=chaine[0]
                if len(chaine): chaine.pop(0)
            else:
                self.is_read=1 
                # on passe l'accolade
                if self.__class__.readacc_==1:
                    # on passe l'accolade
                    # print "accolade",chaine[0]
                    test_accolade(chaine[0])
                    if len(chaine): chaine.pop(0)
                    pass
                j=0
                n=len(listmot)
                while (((self.__class__.readacc_==1)and(chaine[0]!= '}')) or ((self.__class__.readacc_==0)and(j<n))):
                    
                    # on attend accolade ou on connait le nombre d'argument
                    # print "in readdata",self.__class__,n,j,self.__class__.readacc_,chaine[0]
                    if self.__class__.readacc_==1:
                        mottest=lower(chaine[0])
                        ##if not mottest in self.list_mot_trio_:
##			    print self.print_py()
##                            msg=chaine[0]
##                            msg=msg+" pas compris par "
##                            msg=msg+name_
##                            msg=msg+" mots compris "+str(self.list_mot_trio_)
##                            raise Exception(msg)
##                        i=self.list_mot_trio_.index(mottest)
                        if len(chaine): chaine.pop(0)
                    else:
                        attr=listmot[j]
                        # print "avt readval",attr,attr.name,attr.name_trio
                        mottest=listmot[j].name_trio
                        pass
                    # print "in read_data mot_att",self.list_mot_[i],self.list_type_[i],chaine[0]
                    if ((self.__class__.readacc_==0) and (listmot[j].optional==1)):
                        # si on ne lit pas d'accolade et que l'attribut est optionnel on essaye juste de lire
                        # si ca ne marche pas on continue
                        try:
                            self.read_val(chaine,niveau,listdata,mottest)
                        except:
                            #raise Exception("verfif")
			    # print "IIIIIIIIIIII",mottest,self.name_
			    try:
			      # on remet a None l'attribut mottest
			      setattr(self,mottest,None)
                            except:
                              #raise Exception("verfif")
			      pass
                            pass
                        pass
                    else:
                        self.read_val(chaine,niveau,listdata,mottest)
			# try:
		        # 	print "la",mottest, getattr(self,mottest)
			# except:
			#	print "pb", mottest
			#	pass
                        pass
                    j=j+1
                    pass
                if  self.__class__.readacc_ ==1:
                    test_accolade(chaine[0])
                    if len(chaine): chaine.pop(0)
                    pass
                # print "ici_fin_while",chaine
                pass
            pass
        niveau=niveau-1
        return chaine

    def print_py(self):
        a=getNameEn(self)+'('
        list_attr=self.get_list_attr()
        for attr in list_attr:
            # exec("val=self."+self.list_mot_[i]+"_")
            val=getattr(self,attr.name)
            # for i in range(len(self.list_mot_)):
            nm=attr.name
            #val=self.get_value(nm)
            # print "icitt ", nm,val,self.__class__,type(val)
            if val!=None:
                # print "VAL",val,type(val),type(val) is InstanceType
                type_val=attr.xtype
                if  type(val) is InstanceType:
                    # print "classe_val",val
                    # if isinstance(val,list):
                    if 1:
                        a=a+nm+"="+val.print_py()+","
                        pass
                    else:
                        raise Exception("bizarre")
                    pass
                elif isinstance(val,objet_u):
                    a=a+nm+"="+val.print_py()+","
                else:
                    # print "string"
                    a=a+nm+"="+value2text(val)+","
                    #a=a+nm+"=\'"+str(val)+"\',"
                    pass
                pass
            pass
        if self.is_read==1:
            a=a+"is_read=1,"
        # for ref in self.list_ref_:
        #   if ref[2]!='not_set':
        for ref in self.get_list_ref():
            val=getattr(self,ref.name)
            if val!=None:
                #a=a+ref[0]+"=\'"+ref[2]+"\',"
                a=a+ref.name+"=\'"+val+"\',"
                pass
            pass
        if self.name_u != "not_set":
            a=a+"name_u=\'"+self.name_u+"\'"
            pass
        if a[len(a)-1]==",":
            a=a[:len(a)-1]
            pass
        a=a+')'
        return a
    def print_associer(self,list_class):
        a=""
       
        for ref in self.get_list_ref():
            val=getattr(self,ref.name)
            if val!=None:
                # print list_class
		if isinstance(val,objet_u):
                    clref=val
		else:
                    clref=trouve_class_list(val,list_class)
                    pass
                # print "ici associer",clref.mode_ecr,ref[2]
                #if abs(clref.mode_ecr)<2:
                if 1:
                    a=a+clref.print_lu(list_class)
                    pass
                try:
                    te=type(pb_mg)
                    pb_mg_existe=1
                except:
                    pb_mg_existe=0
                    pass
                if (pb_mg_existe==0) or (not isinstance(self,pb_mg)):
                    a=a+"Associate "+self.name_u+" "+clref.name_u+"\n"
                    pass
                pass
            pass
        
        return a
    def print_lire(self,list_class=[]):
        # print "print_lire",self
        bloclire=isinstance(self,interprete)
	# on modifie lec pour voir
	# on ecrit qd meme si on a rien lu 
	# sauf pour les objets de base
	lec=0
        if (bloclire==0) and (self in list_class):
            stri="Read "+self.name_u
        else:
            stri=""
	    lec=1
            pass
        if self.__class__.readacc_==1:
            stri=stri+"\n{ \n"
            pass
        #lec=0
        #list_mot=self.get_list_mot()
        #list_mot_trio=self.get_list_mot_trio()
        list_attr=self.get_list_attr()
        for attr in list_attr:
            # exec("val=self."+self.list_mot_[i]+"_")
            val=getattr(self,attr.name)
            if (val!=None):
                lec=1
                if self.__class__.readacc_==1:
                    stri=stri+attr.name_trio
                    pass   
                stri=stri+" "
                type_val=attr.xtype
                #print "titi",val,type_val,type(val),type(val) is MethodType
                #print MethodType
                if type(val) is InstanceType:
                    # print type(val),val,self.__class__
                    stri=stri+val.print_lu()
                elif isinstance(val,objet_u):
                    #print type_val.xtypes[1]
                    if (isAttributeLink(attr)):
                    #  if (len(type_val.xtypes)==3)and isinstance(type_val.xtypes[1],XString):
                        print " afaire"
                        stri+=val.name_u
                    else:
                        stri=stri+val.print_lu()
                        pass
                else:
                    # print "dans le else",attr.name,type(val)
                    # print type(type_val)
                    
                    if (type_val==Listfloat) or (type_val==Listfloat_f) or (type_val==Listentier_f) or (type_val==Listentier):
                        if (type_val==Listfloat) or (type_val==Listentier):
                            # print "on a une liste"
                            stri=stri+' '+ str(len(val))
                            pass
                        for i in val:
                            stri=stri+' '+repr(i)
                            pass
                        pass
                    elif (type_val==Listchaine) or (type_val==Listchaine_f):
                        if (type_val==Listchaine):
                            stri=stri+' '+ str(len(val))
                            pass
                        for i in val:
                            stri=stri+' '+str(i)
                            pass
                        pass
                    elif (type_val==rien):
                        pass
                    elif (type(val)==float):
                        stri+=' '+repr(val)
                    else:
                        stri=stri+str(val)
                        pass
                    pass
                if self.__class__.readacc_==1:
                    stri=stri+'\n'
                    pass
                pass
            pass
        if (lec==1) or (self.is_read):
            if self.__class__.readacc_==1:
                stri=stri+" }"
                pass
            #stri=stri+'\n'
            pass
        else:
            stri=""
            pass
        # self.mode_ecr=self.mode_ecr/abs(self.mode_ecr)*16
        return stri
    def print_lu(self,list_class=[]):
        # print self, "print_lu"
        abs_mode_ecr=abs(self.mode_ecr)
        stri=""
	# pour ne pas ecrire deux fois les noms des eqn
        if not isinstance(self,mor_eqn):
            stri=getNameTrio(self)
            if (isinstance(self,class_generic)and self in list_class):
                stri=getNameEn(self)
                pass
	elif (self in list_class):
            stri=getNameTrio(self)
            pass
   
        stri+=" "
        stri0=stri
        #print stri," ",name_
        if 1:
            if abs_mode_ecr<2:                
                if (self in list_class):
                    if not isinstance (self,interprete):
                        stri=stri+self.name_u+" \n" #split(self,'.')[1]+" "
                        pass
                    pass
                if self in list_class:
                    self.mode_ecr=self.mode_ecr*2
                    pass
                pass
            else:
                stri=""
                pass
            if abs_mode_ecr<4:
                stri=stri+self.print_associer(list_class)
                if self in list_class:
                    self.mode_ecr=self.mode_ecr*2
                    pass
                pass
            if (abs_mode_ecr<8):
                on_ecrit=1
                if (self.__class__.attdiscr==1):
                    i0=-1
                    # on recherche le discretiser precedent
                    if (self in list_class):
                        m=list_class.index(self)

                        for i in range(m):
                            cl=list_class[i]
                            # print "cherche discr",cl
                            if isinstance(cl,discretize):
                                i0=i
                                break
                            pass
                        # print cl
                        pass
                    if i0!=-1:
                        if cl.mode_ecr<2:
                            on_ecrit=0
                        else:
                            on_ecrit=1
                            pass
                        pass
                    #print "attdicscr",self.attdiscr,i0,on_ecrit
                    pass
                
                if (on_ecrit):
                    if isinstance (self,interprete): stri=stri0
                    stri=stri+self.print_lire(list_class)+'\n'
                    if self in list_class:
                        self.mode_ecr=self.mode_ecr*2
                        pass
                else:
                    if isinstance (self,interprete): stri=""
                    pass
                pass
            #    pass
            pass
        return stri
    def verify(self,listclass=[],mode=0,leve=0):
	self.status="Valid"
	msg=""
	tout_vide=1
        #print "verify",self
        self.test_valid={}
        for attr in self.__class__.getAllInitXAttributes():
            self.test_valid[ attr.name]="Valid"
            val=getattr(self,attr.name)
	    if val and attr.name!="name_u":
                tout_vide=0
                pass
            #print attr.name,val,isinstance(attr,Tr_Attribute) and (attr.optional==0)
            if (val is None) and  isinstance(attr,Tr_Attribute) and (attr.optional==0):
               if self.is_read==1:
                msgn= "Pb "+str(self)+" "+str(self.name_u)+" "+str(self.is_read) + " "+ attr.name+ " is none and no optionnal"
		msg+=msgn+";"
                self.status="NoValid"
                self.test_valid[attr.name]="NoValid"
                pass
            if val:
                st="Valid"
                if isAttributeLink(attr):
		    try:
                        st=verify_ref(val,attr,listclass)
		    except:
                        msg+=self.name_+" "+str(self.name_u)+" "+attr.name+" ref invalid;"
                        st="NoValid"
                        pass
                    pass
                if isinstance(val,objet_u):
                    st=val.verify(listclass,mode,leve)
                    pass
                #print val,st
                if st=="NoValid":
                    self.status=st
                    self.test_valid[attr.name]="NoValid"
                    pass
                
                pass
            pass
	
	if self.status!="Valid":
            
            if tout_vide:
                
                for cl in listclass:
                    if isinstance(cl,partition) and not(isinstance(self,interprete)):
                        self.status="Valid"
                        pass
                    pass
		pass
            pass
        if self.status!="Valid":
            global is_import_file
            if not is_import_file:
                print msg
            if leve:
                raise Exception(msg)
            pass
        return self.status
    
    pass

def verify_all(listclass,mode=0):
    for cl in listclass:
        st=cl.verify(listclass,mode,leve=1)
        pass
    pass

def trouve_class_list(mot,listclass):
    "trouve la classe de nom mot dans la listclass"
    ok=0
   
    mot2=mot

    
    for cl in listclass:
        #print cl.name_u, mot
        if cl.name_u==mot2:
            ok=1
            break
        pass
    if ok !=1 :
        global is_import_file
        if not is_import_file:
            print mot," pas trouve comme classe utilisateur, noms existants: ",
            for cl in listclass:
                if cl.name_u!="not_set":
                    print  cl.name_u,
                pass
            print
        raise Exception("pb dans trouve_class")
    return cl


def read_data_read(self,chaine,niveau,listclass=[]):
    niveau=niveau+1
    mot=chaine[0]
    # print listclass[0].name_u
    cl=trouve_class_list(mot,listclass)
    #self.name_obj_=mot
    #self.obj_=cl
    print "read ",cl.name_u,cl
    # on remet cl a la fin de la list
    listclass.remove(cl)
    listclass.append(cl)
    #cl.lire_=1
    # on passe l'accolade
    # on passe le mot
    if len(chaine): chaine.pop(0)
    cl.read_data(chaine,niveau,listclass)
    niveau=niveau-1
    return chaine

def read_string(chaine):
    " lecture d'un string c.a.d \"rtoit otot\" et renvoie de la meme chose"		
    for i in xrange(1,len(chaine)):
        if chaine[i][-1] == '"':
	   break
	pass
    bloc=""
    for ii in xrange(i+1):
        bloc=bloc+chaine[ii]+" "   
        pass
    for ii in xrange(ii+1):
        chaine.pop(0)
        pass
    #chaine=chaine[i+1:]
    return bloc
def read_bloc(chaine):
    " lecture d'un bloc c.ad. soit un mot soit un bloc entre accolade"
    acc=1;
    # print chaine[0]
    for i in xrange(1,len(chaine)):
        if chaine[i] == '{':
            acc=acc+1
        elif chaine[i] == '}':
            acc=acc-1
            if acc==0:
                # print "fin trouve"
                break
            pass
        pass
    bloc=""
    for ii in xrange(i+1):
        bloc=bloc+chaine[ii]+" "
        pass
    for ii in xrange(i+1):
        chaine.pop(0)
        pass
    #chaine=chaine[i+1:]
    return bloc




def read_bloc_comm(c):
    " lecture d'un bloc de commentaire (cherche le # suivant) "
    z=c.index("#")
    bloc=' '.join(c[:z])
    c=c[z+1:]
    #    print "comm ",bloc, c
    bloc=replace(bloc,"'"," ")
    return bloc,c
def read_bloc_comm_entier(c):
    " lecture d'un bloc de commentaire (cherche le */ fermant) "
     
    z=c.index("*/")
    bloc=' '.join(c[:z])
    if "/*" in bloc:
        print "non code"
        1/0
    c=c[z+1:]
    #    print "comm ",bloc, c
    bloc=replace(bloc,"'"," ")
    return bloc,c

def retire_new(c):
    " pour ovap mais pas utilisable.. "
    
    for i in range(len(c)):
        if (lower(c[i])=='new'):
            
            print "on retire new"
            c[i]=c[i+1]
            c[i+1]=c[i+2]
            c[i+2]='lire'
            c.insert(i+3,c[i+1])
            pass
        pass
    pass
pass
def modifie_ordre(listclass,mtype1,mtype2):
    "modifie listclass pour que type2 soit apres type1"
    try:
     cmd="type1="+mtype1
     exec(cmd)
     cmd="type2="+mtype2
     exec(cmd)
    except:
        print mtype1," ou ",mtype2," inconnus..."
        return
    listpbsol=[]
    listpbf=[]
    for i in range(len(listclass)):
        cl=listclass[i]
        if isinstance(cl,type1): 
            listpbf.append(i)
            pass
        if isinstance(cl,type2):
            listpbsol.append(i)
            pass
        pass
    while len(listpbsol) and len(listpbf):
        dpbf=listpbf[-1]
        ppbs=listpbsol[0]
        if (ppbs<dpbf):
            sa=listclass[ppbs]
            listclass[ppbs]=listclass[dpbf]
            listclass[dpbf]=sa
            listpbf[-1]=ppbs
            pass
        listpbsol=listpbsol[1:]
        pass
    pass
def retire_nl(a):
    " retire les new line sauf dans les commentaires "
    aa=a.replace('\n',' \n ')
    aa=split(aa,' ')
    # b=' '.join(aa)
    poub,_,listcomm=traite_naif(aa)
    
    rr=' '.join(poub).replace('\n',' ').replace('__NL__','\n')
    # int rr
    
    return rr
    # print "la"
    # aa.append('fin')
    # print listcomm
    debut=0
    while ('\n' in aa[debut:]):
        index=debut+aa[debut:].index('\n')
        index0=len(aa)-index
        in_comm=0
        for i  in range(len(listcomm)/2-1,-1,-1):
            index1=listcomm[2*i]
            index2=listcomm[2*i+1]
            # print index0,index1,index2
            
            if index0>index2   and index0<index1:
                in_comm=1
                pass
            pass
        if (in_comm==0):
            # print "avt", aa[index-3:index+3]
            bb=aa
            aa=bb[:index]
            aa.append(' ')
            aa+=(bb[index+1:])
            # print "apres", aa[index-3:index+3]
        # on pase au suivant
        # print "aaici",aa[debut:debut+5]
        debut=index+1
        # print "aaici",aa[debut:debut+5]
        pass
    b=' '.join(aa)
    return b
    b=""
    in_comm=0
    for i in range(len(a)):
        c=a[i]
	avt_c=" "
	apres_c=" "
	if i>0:
		avt_c=a[i-1]
		pass
	if i<len(a)-1:
		apres_c=a[i+1]
	else:
		apres_c=" "
		pass
        if (c=="#")and ((avt_c==" ") or (avt_c=='\n')) and  ((apres_c==" ") or (apres_c=='\n')):
            if (in_comm==1):
                in_comm=0
            else:
                in_comm=1
                pass
            pass
        if (c=='\n'):
            if (in_comm==0):
                b+=" "
            else:
                # on garde le new line et on rajoute des espaces
                b+=" "+c+" ";
                pass
            pass
        else:
            b+=c
            pass
        pass
    return b

def strip_data(data):
    """ retire les blancs et les retour chariot"""
    b=data.replace('\n',' ').replace('\t',' ').strip().replace('  ',' ')
    c=b.replace('  ',' ')
    while (cmp(b,c)!=0):
        b=c
        c=b.replace('  ',' ')
        pass
    return b

def lire_fichier(name):
    " lit le fichier et renvoie une liste modififee de chaines de caractere"
    ent=open(name,'r')
    a=ent.read()
    b=replace(a,"\011"," ")
    # pour le cas ou le fichier est passe par dos
    b=replace(b,'\r'," ")
    b=retire_nl(b);
    #b=replace(a,"\012"," ")
    b=replace(b,"}"," }")
    c=replace(b,"  "," ")
    while (cmp(b,c)!=0):
        b=c
        c=replace(b,"  "," ")
        pass
    c=split(c,' ')

    # important pour ne pas s'arreter au debut du cas
    # si il commence par des blancs....
    while (c[0]==''):
	if len(c): c.pop(0)
	pass
    # pareil a la fin
    while (c[-1]==''):
        if len(c): c.pop()
	pass
    _,d,listcomm=traite_naif(c)
    return c,d,listcomm
def read_file_data(name):
    " lit le jdd de nom name et renvoie la liste des classes"
    print "read_file_data",name
    listclass=[]

    c,d,listcomm=lire_fichier(name)
    # on ajoute fin a la fin au cas ou
    
    c.append('fin')
        


    #c=map(lower,c)
    #   print c
    #retire_new(c)
    nbclass=0
    mot="debut"
    while ( (mot!='fin') and (mot!='end') and (c[0]!='£') ):
        mot=c[0]
        mot=lower(mot)
        fichier_lu=0
        if ((mot=='lire_fichier')or (mot=='read_file') )and (c[2]==';'):
            try: 
                print 'lecture de',c[1]
                new_chaine,dd,listcomm2=lire_fichier(c[1])
                l=c[3:]
                # ouhps si commentaire dans lire_fichier c est faux !!!
                c=new_chaine+l
                _,d,listcomm=traite_naif(c)
                c.append('fin')
                # on remet le fin pour decaler
                fichier_lu=1
            except:
                print "fichier pas encore existant ? "
            pass
        if fichier_lu==0:
            if len(c): c.pop(0)
            
            namea="a"+str(nbclass)
            cmd=namea+"="+get_class_name(mot)+"()"
            # print "cmd",cmd
            try:
                exec(cmd)
                pass
            except:
                print cmd, " impossible"
                raise Exception("pb")
            niv=0

            
            # print len(c),len(d),d[0]
            
            if (mot=="#") or (mot=="/*"):
                sa=len(c)
                ok=0
                for index  in range(len(listcomm)/2-1,-1,-1):
                    index1=listcomm[2*index]
                    index2=listcomm[2*index+1]
                    #print index1,index2,sa
                    if index1==sa-1:
                        ok=1
                        # print (c[len(c)-index1:len(c)-index2-1])
                        exec(namea+".comm=' '.join(c[len(c)-index1-1:len(c)-index2-2])")
                        c=c[len(c)-index2-1:]
                        # print "iii",c[0:4],d[0:4]
                        break
                    pass
                if (ok==0):
                    exec(namea+".read_data(c,niv,listclass)")
                    # a revoir pour uiliiser listcomm !!!!!!!
                    1/0
                    pass
                pass
            else:
                if len(d): d.pop(0)
                sa=len(d)
                            
                exec(namea+".read_data(d,niv,listclass)")
                
                decal=0
                lenc=len(c)+1 # on  a retire le premier mot
                lenc=len(d)
                #nbmotlu= sa-len(d)
                for index  in range(len(listcomm)/2-1,-1,-1):
                    index1=listcomm[2*index]
                    index2=listcomm[2*index+1]
                    if (lenc>=index2):
                        decal+=index1-index2+1
                        lenc=len(d)+decal
                        # print lenc,index1,index2,decal
                        
                        pass
                    pass
                
                c=c[len(c)-len(d)-1-decal:]
                # print "d", d[0:4]
                # print "c", c[0:4]
                pass
            
            exec("listclass.append("+namea+")")
            
            nbclass=nbclass+1
            pass
        pass
      
    inut=[]
    for cl in listclass:
        # is_associate=isinstance(cl,associate)
        is_associate=(cl.__class__.name_=="associate")
        
        if isinstance(cl,read) or is_associate:
            inut.append(cl)
            pass
    pass
    for cl in inut:
	listclass.remove(cl)
        #print cl 
        from xmetaclass import removeXObject,getXObjects
        if cl in getXObjects():
            removeXObject(cl)
            pass
        #print "apres",getXObjects()
	pass
    for cl in listclass:
        #print "TEST ECR",cl
        #print cl.print_lu(listclass)
        pass
    # on s'occupe des pbs couples....
    # on veut les pb solides apres les pb_fluides_turbulents
    modifie_ordre(listclass,"pb_thermohydraulique_turbulent","pb_conduction")
    modifie_ordre(listclass,"pb_hydraulique_turbulent","pb_conduction")
    modifie_ordre(listclass,"modele_rayo_semi_transp","Pb_base")
    modifie_ordre(listclass,"modele_rayonnement_milieu_transparent","Pb_base")

    # modifie_ordre(listclass,"probleme_couple","discretiser")
    modifie_ordre(listclass,"coupled_problem","discretize")

    #modifie_ordre(listclass,pb_couple_rayo_semi_transp,modele_rayo_semi_transp)
    return listclass



dec="    "
def decal(i):
    p=""
    for j in range(i):
        p+=dec
        pass
    return p
def indent_chaine(c):
    new=""
    niveau=0
    i0=-1
    for i in range(len(c)):
        if (i<=i0):
            continue
        i0=i
        if (c[i]=='{'):
            new+="  "+c[i]
            niveau+=1;
        elif (c[i]=='}'):
            niveau-=1
            if new[-len(dec):]==dec:
                new=new[:-len(dec)]
                pass
            new+="  "+c[i]
        elif (c[i]=='\n'):
            new+=c[i]+decal(niveau)
            try:
                while (c[i0+1]==' '):
                    i0+=1
                    pass
                pass
            except:
                pass
        else:
            new+=c[i]
            pass
        pass
    return new
   


def traite_naif(aa):
    a=aa[:]
    # return a
    c=[]
    decal=0
    listcomm=[]
    lena=len(a)-1
    for i0 in range(len(a)):
        i=i0+decal
        if i>=len(a): break
        mot=a[i]
        # print i,"la",mot
        if (mot=='#'):
            listcomm.append(lena-i)
            # print "commentaire apres", a[i-30:i],a[i]
            while (a[i+1]!="#"):
                if a[i+1]=='\n':
                    a[i+1]='__NL__'
                if i+1>=len(a)-1:
                    i-=1;
                    print " il manque un # ?"
                    break;
                i+=1
                pass
            i+=1
            listcomm.append(lena-i)
            # print "OKKKKKKKKKKK",a[i]
        elif (mot=='##'):
            print "pas gere .... ";1/0
            while (a[i+1]!="end##"): i+=1;print i,"dans ##",a[i]
            i+=1
            print "okkk",a[i],"okk"
        elif (mot=="/*"):
            listcomm.append(lena-i)
            acc=1
            while(acc!=0):
                if i+1>=len(a):
                    i-=1;
                    print " il manque un */ ?"
                    break;
                
                te=a[i+1]
                if (te=="*/"):
                    acc-=1
                elif (te=="/*"):
                    acc+=1
                    pass
                if a[i+1]=='\n':
                    a[i+1]='__NL__'
                i+=1
                pass
            listcomm.append(lena-i)
            # print "okkk",a[i],"okk"
        else:
            c.append(mot)
            pass
        decal=i-i0
        pass
    return a,c,listcomm
def write_file_data(name,listclass):
    " ecrit le jdd dans name "
    print "write_file_data ", name
    s=open(name,'w')
    # on initialise le flag d'ecriture
    listech=[]
    listproech=[]
    # print "write",listclass
    for c in listclass:
        # print c
        if  isinstance(c,objet_u):
            # is_associate=isinstance(c,associate)
            is_associate=(c.__class__.name_=="associate")
        
            if isinstance(c,read) or is_associate:
                pass
            else:
                c.mode_ecr=c.mode_ecr/abs(c.mode_ecr)
                # print "mode_ecr",c.name_u,c,c.mode_ecr
                pass
            pass
        else:
            print c
	    throw("pas une classe objet_u ?????")
    for c in listclass:
        # is_associate=isinstance(c,associate)
        is_associate=(c.__class__.name_=="associate")
        
        if isinstance(c,read) or is_associate:
            continue
        # a=c.print_lu(listclass)
        # print c,a,"l"
        s.write( c.print_lu(listclass))
        pass
    s.write('\n')
    s.close()
    # on retire les doubles \n
    s=open(name,'r')
    a=s.read()
    s.close()
    b=replace(a,'\n\n','\n')
    b=replace(b,'  ',' ')
    b=replace(b,'\n\n','\n')
    # on idente grace aux accolades
    b=indent_chaine(b)
    s=open(name,'w')
    s.write(b)
    s.close()    
    return

def write_file_python(name,listclass):
    " ecrit les classes dans un fichier python"
    print "write_file_python ", name
    s=open(name,'w')
    s.write('from triou import *\n')
    #s.write('listclass=[]\n')
    # s.write('s=open(\'test2.data\',\'w\')\n')
    i=0
    list_nom=[]
    ll2="["
    for c in listclass:
        nm="a_"+str(i)
        # is_associate=isinstance(c,associate)
        is_associate=(c.__class__.name_=="associate")
        
        if isinstance(c,read) or is_associate:
            continue
        i=i+1
        if c.name_u != "not_set":
            nm=c.name_u
	    if nm in list_nom:
		    nm+="_"+str(i)
		    pass
            if nm in dicoclass.values() or nm=="in":
                nm+="_i"
                pass
            pass
        #print "print py",nm,c
	list_nom.append(nm)
        s.write(nm+"="+c.print_py()+'\n')
        #s.write('listclass.append('+nm+')\n')
        ll2=ll2+nm+","
        
        # s.write('s.write('+nm+'.print_lu()+\'\\n\')\n')
        pass
    ll2=ll2[:-1]+"]"
    s.write('listclass='+str(ll2)+'\n')
    s.close()
    return

class listobj_impl(objet_u):
    name_='listobj_impl'
    name_trio_='listobj_impl'
    readacc_=1
    class_type="not_set"
    virgule=1
    " classe mere pour la lecture d'une liste de classes"
    def __init2__(self):
        # self.class_type="not_set"
        # self.virgule=1
        objet_u.__init2__(self) 
        return
    def verify(self,listclass,mode,leve):
	self.status="Valid"
        self.test_valid["listobj"]="Valid"
        for obj in self.listobj:
            st=obj.verify(listclass,mode,leve)
	    if st != "Valid":
		    self.status=st
                    self.test_valid["listobj"]=st
            pass
        return self.status
    def read_data(self,chaine,niveau,listdata=[]):
        if niveau==0  :
           self.name_u=chaine[0]
           if len(chaine): chaine.pop(0)
	   return
        niveau=niveau+1
        bizarre=0
        
        # on passe l'accolade
        i2=-1
        if (self.__class__.readacc_==0):
            # print self," ici pb"
            if (type(self.virgule)==type(1)):
                i2=atoi(chaine[0])
            else:
                bizarre=1
                i2=1
                pass
        #    print "nbre ",i2
            pass
        else:
            test_accolade(chaine[0])
            pass
        if not(bizarre):
            if len(chaine): chaine.pop(0)
        else:
            # print chaine
            pass
        if self.listobj==None:
	    self.listobj=[]	
            pass
        
        cmd2="tst_titi2="+self.__class__.class_type
        exec(cmd2)
        is_class_generic=issubclass(tst_titi2,class_generic)
       
        while (chaine[0]!= '}') and (i2!=0):
            i2=i2-1
            #   print "lecture ",i2
            self.listobj.append(None)
            newob="self.listobj[-1]"
	    special=1
	    if (self.__class__.class_type[-6:]=='_deriv') or (is_class_generic):
                # on va lire un derive
		type2_=self.__class__.class_type
                mot0=type2_+'___'
	        mot1=lower(chaine[0]);
                if len(chaine): chaine.pop(0)
                try:
                    type2_=get_class_name(mot0+synonyme[mot1])
                except KeyError:
                    type2_=get_class_name(mot0+mot1)
                    pass
	        cmd=newob+"="+type2_+"()"
	    elif (self.__class__.class_type[-5:]=='_base'):
                type2_=get_class_name(chaine[0])
                if len(chaine): chaine.pop(0)
                
                cmd="cl1="+type2_
                exec(cmd)
                cmd="cl2="+self.__class__.class_type
                exec(cmd)
                if not issubclass(cl1,cl2):
                    print "Pb ",cl1," not asubclass of ",cl2
                    raise Exception("erreur de type")
                cmd=newob+"="+type2_+"()"
	    else:
	    	special=0
                cmd=newob+"="+self.__class__.class_type+"()"
                pass
            exec(cmd)
            # print newob
            # print chaine
            #  print listdata
            newobj=self.listobj[-1]
            lenchaine=len(chaine)
            newobj.read_data(chaine,niveau,listdata)
            
            if (self.__class__.virgule==1):
                # chaine=chaine2
                if chaine[0]==',':
                    if len(chaine): chaine.pop(0)
                    pass
                else:
                    if chaine[0]!='}':
                        message="erreur dans la lecture de "+self.__class__.name_
                        message+='\n on attendait , ou } et non '+chaine[0]
                        print message
                        raise Exception(message)
                    pass
                pass
	    else:
                if (special==0) and (len(chaine)==lenchaine) and chaine[0]!='}' :
		    raise Exception("erreur dans lecture "+self.__class__.class_type+" chaine "+' '.join(chaine[:4])+" new chaine2 "+' '.join(chaine[:4]))
	        
                pass
        if (self.__class__.readacc_==1):
            test_accolade(chaine[0])
            if len(chaine): chaine.pop(0)
            pass
        niveau=niveau-1
        pass
    
    def print_lu(self,listclass=[]):
        stri=""
        if self in listclass:
            stri+=self.name_trio_+" "+ self.name_u+"\nread "+ self.name_u
        bizarre=0
        if type(self.__class__.virgule)!=type(1):
            bizarre=1
            pass
        # print type(self.__class__.virgule)," ",bizarre
        if (self.listobj==None) or len(self.listobj)==0:
            if (self.__class__.readacc_ == 0):
                return "0 "
            else:
                return "{ } "
        if self.__class__.readacc_==1:
            stri+=" \n{\n"
        else:
            if not(bizarre):
                stri+=str(len(self.listobj))
            else:
                stri+=""
                pass
            pass
        for obj in self.listobj:
            if (isinstance(obj,mor_eqn)):
                stri=stri+" "+getNameTrio(obj)
                pass
            if bizarre and stri!="" : stri=stri+self.__class__.virgule+" "
            stri=stri+" "+obj.print_lu(listclass)
            #stri=stri[:-2]
            if (self.__class__.virgule==1):
                stri=stri+"\n,\n"
                pass
            pass
        if (self.__class__.virgule==1):
            stri=stri[:len(stri)-3]
            pass
        if self.__class__.readacc_==1:
            stri=stri+"}\n"
        else:
            stri=stri+'\n'
            pass
        return stri
    def print_py(self):
        #  print self.__class__,"print_py"
        stri=getNameEn(self)+'('
        if (self.name_u!="not_set"):
            stri+="name_u=\""+self.name_u+"\","
        stri+='listobj=['
        i=0
        for obj in self.listobj:
            # stri=stri+"obj"+str(i)+"="+obj.print_py()+","
            stri=stri+obj.print_py()+","
            i=i+1
            pass
        if stri[len(stri)-1]==",":
            stri=stri[:len(stri)-1]
            pass
        stri=stri+"])"
        return stri
    pass


def trouve_base(name):
    " renvoie la classe de base de name -> ne sert plus"
    # print dir()
    i=0
    for l in dicobases.values():
        if name in l:
            return dicobases.keys()[i]
        i=i+1
        pass
    msg=name+" not in dico"
    print "dico",dicobases
    raise Exception(msg)
pass

def trouve_pere(name):
    " trouve le nom du pere de la classe name" 
    #base=trouve_base(name)
    #print dicobases[base]
    #ll=[]
    cmd="cl="+name
    exec(cmd)
    #print cl.__bases__,type(cl.__bases__[0]),dir(cl.__bases__[0])
    #cmd="per="+cl.__bases__[0]
    per=cl.__bases__[0]
   
    per=per.name_
    #print per
    name_per=replace(str(per),'triou.','')
    return name_per
pass

def gen_doc(name,doc_gen,fr=1,l=None,niveau=-1):
    " genere la doc associee a la classe de base name"
    niveau+=1
    debug=0
    if l:
        if name not in l:
            niveau-=1
            return ""
        pass
    
    if (doc_gen[name] == 1) :
	if debug: 
            print "la doc de ",name," a deja ete genere"
            pass
        niveau-=1
        return ""
    doc_gen[name]=1
    pere=trouve_pere(name)
    tstr=""
    tstr2=""
    cmd="icl="+name
    exec(cmd)
    cmd="ipere="+pere
    exec(cmd)
    # print "pere",ipere
    #lcl=icl.list_mot_
    #lcltr=icl.list_mot_trio_
    #lpere=ipere.list_mot_
    clname=icl.name_trio_
    vrai_name_=icl.name_
    if debug: 
        print "gen_doc name",name
        pass

    # generation de l'entete tstr contient le debut
    # tstr2 la deuxieme partie
    pb=1
    if fr==1 and clname=='':
        clname=vrai_name_
        pb=0
        pass
    if fr==0:
        clname=icl.name_
        pass
    
    if clname=='{':
        clname='\{'
	pb=0
	pass
    elif clname=='}':
        clname='\}'
	pb=0
	pass
        pass

    if clname in dicobases.keys(): # si c'est une section, je ferme l'accolade laissee ouverte
        if name!='methode_loi_horaire':
            tstr+='\label{'+replace(name,'_','')+'}'+"}\n"
        else:
            tstr+='\label{'+replace(name,'_','')+"}\n"
    else:
        if clname=='/*' or clname=='#' or clname=='champ_base' or clname=='champ_front_base':
            tstr+='}'

    if pb:
         tstr=tstr+' \index{'+clname+'} '
	 pass
    tstra=""
    if clname not in dicobases.keys():
        # if name != vrai_name_:
        tstra = "\subsection{"+clname+"\label{"+replace(name,'_','')+"} }\n"
        pass
    if issubclass(icl,objet_lecture) or issubclass(icl,listobj) :
        if (niveau>1):
            tstra = "\subsubsection{"+clname+"\label{"+replace(name,'_','')+"} }\n"
            pass
        pass
    # a t on un syno
   
    if clname in synonyme.values():
	
        for k in synonyme.keys():
            if synonyme[k]==clname:
                tstra+="Synonymous: { \\bf "+ k   + ' } \index{  '+k +'  } \\newline \\newline  \n' 
                break
     # ecriture de la description de l'objet
    tstr = tstr + tstra
    try:
         descr=replace(replace(icl.descr_,'{','\{'),'}','\}')
         descr=replace(replace(descr,'\{\{','{'),'\}\}','}')
         tstr=tstr+"Description: "+descr+'\n \\newline \\newline\n'
         pass
    except:
         pass
    if (icl.attdiscr==1):
         tstr+="Keyword Discretiser should have already be used to read the object. \n\n"
    if fr:
        # print "clname",clname
        tstr=tstr+ "See also: "+ ipere.name_trio_
        # print tstr
        
    else:
        tstr=tstr+ "Class "+ clname +" herits from "+ ipere.name_
        pass
    
    tstr=tstr+" (\\ref{"+replace(ipere.name_,'_','')+"})"
    # tstr=tstr+'.\n \n'


    # recherhce des classes filles
    listf=[]
    for obj in getXClasses():
        # print obj
        if issubclass(obj,icl):
            if obj.__bases__[0].name_==vrai_name_:
                if (l is None) or (obj.name_ in l):
                    listf.append(obj)
                    pass
                pass
            pass
        pass
    # ecriture des classes filles
    if len(listf):
        # tstr+="classes filles: "
        for ob in listf:
            nob=ob.name_trio_
            if nob=='{': nob='\{'
            if nob=='}': nob='\}'
            if nob is None or nob=='':
                nob=ob.name_
                pass
            tstr+=" "+nob+" (\\ref{"+replace(ob.name_,'_','')+"}) "
            pass
        pass
    tstr+="\n\\newline \\newline \n"
    
    tstr+="Usage:\n\n"

    # cas particulier d'une liste d'objet
    if issubclass(icl,listobj_impl):
        if icl.class_type=="not_set":
            niveau-=1
            return tstr
        if icl.readacc_==1:
            tstr+="\{ "
        else:
	    tstr+=" n "     
            pass
	tstr+=" object1 "
	if icl.virgule:
            tstr+=" ," 
            pass
        tstr+=" object2 ...."
	if icl.readacc_==1:
            tstr+=" \} "
            pass
        
        type="\n\nlist of { \it "+dicoclass.keys()[dicoclass.values().index(icl.class_type)]+"}"
        type2=icl.class_type
        tstr+=type+" (\\ref{"+replace(type2,'_','')+"})"
        if icl.virgule:
            tstr+=" separeted with , "
            pass
        tstr+="\n\n"
        
        # print type,type2
        cmd="es1="+type2+"()"
        exec(cmd)
        es=es1.__class__
        if issubclass(es,objet_lecture):
            tstr+=gen_doc(es.name_,doc_gen,fr,l,niveau)
            pass
        niveau-=1
        return tstr
        pass

    # lcl va contenir la liste des attributs de icl   
    lcl2=icl.getAllInitXAttributes()
    lcl=[]
    list_objet_lecture=[]
    for cl in lcl2:
        if cl.name=='name_u':
            continue
        if cl.name=='is_read':
            continue
        if isinstance(cl,Ref_Tr_Attribute):
            continue
        lcl.append(cl)
        pass
    # lcl contient la liste des attributs de icl
    
    
    lpere=ipere.getAllInitXAttributes()
    clname="{ \\bf "+clname+" } "
    if icl.readacc_==0:
        if icl.name_trio_!='':
            tstr=tstr+clname
            pass
        pass
    else:
        if len(lcl):
            
	    if issubclass(icl,interprete):
	        tstr=tstr+clname+" "
            elif issubclass(icl,objet_lecture):
	        if icl.name_trio_!='':tstr=tstr+clname+" "
	    else:
	        if  icl.name_trio_=='': print "objet lecture???? ",clname,icl.name_trio_
                tstr+=clname+" obj\nLire obj "
                pass
            tstr+="\{  "
            tstr=tstr+'\n\\begin{itemize}\setlength{\itemsep}{0pt}\setlength{\parskip}{0pt}\n'
            pass
        pass

     # pour pas generer la doc dans chaque classe
    lcl_p=[]
    for xattr in lcl:
        if icl.readacc_==10:
            if xattr in lpere:
                continue
        lcl_p.append(xattr)
    
    if len(lcl_p)  :
        tstr2=tstr2+'\n\\begin{itemize}\setlength{\itemsep}{0pt}\setlength{\parskip}{0pt}\n'
        
        for xattr in lcl_p:
            #xattr=lcl[i]
           
           
            
            mot=xattr.name
            
            if fr:
                mot=xattr.name_trio
                pass
            if icl.readacc_:
                tstr2+=' \index{ {\\bf '+mot+' } } '
                pass
            for syn in xattr.syno:
                mot+="|"+syn
                if icl.readacc_:
                    tstr2+=' \index{ {\\bf  '+syn+' } } '
                pass
            mot='{ \\bf '+ mot+ ' }' 
            motcomprend='\item '+ mot
            her=""
            if icl.readacc_==0:
                motcomprend='\item '+mot
                #her=" ("+mot+")"
            else:
                if xattr in lpere:
                    her=her+' for inheritance'
                    pass
                pass
            # debut de la determination du type 
            #type=icl.liste_type_[i]
            into=None
            the_type=xattr.xtype
            try:
                the_type=the_type.xtypes[1]
            except:
                pass    
            try:
                cl=the_type.classes
                # print cl
                if issubclass(cl[0],objet_u):
                    if issubclass(cl[0],objet_lecture) or issubclass(cl[0],listobj):
                        list_objet_lecture.append(cl)
                        pass
                    if fr:
                        
                        stype=cl[0].name_trio_
                        stype2=cl[0].name_
                        pass
                    else:
                        stype=cl[0].name_
                        stype2=stype
                        pass
                    pass
                # print xattr.name,the_type.classes
                pass
            except:
                try:
                    into=the_type.into
                except:
                    into=None
                    pass
                stype=the_type.__python__type__.__name__
                stype2=stype
                pass
            if 1:
                if (xattr.xtype==Listfloat_f):
                    stype="x1 x2 (x3)"
                    stype2="Listfloat_f"
                elif (xattr.xtype==Listfloat):
                    stype="n x1 x2 ... xn"
                    stype2="Listfloat"                
                elif (xattr.xtype==Listentier_f):
                    stype="n1 n2 (n3)"
                    stype2="Listentier_f"
                elif (xattr.xtype==Listentier):
                    stype="n n1 n2 ... nn"
                    stype2="Listentier"
                elif (xattr.xtype==Listchaine_f):
                    stype="word1 word2 (word3)"
                    stype2="Listentier_f"
                elif (xattr.xtype==Listchaine):
                    stype="n word1 word2 ... wordn"
                    stype2="Listentier"                
                    pass
                pass
            if into:
                stype=stype+" into "+str(into)
                for c in into:
		    if isinstance(c,str):
                       stype+='\index{ '+str(c)+' }'
                pass
            #print "ici", xattr.name,stype2,name,stype
            
            gen_ref=1    
            try:
                strtype=dicoclass.keys()[dicoclass.values().index(stype2)]
            except:
                if stype2=="list":
                    print "list non code", xattr.name,stype2,name,stype
                    raise Exception("on ne doit plus passer la")
                
                # print "pas dans dico" , stype
                gen_ref=0
                strtype=stype
                pass
            if (xattr.xtype!=rien) and (gen_ref==1):
                her=" (\\ref{"+replace(stype2,'_','')+"})"+her
		pass
            if strtype=='':
                raise Exception("on ne doit plus passer la")
                # print "ici",xattr.name,stype2,name,stype
                strtype=stype2
                pass
            strtype='{ \it '+strtype +'}'
            if (xattr.xtype==rien):
                strtype=""
                stype=""
                #print "ooooo", tstr2
                pass
            # fin de la determination du tyoe de l'attribut
            if fr:
                tstr2=tstr2+ motcomprend+strtype+her
                if icl.readacc_:
                    if xattr.optional:
                        tstr=tstr+"\item[] ["+mot+' '+strtype+']\n'
                    else:
                        tstr=tstr+"\item[] "+mot+' '+strtype+'\n'
                        pass
                else:
                    if xattr.optional:
                        tstr=tstr+' ['+mot+']'
                    else:
                        tstr=tstr+' '+mot
                        pass
                    pass
            else:
                tstr2=tstr2+ motcomprend+stype+her
                tstr=tstr+"\item[] "+mot+" "+stype
                pass
            if xattr.descr_!='not_set':
                tstr2+=': '+replace(replace(xattr.descr_,'{','\{'),'}','\}')
                pass
            tstr2=tstr2+'\n'
            pass
        if icl.readacc_:
            tstr=tstr+'\end{itemize} \} \n\n where \n '
        else:
            tstr2='\n \n where'+'\n'+tstr2
            pass
        tstr2=tstr2+'\end{itemize} \n '
        
        tstr+=tstr2
    else:
        tstr+='\n\n ' 
        pass
    # print tstr
    tstr=tstr.replace("NL1",'\n')
    tstr=tstr.replace("NL2",'\\newline\n')
    if len(list_objet_lecture):
        # print "list ",             list_objet_lecture
        pass
    for obj in list_objet_lecture:
        #print obj[0]
        #print "totot" ,obj[0].name_
        ob_lec=gen_doc(obj[0].name_,doc_gen,fr,l,niveau)
        tstr+=ob_lec
        pass

    if issubclass(icl,objet_lecture) or issubclass(icl,listobj)  :
        # on genere ici toutes les classes filles:
        for icl in listf:
            ob_lec=gen_doc(icl.name_,doc_gen,fr,l,niveau)
            tstr+=ob_lec
            pass
        pass
    niveau-=1
    return tstr
    pass

def gen_doc_base(fr=1):
    " genere toute la doc"
    lutil=None
    if (fr==3):
	fr=1
        from lutil import lutil
        pass

    doc_gen={}
    listxc=[]
    for  cla in  getXClasses():
        listxc.append(cla.name_)
        pass
    for cla in dicoclass.values():
        doc_gen[cla]=0
        if not cla in listxc:
            print cla,"not in listxc"
        pass
    s1=open('doc.tex','w')

    s1.write(r'''\newif\ifppdf 
\ifx\pdfoutput\undefined 
  \ppdffalse 
\else 
 \pdfoutput=1 
  \ppdftrue 
\fi 
\batchmode
\documentclass{article}[11pt]
\usepackage[T1]{fontenc}
\usepackage{times}
\usepackage{xcolor}
\definecolor{darkblue}{HTML}{3535B4}
\usepackage{makeidx}
\usepackage{array}
\usepackage{longtable}
\usepackage{amsmath}
\newcommand\normalsubformula[1]{\text{\mathversion{normal}$#1$}}

\newcommand\includepng[2]{{\begin{figure}[h!]\begin{center}\includegraphics[width=#2cm]{#1}\end{center}\end{figure}}}
\newcommand\includetabfig[4]{{\begin{center} \begin{tabular}{cc} \includegraphics[width=#2cm]{#1} & \includegraphics[width=#4cm]{#3} \tabularnewline \end{tabular} \par\end{center}}}
\newcommand\jolitheta{\theta}

\ifppdf \usepackage[pdftex,pdfstartview=FitH,colorlinks=true,linkcolor=blue,urlcolor=darkblue]{hyperref} 
\pdfcompresslevel=9 
\usepackage[pdftex]{graphicx}
\AtBeginDocument{
    \let\Oldlabel\label
    \renewcommand{\label}[1]{\hypertarget{#1}{} \Oldlabel{#1}}
}

\hypersetup{pdftitle={TRUST Reference Manual},
 pdfauthor={Team TRUST},
 pdfsubject={TRUST Reference Manual},
 pdfkeywords={TRUST, guide, howto}
}

\else 
\usepackage[colorlinks=true,linkcolor=blue]{hyperref}
\fi
\usepackage{a4wide} 
\setlength{\parindent}{0pt} 
\makeindex 
 \begin{document}
\title{
\vspace{5cm} 
\Huge \textbf{TRUST Reference Manual V1.7.5} \vspace{0.5cm}}
\author{
\vspace{2cm} 
\LARGE \textbf{Support team: \href{mailto:triou@cea.fr}{triou@cea.fr}} \\
\vspace{4cm} % espace entre ref manual et ce qui est en dessous
Link to: \LARGE \textbf{\href{run:TRUST_Generic_Guide.pdf}{ TRUST Generic Guide}}\\
 }
\maketitle
\newpage
\tableofcontents
\input{parser.tex}
\section{Existing \& predefined fields names\label{fieldsnames}}
Here is a list of post-processable fields, but it is not the only ones.
\input{fieldnames.tex}
''')
    

    l=[]
    for name in dicobases.keys():
        if lutil:
            # on verifie qu'au moins une classe fille est dans lutil
             
            if name in lutil:
            
                l.append(name)
                continue

            for cl in dicobases[name]:
                if cl in lutil:
                    print name
                    l.append(name)
                    break
                pass
            pass
        else:
            l.append(name)
            pass
        
        pass
    l.sort()


    # on change la liste pour mettre d'abord et dans cette ordre les classes suivantes
    list_prefere=['interprete','pb_gen_base','mor_eqn']
    list_p2=[]
    for x in list_prefere:
        if x in l:
            l.remove(x)
            list_p2.append(x)
            pass
        pass
    l=list_p2+l
    
   
    if 'listobj_impl' in l:
        l.remove('listobj_impl')
        l.append('listobj_impl')
    if 'objet_lecture' in l:
        l.remove('objet_lecture')
        l.append('objet_lecture')
    for name in l:
        s1.write('\input{'+name+'}\n')
        tstr="\section{"
        if (fr==1):
            # faux!!!
            tstr=tstr+dicoclass.keys()[dicoclass.values().index(name)]
            #tstr=tstr+dicoclass[name]
        else:
            tstr=tstr+name
            pass

        # namecl=dicoclass[name]
        namecl=name
        try:
            tstr=tstr+gen_doc(namecl,doc_gen,fr,lutil)
	except:
            print "generation de la doc de "+name+" rate"
            gen_doc(namecl,doc_gen,fr,lutil)
            raise Exception("doc rate")
        print"gen_doc_base ",name
        for l in dicobases[name]:
            # print "l",l
            if l!=name:
                tstr=tstr+gen_doc(l,doc_gen,fr,lutil)
                pass
            pass
        tstr=replace(tstr,"\'{\'","\'\{\'") 
        tstr=replace(tstr,"\'}\'","\'\}\'") 
        tstr=replace(tstr,"\\xe9","e") 
        s=open(name+".tex",'w')
        str2=replace(tstr,'_','\-\_')
	str2=replace(str2,'#','\#')
        s.write(str2)
        s.close()
        pass
    
    s1.write('\section{index\label{objetu}} \printindex\n \end{document}\n')
    s1.close()
    if (lutil):
	    for cla in getXClasses():
		    cl=cla.name_
		    if cl not in lutil:
			    print "La doc pour ",cl," n a pas ete genere"
			    pass
		    pass
       
    pass

def copy(a):
    " renvoie une copie de a"
    print "copy deb"
    cmd="c="+a.print_py()
    exec(cmd)
    print "copy fin"
    return c

def change_type(inst,new_type):
    " renvoie une instance de type new_type, initialisee avec les valeurs de inst"
    cmd="pr="+new_type.name_+"()"
    exec(cmd)
    list1=[ x.name for x in inst.__init__xattributes__ ]
    list2=[ x.name for x in pr.__init__xattributes__ ]
    list0=[]
    for x in list2:
	    if x not in list0:
		    list0.append(x)
    for attr in list1:
        if attr in list2:
            try:
                setattr(pr,attr,getattr(inst,attr))
                pass
            except:
                pass
            pass
        else:
            print "sauvegarde", attr
            setattr(pr,"saattr_"+attr,getattr(inst,attr))
            pass
        pass
    for attr in list2:
        if attr not in list1:
            if hasattr(inst,"saattr_"+attr):
                print "recup sauvegarde",attr
                setattr(pr,attr,getattr(inst,"saattr_"+attr))
                pass
            pass
        pass
    listm=dir(inst)
    listm2=[]
    for mot in listm:
        if mot[0:7]=="saattr_" and mot[7:] not in list2:
            listm2.append(mot)
            pass
        pass
    print "listm2",listm2
    for mot in listm2:
        setattr(pr,mot,getattr(inst,mot))
        pass
    print pr.print_py()
    return pr

