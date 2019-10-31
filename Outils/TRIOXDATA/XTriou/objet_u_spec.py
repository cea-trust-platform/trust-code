###
# objets speciaux: read, discretize,associer , comment , pb_ft_discr
#
###


def read_data_associer(self,chaine,niveau,listclass=[]):
    interprete.read_data(self,chaine,niveau,listclass)
    self.associer(listclass)
    # return chaine
def associer_associer(self,listclass):
    #mot=self.list_val_[0]
    #print mot
    cl1=trouve_class_list(self.objet_1,listclass)
    cl2=trouve_class_list(self.objet_2,listclass)
    # print "cl1",cl1,"cl2",cl2
    ok=None
    for ref in cl1.get_list_ref():
        #print ref
        clref=ref.type_ref
        cmd="isi=isinstance(cl2,"+clref+")"
        exec(cmd)
        if isi:
            if getattr(cl1,ref.name)==None:
                ok=ref
                break
            pass
        pass
    if ok==None:
        msg="on ne peut pas associer "+str(self.objet_2)+" de type " +str(cl2)+" comme ref de "+str(self.objet_1)+" de type "+str(cl1)
        msg=msg+"qui connait "+str([ xprop.type_ref for xprop in cl1.get_list_ref()] )
        # msg="Erreur dans le jdd associer"+str(self.list_val_[1])
        raise Exception(msg)
    # raise Exception("Erreur dans le jdd associer")
    else:
        #ref[2]=self.list_val_[1]
        setattr(cl1,ref.name,self.objet_2)
        #print cl1.list_ref_
        #print cl1.print_py()
        # raise Exception(" on arrete expres")
        pass
    # on remet la classe a la fin pour etre sur
    # que la classe qu'on associe soit ecrite avant
    # en particulier quand pb est declare avant domaine
    # mais que dans le cas Associer
    if (self.__class__.name_=="associate"):
        listclass.remove(cl1)
        listclass.append(cl1)
        pass
    return

def print_lu_associer(self,listclass=[]):
    if (self.__class__.name_=="associate"):
        d=""
        return d
    else:
        return interprete.print_lu(self,listclass)
    pass

associate.read_data=read_data_associer
associate.associer=associer_associer
associate.print_lu=print_lu_associer


def print_lu_discretize(self,list_class=[]):
    # on regarde si on n'a pas des classes a ecrire avant
    # c a d des classes qui  attendent discr mais qui n'ont pas
    # ete ecrite a cause des associer
    stri=""
    i0=-1
    if (self in list_class):
        i0=list_class.index(self)
        pass
    # pour eviter d ecrire les pbs avant le discretize
    for i in range(i0):
        cl=list_class[i]
        if (isinstance(cl,discretize)):
            cl.mode_ecr=0
            pass
        pass
    pb=trouve_class_list(self.problem_name,list_class)
    stri=stri+pb.print_lu(list_class)
    stri=stri+interprete.print_lu(self,list_class)
    # on remet le bon flag aux discretize
    for i in range(i0):
        cl=list_class[i]
        if (isinstance(cl,discretize)):
            cl.mode_ecr=8
            pass
        pass
    # on ecrit les classes precedentes attendant discrtiser
    if (self in list_class):
        for i in range(i0):
            stri=stri+list_class[i].print_lu(list_class)
            pass
        pass
    return stri

def read_val_pb_ft(self,chaine2,niveau,listdata,mottest):
    # print "coucou"
    list_ref= self.get_list_ref();
    lmottest=lower(mottest)
    for ref in list_ref:
        val=getattr(self,ref.name)
        if val!=None:
            # print list_class
            if isinstance(val,objet_u):
                clref=val
            else:
                clref=trouve_class_list(val,listdata)
                pass

            if lmottest == lower(clref.name_u):
                # print clref
                clref.read_data(chaine2,niveau,listdata)
                listdata.remove(clref)
                listdata.append(clref)
                return
            pass
        pass
    # 1/0
    return Pb_base.read_val(self,chaine2,niveau,listdata,mottest)
def print_lire_pb_ft(self,listdata=[]):
    toto=Pb_base.print_lire(self,listdata)
    ok=1
    if toto == "":
        ok=0
        toto="  Lire " +self.name_u+ " { }"
        pass
        #return toto
    il=toto.index('{')+1
    #print "ici", self,toto[:il]

    list_ref= self.get_list_ref();
    ajout=""
    for ref in list_ref:
        val=getattr(self,ref.name)
        if val!=None:
        # print list_class
            if isinstance(val,objet_u):
                clref=val
            else:
                clref=trouve_class_list(val,listdata)
                pass
            new=clref.print_lu(listdata)
            if new:
                #print clref.name_u
                # on recupere la chaine la ou commence name_u
                ajout+=new[new.index(clref.name_u):]
                pass
            pass
        pass
    # on garde le debit Lire PPPP { , on ajoute ajout et la suite .
    if (ok) or (ajout!= ""):
        titi=toto[:il]+" "+ajout+" "+toto[il:]
    else:
        titi=""
    pass
    return titi
def get_list_ref_ft_disc_gen(self):
    """ pour assurer que les equations sont associes en premier """
    xatt=self.__class__.getAllInitXAttributes()
    list=[]
    listeq=[]
    for xprop in xatt:
        if isinstance(xprop,Ref_Tr_Attribute):
            if (xprop.type_ref=='eqn_base'):
                listeq.append(xprop)
            else:
                list.append(xprop)
                pass
            pass
        pass
    for ref in list:
        listeq.append(ref)
        pass

    return listeq


# GF changement introduction de la classe mere problem_read_generic
try:
    problem_read_generic.read_val=read_val_pb_ft
    problem_read_generic.print_lire=print_lire_pb_ft
    problem_read_generic.get_list_ref=get_list_ref_ft_disc_gen
except NameError:
    print("problem_read_generic not defined  ... pass")
    pass
read.read_data=read_data_read
try:
    discretize.print_lu=print_lu_discretize
except NameError:
    print("discretize not defined  ... pass")
    pass

def comment_read_data(self,chaine,niveau,listdata=[]):
    comm,chaine=read_bloc_comm(chaine)
    setattr(self,"comm",comm)

    #        print "comm_read",chaine
    return chaine
def comment_print_lu(self,ll=[]):
    if (self.mode_ecr<2):
        self.mode_ecr=2
        return "# "+str(self.comm)+" #\n"
    else:
        return ""
    pass

comment.read_data=comment_read_data
comment.print_lu=comment_print_lu

def bloc_comment_read_data(self,chaine,niveau,listdata=[]):
    comm,chaine=read_bloc_comm_entier(chaine)
    setattr(self,"comm",comm)
    #        print "comm_read",chaine
    return chaine
def bloc_comment_print_lu(self,ll=[]):
    if (self.mode_ecr<2):
        self.mode_ecr=2
        return "/* "+str(self.comm)+" */\n"
    else:
        return ""
    pass

bloc_comment.read_data=bloc_comment_read_data
bloc_comment.print_lu=bloc_comment_print_lu
coupled=True
try:
    coupled_problem.print_associer_org=coupled_problem.print_associer
    coupled_problem.print_lire_org=coupled_problem.print_lire
except NameError:
    coupled=False
def probleme_couple_print_lire(self,list_class):
    return ""
def probleme_couple_print_associer(self,list_class):
    if (self.groupes):
        a=""
        for g in self.groupes.listobj:
            for pb in g.listobj:
                val=pb.mot
                if isinstance(val,objet_u):
                    clref=val
                else:
                    clref=trouve_class_list(val,list_class)
                    pass
                a=a+clref.print_lu(list_class)
                pass
            pass
        a+= '\n'+self.print_lire_org(list_class)
        a+= '\n'+self.print_associer_org(list_class)
        return a
    else:
        return  self.print_associer_org(list_class)
    pass
if coupled:
    coupled_problem.print_associer=probleme_couple_print_associer
    coupled_problem.print_lire=probleme_couple_print_lire
    pass
