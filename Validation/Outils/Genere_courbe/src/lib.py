#script pour generer un fichier des sequences d'appel
# -*- coding: UTF-8 -*-
#****************************************************************************
# Copyright (c) 2015 - 2016, CEA
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:
# 1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
# 2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
# 3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
# IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
# OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#
#*****************************************************************************

import sys, traceback

#imports locaux

__version__ = '0.1'
__date__    = '20061207'

dico_en={
'parameterstrio_u':'parametrestrio_u',
'parameterstrust':'parametrestrio_u',
'versiontrust':'versiontrio_u',
'title_figure':'titre_figure',
'include_description_curves':'incluredesccourbes',
'nb_columns_file':'nb_colonnes_fichier',
'nb_columns':'nb_colonnes',
'include_description_lines':'incluredesclignes',
'last_line':'derniere_ligne',
'display_line':'afficher_ligne',
'number_first_line':'numero_premiere_ligne',
'number_last_line':'numero_derniere_ligne',
'chapter':'chapitre',
'parameters':'parametres',
'testcase':'castest',
'includedata':'incluredata',
'prerequisite':'prerequis',
'author':'auteur',
'title':'titre',
'picture':'image',
'table':'tableau',
'curve':'courbe',
'legend':'legende',
'origin':'origine',
'columns':'colonnes',
'axis':'axes',
'linewidth':'largeurligne',
'isovalues':'isovaleurs',
'formula':'formule',
'line':'ligne',
'lines':'lignes',
'file':'fichier',
'values':'valeurs',
}


class GestionMessages:
    #drapeau des messages
    _ERR = 0
    _AVERT = 1
    _INFO = 2
    _DEBOG = 3
    __drapeau = ['ERROR', 'WARNING', 'INFO', 'DEBOG']

    def __init__(self, verbose=10, output='', ecran=True):
        self.setNiveauMessage(verbose)
        self.setOutputFile(output)
        self.ecran = ecran

    def setNiveauMessage(self, verbose):
        self.NiveauMax = verbose
    def setOutputFile(self, output):
        self.OutputFile = output
        if output!='':
            self.OutputFileF = open(self.OutputFile, 'w')

    #@staticmethod

    def ecrire(self, criticite, message, arret=False, usage=False, niveau=0, texteUsage='',fichier=None):
        '''Procedure d ecriture d un message.'''
        #print 'ecrire %s criticite=%d, niveau=%d, niv max=%d' % (message, criticite, niveau, self.NiveauMax)
        if criticite>len(GestionMessages.__drapeau):
            criticite = 0
        #en cas d'erreur : demande l'arret du code
        if criticite==GestionMessages._ERR:
            arret = True
        #test l'affichage du message, selon le niveau de verbosite
        if (criticite in (GestionMessages._ERR,GestionMessages._AVERT)) or niveau<=self.NiveauMax:
            if self.ecran:
                print('%s : %s' % (GestionMessages.__drapeau[criticite], message))
            if self.OutputFile!='':
                self.OutputFileF.write('%s : %s\n' % (GestionMessages.__drapeau[criticite], message))
            if criticite==GestionMessages._ERR:
                callStack = traceback.extract_stack()
                frame = callStack[-2]
                if frame[2]=="ecrire_usage" or frame[2]=="verifie_accolade_suivante":
                    frame = callStack[-3]
                if self.ecran:
                    print('%s  :  Localisation = fonction %s (fichier %s, ligne %d) : \n\t%s' % (GestionMessages.__drapeau[criticite], frame[2], frame[0], frame[1], frame[3]))
                if self.OutputFile!='':
                    self.OutputFileF.write('%s  :  Localisation = fonction %s (fichier %s, ligne %d) : \n\t%s\n' % (GestionMessages.__drapeau[criticite], frame[2], frame[0], frame[1], frame[3]))
                    if (fichier):
                        toto=fichier.readlines()
                        nb_l=len(toto)
                        fichier_0=open(self.parametersFile, 'r')
                        toto=fichier_0.readlines()
                        nb_l0=len(toto)
                        curentline=nb_l0-nb_l
                        message_info="Error at the line %s of %s"%(curentline,self.parametersFile)
                        if self.ecran:
                            print('%s : %s' % (GestionMessages.__drapeau[criticite], message_info))
                        if self.OutputFile!='':
                            self.OutputFileF.write('%s : %s\n' % (GestionMessages.__drapeau[criticite], message_info))
                            pass
                        pass
        #imprime l'usage si besoin
        if texteUsage!='':
            if self.ecran:
                print(texteUsage)
            if self.OutputFile!='':
                self.OutputFileF.write('%s\n' % (texteUsage))
        if self.OutputFile!='':
            self.OutputFileF.flush()
        sys.stdout.flush()
        #provoque l'arret du code si besoin
        if arret:
            sys.exit(2)
            pass
        pass
    def ecrire_usage(self, criticite, what, dico, motcle, arret=False, usage=False, niveau=0, texteUsage='',fichier=None):
        # construction dico_inverse
        dico_inverse={}
        for m in dico_en:
            dico_inverse[dico_en[m]]=m

        lst=[]
        for m in dico:
            mot=""
            if m in list(dico_inverse.keys()): 
                mot=dico_inverse[m]+"|"
            mot+=m
            lst.append(mot)
        message='Unknown parameter. We were expecting a parameter of %s  like %s, and not %s' %(what,lst,motcle)

        return self.ecrire(criticite, message, arret, usage, niveau, texteUsage,fichier)
#GestionnaireMessages = GestionMessages()

#renvoie le nom de la fonction appelante
def getNomFonction():
    '''Renvoie le nom de la fonction appelant cette fonction.'''
    return sys._getframe(1).f_code.co_name
#renvoit le nom moins  le .data
def get_detail_cas(cas0):
    ''' renvoir le nom du cas dans le .data,le directory, le nombre de proc, le commmentaire éventuel'''
    cas = cas0.strip()
    cas = cas.replace('"',' ')
    cas=" ".join(cas.split())
    cas=cas.split()
    case=get_nom_cas(cas[1])
    dir=cas[0]
    nb_proc=1
    comment=""
    debut=2
    if (len(cas)>debut):

        # a t on defini nb_proc
        try:
            nb_proc_p=eval(cas[2])
            from types import IntType
            if (type(nb_proc_p) is IntType):
                debut=3
                nb_proc=nb_proc_p
            else:
                pass
        except:
            pass
        pass
    if (len(cas)>debut):
        for i in range(debut,len(cas)):
            comment+=cas[i]+" "
            pass
        pass
    return case,dir,nb_proc,comment
def get_nom_cas(case):
    ''' renvoie case prive de .data si il y a lieu '''
    if case[-5:]=='.data':
        return  case[:-5]
    else:
        return case
    pass

def print_description(description,dec):
    ''' affiche la description, en mettant EOF si nécessaire'''
    for des in description:
        if des.find('\n')!=-1:
            lignes=des.split('\n')
            print(dec,"description", lignes[0], "<<EOF")
            for l in lignes[1:-1]: print(l)
            print(lignes[-1],"EOF")
        else:
            print(dec,"description", des)
    pass
#extraction du couple mot-cle / valeur d'un ligne
def extraireMotcleValeur(fichier,ligne, gestMsg):
    '''Renvoie 2 elements de type motcle,valeur : le 1er mot de la chaine passee, suivie du reste de la chaine.'''
    ligneT = ligne.split(' ',1)
    motcle_lu=ligneT[0].lower()
    if motcle_lu in list(dico_en.keys()):
        motcle=dico_en[motcle_lu].lower()
    else:
        motcle=motcle_lu
        pass
    if len(ligneT)==2:
        gestMsg.ecrire(GestionMessages._DEBOG, 'lecture MC=%s, valeur=%s' % (motcle_lu,ligneT[1]), niveau=15)
        resu=ligneT[1]
        # on regarde dans le cas d'une description, si la ligne finie par <<EOF
        # on se limite aux descriptions, pour faire facilement le printFichierParamatres
        if ((motcle=="description") and (resu.strip().split(' ')[-1].upper()=='<<EOF')):
            resu=' '.join(resu.split(' ')[:-1])+'\n'
            trouve=0
            while (trouve==0):
                ligne=fichier.readline()
                if not ligne:
                    gestMsg.ecrire(GestionMessages._ERR, 'Unexpected end of file. A line finished by EOF was expected.')
                    pass
                LL=ligne.strip().split(' ')
                if (LL[-1].upper()=="EOF"):
                    resu+=' '.join(LL[:-1])
                    trouve=1
                else:
                    resu+=ligne
                    pass

                # print ligne, "la", resu
                pass

            pass
        else:
            resu=resu.strip()
            pass
        return motcle,resu,motcle_lu
    else:
        gestMsg.ecrire(GestionMessages._DEBOG, 'Reading MC=%s' % (motcle), niveau=15)
        return motcle,'',motcle_lu

#constantes
_accoladeF = '}'

def verifie_accolade_suivante(ligne,fichier,gestMsg):
    err=""
    #print "cocou ", ligne.split()[1:], len(ligne.split())
    if (ligne.split()[1:]!=["{"]):
        if len(ligne.split())==1:
            ligne = fichier.readline()
            ligne = ligne.strip()
            if ligne!="{":
                err="the following line does not have a { but "+ligne
                pass
            pass
        else: err="to begin the block, we expected { followed by a carriage return but not " +ligne
        if err!="":
            print(err)
            gestMsg.ecrire(GestionMessages._ERR,err,fichier=fichier)
            pass
        pass
    pass
def chaine2Ascii(chaine):
    res = chaine.replace('"','')
    res = res.replace('à','a')
    res = res.replace('â','a')
    res = res.replace('é','e')
    res = res.replace('è','e')
    res = res.replace('ê','e')
    res = res.replace('ë','e')
    res = res.replace('î','i')
    res = res.replace('ï','i')
    res = res.replace('ô','o')
    res = res.replace('ö','o')
    res = res.replace('ù','u')
    res = res.replace('û','u')
    res = res.replace('ü','u')
    res = res.replace('ç','c')
    res = res.replace('_',' ')
    return res
def chaine2File(titi):
    ''' ajoute des antislash pour que la chaine soi correctement ecrise dans le fichier '''
    titi2=titi.replace('\a','\\a')
    titi2=titi2.replace('\b','\\b')
    titi2=titi2.replace('\f','\\f')
    titi2=titi2.replace('\r','\\r')
    titi2=titi2.replace('\t','\\t')
    titi2=titi2.replace('\v','\\v')
    #titi2=titi2.replace('\'','\\\'')

    #titi2=titi2.replace('\x','\\x')
    #print titi2
    #titi2=(titi2.replace('\\\\','\\\\\\\\'))

    #titi2=titi2.replace('\b','\\b')
    #print "lll" ,titi2
    return titi2
def read_value_in_file(value,file):
    import os,sys
    # print dir(sys.path)
    sys.path.append(".")
    if not(os.path.exists(file+".py")):
        raise Exception("No such file or directory: '"+file+".py'")
    exec("from "+file+" import "+value)

    exec("res="+value)

    return str(res)
def replace_value_in_file(chaine):
    separateur_debut='read_value_in_file('
    separateur_fin=')'
    deb=chaine.find(separateur_debut)
    if deb!=-1:
        res=chaine[:deb]
        suite=chaine[deb+len(separateur_debut):]
        fin=suite.find(separateur_fin)
        if fin==-1:
            msg=separateur_fin+" non trouve dans "+chaine
            raise Exception(msg)
        cmd="res+="+separateur_debut+suite[:fin+1]
        # print "read..." ,cmd
        exec(cmd)

        res+=suite[fin+len(separateur_fin):]
        return res
    return chaine
def chaine2Tex(chaine):
    separateur_debut='\latex_('
    separateur_fin='\latex_)'
    deb=chaine.find(separateur_debut)
    if deb!=-1:
        res=chaine2Tex(chaine[:deb])
        suite=chaine[deb+len(separateur_debut):]
        fin=suite.find(separateur_fin)
        if fin==-1:
            msg=separateur_fin+" non trouve dans "+chaine
            raise Exception(msg)
        res+=chaine2File(suite[:fin])
        res+=chaine2Tex(suite[fin+len(separateur_fin):])
        return res
    separateur_debut='triokeyword('
    separateur_fin=')'
    deb=chaine.find(separateur_debut)
    if deb!=-1:
        res=chaine2Tex(chaine[:deb])
        suite=chaine[deb+len(separateur_debut):]
        fin=suite.find(separateur_fin)
        if fin==-1:
            msg=separateur_fin+" non trouve dans "+chaine
            raise Exception(msg)
        res+='{ \\bf '
        res+=chaine2Tex(suite[:fin])
        res+=' } '
        res+=chaine2Tex(suite[fin+len(separateur_fin):])
        return res
    separateur_debut='read_value_in_file('
    separateur_fin=')'
    deb=chaine.find(separateur_debut)
    if deb!=-1:
        res=chaine2Tex(chaine[:deb])
        suite=chaine[deb+len(separateur_debut):]
        fin=suite.find(separateur_fin)
        if fin==-1:
            msg=separateur_fin+" non trouve dans "+chaine
            raise Exception(msg)
        cmd="res+="+separateur_debut+suite[:fin+1]
        # print "read..." ,cmd
        exec(cmd)

        res+=chaine2Tex(suite[fin+len(separateur_fin):])
        return res
    separateur_debut='include_text_file('
    separateur_fin=')'
    deb=chaine.find(separateur_debut)
    if deb!=-1:
        res=chaine2Tex(chaine[:deb])
        suite=chaine[deb+len(separateur_debut):]
        fin=suite.find(separateur_fin)
        if fin==-1:
            msg=separateur_fin+" non trouve dans "+chaine
            raise Exception(msg)
        #cmd="res+="+separateur_debut+suite[:fin+1]
        #print "read..." ,cmd
        #exec(cmd)


        mots=suite[:fin].split(",")
        res+="\n\lstdefinelanguage{perso}{morekeywords={"
        res+=",".join(mots[1:]).replace('"','')
        res+="}, sensitive=false}"
        res+="\n\lstset{\n basicstyle=\small, numbers=none, tabsize=2, extendedchars=true, linewidth=16cm, emptylines=0, breaklines=true,language=perso}\n"
        import os.path
        print(os.path.abspath(os.curdir ))
        ff=mots[0].replace("\"","")
        if not(os.path.exists(ff)):
            print("in prm, incluce_text_file can't read data file :",ff)


        res+="\lstinputlisting{\orig/%s}\n"%mots[0]
        res+=chaine2Tex(suite[fin+len(separateur_fin):])
        return res
    res = chaine.replace('"','')
    res = res.replace('{','\{')
    res = res.replace('}','\}')
    res = res.replace('à','\`{a}')
    res = res.replace('â','\^{a}')
    res = res.replace('é','\\\'{e}')
    res = res.replace('è','\`{e}')
    res = res.replace('ê','\^{e}')
    res = res.replace('ë','\"{e}')
    res = res.replace('î','\^{i}')
    res = res.replace('ï','\"{i}')
    res = res.replace('ô','\^{o}')
    res = res.replace('ö','\"{o}')
    res = res.replace('ù','\`{u}')
    res = res.replace('û','\^{u}')
    res = res.replace('ü','\"{u}')
    res = res.replace('ç','\c{c}')
    res = res.replace('_','\_')
    res = res.replace('%','\%')
    res = res.replace('<','$<$')
    res = res.replace('>','$>$')
    #print 'chaine2Tex "%s" == "%s"' % (chaine, res)
    return res

_TypesReconnus = ['lines', 'points', 'linespoints', 'dots', 'impulses', 'yerrorbars', 'xerrorbars', 'xyerrorbars', 'steps', 'fsteps', 'histeps', 'filledcurves', 'boxes', 'boxerrorbars', 'boxxyerrorbars', 'vectors', 'financebars', 'candlesticks', 'errorlines', 'xerrorlines', 'yerrorlines', 'xyerrorlines', 'pm3d']
#====================================================================================================================

if __name__ == '__main__':
    pass
