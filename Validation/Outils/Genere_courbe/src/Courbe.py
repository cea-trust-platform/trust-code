#classe decrivant une figure
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

import sys, os, shutil

from lib import GestionMessages
from lib import getNomFonction
from lib import extraireMotcleValeur,print_description
from lib import chaine2Ascii
from lib import _accoladeF
from lib import _TypesReconnus
from lib import get_nom_cas

class Courbe:
    '''Classe decrivant une courbe telle qu elle devra etre
tracee dans le rapport de validation de TRUST.'''

    #constructeur
    def __init__(self, verbose=0, output=''):
        '''Constructeur.'''
        if output=='':
            self.gestMsg = GestionMessages(verbose,'log')
        else:
            self.gestMsg = output
        self.verbose = verbose
        #initialisations
        self.legende     = 'Undefined'
        self.origine     = 'trio_u'
        self.version     = 'Undefined'
        self.fichier     = 'Undefined'
        self.plan        = 'Undefined'
        self.segment     = 'Undefined'
        self.points      = 'Undefined'
        self.fonction    = 'Undefined'
        self.gnuplot_command     = 'Undefined'
        self.colonnes    = 'Undefined'
        self.style       = 'Undefined'
        self.typeLigne   = 'Undefined'
        self.typePoints  = 'Undefined'
        self.pointSize   = 'Undefined'
        self.axes        = 'Undefined'
        self.description = []
        self.largeurLigne= 'Undefined'
        pass
    def  printFichierParametres(self):
        dec='\t\t'
        print(dec,"Courbe {")
        dec='\t\t\t'
        if self.legende != 'Undefined' : print(dec,"legende", self.legende)
        print_description(self.description,dec)


        if self.origine != 'trio_u' : print(dec,"origine", self.origine)
        if self.version != 'Undefined' : print(dec,"version", self.version)
        if self.fichier != 'Undefined' : print(dec,"fichier", self.fichier)
        if self.plan != 'Undefined' : print(dec,"plan", self.plan)
        if self.segment != 'Undefined' : print(dec,"segment", self.segment)
        if self.points != 'Undefined' : print(dec,"points", self.points)
        if self.fonction != 'Undefined' : print(dec,"fonction", self.fonction)
        if self.gnuplot_command != 'Undefined' : print(dec,"gnuplot_command", self.gnuplot_command)
        from string import join
        if self.colonnes != 'Undefined' : print(dec,"colonnes", self.colonnes)
        if self.style != 'Undefined' : print(dec,"style", self.style)
        if self.typeLigne != 'Undefined' : print(dec,"typeLigne", self.typeLigne)
        if self.typePoints != 'Undefined' : print(dec,"typePoints", self.typePoints)
        if self.pointSize != 'Undefined' : print(dec,"pointSize", self.pointSize)
        if self.axes != 'Undefined' : print(dec,"axes", self.axes)
        #if self.description != [] : print dec,"description", self.description
        if self.largeurLigne != 'Undefined' : print(dec,"largeurLigne", self.largeurLigne)
        dec='\t\t'
        print(dec,"}")
        pass
    #lecture des parametres de la courbe dans le fichier de parametres
    def lireParametres(self, fichier):
        '''Lecture des parametres de la courbe.'''
        self.gestMsg.ecrire(GestionMessages._DEBOG, 'DEBUT %s.%s' % (self.__class__.__name__, getNomFonction()), niveau=15)
        fin = False
        dico=['legende','origine','version','fichier','plan','segment','points','fonction','gnuplot_command','colonnes','style','typeligne','typepoints','pointsize','axes','largeurligne','description']
        while not fin:
            ligne = fichier.readline()
            if not ligne:
                self.gestMsg.ecrire(GestionMessages._ERR, 'Unexpected end of file. We were expecting parameters of a figure.')
            ligne = ligne.strip()
            if len(ligne)>0 and ligne[0]!='#':
                motcle,valeur,motcle_lu = extraireMotcleValeur(fichier,ligne, self.gestMsg)
                if motcle==_accoladeF:
                    fin = True
                elif motcle=='legende':
                    self.legende = valeur
                elif motcle=='origine':
                    self.origine = valeur
                elif motcle=='version':
                    self.version = valeur
                elif motcle=='fichier':
                    self.fichier = valeur
                elif motcle=='plan':
                    self.plan = valeur
                elif motcle=='segment':
                    self.segment = valeur
                elif motcle=='points':
                    self.points = valeur
                elif motcle=='fonction':
                    self.fonction=valeur
                elif motcle=='colonnes':
                    valeur = valeur.strip()
                    valeurT = valeur.split()
                    if len(valeurT)!=2:
                        self.gestMsg.ecrire(GestionMessages._ERR, 'Error, expected format for %s : %s, and not %s' % (motcle_lu, 'index0 index1', valeur))
                    self.colonnes=valeur
                elif motcle=='style':
                    self.style = valeur
                    style=self.style.split()[0]
                    if not style in _TypesReconnus:
                        self.gestMsg.ecrire(GestionMessages._AVERT, 'Graphic style "%s" is not recognized in the following list: %s. gnuplot may fail.' % (self.style, _TypesReconnus))
                elif motcle=='typeligne':
                    self.typeLigne = valeur
                elif motcle=='typepoints':
                    self.typePoints = valeur
                elif motcle=='pointsize':
                    self.pointSize = valeur
                elif motcle=='axes':
                    self.axes = valeur
                elif motcle=='largeurligne':
                    self.largeurLigne = valeur
                elif motcle=='gnuplot_command':
                    self.gnuplot_command = valeur
                elif motcle=='description':
                    self.description.append(valeur)
                else:
                    self.gestMsg.ecrire_usage(GestionMessages._ERR, 'Curve',dico,motcle_lu,fichier=fichier)
                if motcle!=_accoladeF and not (motcle in dico): print("Missing code for ",motcle);1/0

    #ajout des informations de la courbe a la ligne de commande gnuplot
    def ajouterParametresGnuplot(self, cmd):
        '''Ajout des parametres de la courbes a la ligne de commande gnuplot.'''
        if self.plan!='Undefined':
            #la courbe a tracer est une sonde plan : il faut generer le fichier.coupe
            param = self.plan.split()
            if len(param)!=3 and len(param)!=2:
                self.gestMsg.ecrire(GestionMessages._ERR, 'We expected 2 or 3 values to define a plan probe : path/file.data PROBE_NAME [ time ], and not %s' % (self.plan))
            temps=""
            if len(param)==3:
                temps=param[2]
            self.fichier = get_nom_cas(param[0]) + '_' + (param[1]).upper() + '.coupe'
            ficSon  = get_nom_cas(param[0]) + '_' + (param[1]).upper() + '.plan'
            if temps!="":
                self.fichier=self.fichier+'.'+temps
            #ficPlan = (param[0])[:-5] + '_' + (param[1]).upper() + '.plan'
            #if not os.path.isfile(ficSon) or os.path.getmtime(ficSon)<os.path.getmtime(ficPlan):
            #       if not os.path.isfile(ficPlan):
            #               self.gestMsg.ecrire(GestionMessages._ERR, 'Le fichier de sonde %s ou %s n\'existe pas !' % (ficSon, ficPlan))
            #       else:
            #               shutil.copyfile(ficPlan, ficSon)
            if not os.path.isfile(self.fichier) or os.path.getmtime(self.fichier)<os.path.getmtime(ficSon):
                #si la sonde est plus recente que la coupe : on la regenere

                res = os.system('extrait_coupe %s %s %s' % (param[0], (param[1]).upper(),temps))
            else:
                if (self.verbose>2):
                    self.gestMsg.ecrire(GestionMessages._DEBOG, 'T(%s)=%d  > T(%s)=%d' % (self.fichier, os.path.getmtime(self.fichier),  ficSon, os.path.getmtime(ficSon)))
                    pass
                pass
            pass
        elif self.segment!='Undefined':
            #la courbe a tracer est une sonde segment : il faut generer le fichier.coupe
            param = self.segment.split()
            # print param, self.segment
            if len(param)!=3 and len(param)!=2:

                self.gestMsg.ecrire(GestionMessages._ERR, 'We expected 2 or 3 values to define a segment probe : path/file.data PROBE_NAME [ time ], and not %s' % (self.segment))
            temps=""
            if len(param)==3:
                temps=param[2]

            self.fichier = get_nom_cas(param[0]) + '_' + (param[1]).upper() + '.coupe'
            ficSon = get_nom_cas(param[0]) + '_' + (param[1]).upper() + '.son'
            if temps!="":
                self.fichier=self.fichier+'.'+temps
                # print "DEBG", self.fichier
            if not os.path.isfile(ficSon):
                print("Generating failed File not found",ficSon)
            if not os.path.isfile(self.fichier) or os.path.getmtime(self.fichier)<os.path.getmtime(ficSon):
                #si la sonde est plus recente que la coupe : on la regenere
                res = os.system('extrait_coupe %s %s %s' % (param[0], (param[1]).upper(),temps))
                if not os.path.isfile(self.fichier):
                    print("Generating failed File not found",self.fichier)
            else:
                if (self.verbose>2):
                    self.gestMsg.ecrire(GestionMessages._DEBOG, 'T(%s)=%d  > T(%s)=%d' % (self.fichier, os.path.getmtime(self.fichier),  ficSon, os.path.getmtime(ficSon)))
                    pass
                pass
            pass
        elif self.points!='Undefined':
            #la courbe a tracer est une sonde en points : il faut generer le fichier.points
            param = self.points.split()
            if len(param)!=2:
                self.gestMsg.ecrire(GestionMessages._ERR, 'We expected 2 values to define a probe : path/file.data PROBE_NAME, and not %s' % (self.points))
            self.fichier = self.genererFichierSonde(param[0], param[1])

        if len(cmd)>10:
            cmd += ','

        if self.fonction!='Undefined':
            cmd += ' %s' % (self.fonction)
        else:
            cmd += ' "%s"' % (self.fichier)
            pass
        if self.gnuplot_command!='Undefined':
            cmd += ' %s' % (self.gnuplot_command)

        if self.colonnes!='Undefined':
            valeurT=self.colonnes.split()
            v0 = valeurT[0]
            from lib import replace_value_in_file
            v0=replace_value_in_file(v0)
            if v0.find('$')!=-1:
                v0 = '(%s)' % v0
            v1 = valeurT[1]

            v1=replace_value_in_file(v1)

            if v1.find('$')!=-1:
                v1 = '(%s)' % v1
            colonnes = '%s:%s' % (v0, v1)
            cmd += ' using %s' % (colonnes)
        if self.axes!='Undefined':
            cmd += ' axes %s' % (self.axes)
        if self.legende!='Undefined':
            cmd += ' title "%s"' % (chaine2Ascii(self.legende))
        if self.style!='Undefined':
            cmd += ' with %s' % (self.style)
        if self.typeLigne!='Undefined':
            if self.style=="points":
                print("On a defini des points avec un stylelines..., on ignore")
            else:
                cmd += ' linetype %s' % (self.typeLigne)
        if self.typePoints!='Undefined':
            cmd += ' pointtype %s' % (self.typePoints)
        if self.pointSize!='Undefined':
            cmd += ' pointsize %s' % (self.pointSize)
        if self.largeurLigne!='Undefined':
            cmd += ' linewidth %s' % (self.largeurLigne)

        return cmd



    #---------------------------------------------
    # Methodes d'affichage des infos
    def afficherParametres(self):
        '''Affichage des parametres de la courbe.'''
        self.gestMsg.ecrire(GestionMessages._INFO, '\t\tLegende=  %s' % self.legende)
        self.gestMsg.ecrire(GestionMessages._INFO, '\t\tOrigine=  %s' % self.origine)
        self.gestMsg.ecrire(GestionMessages._INFO, '\t\tVersion=  %s' % self.version)
        self.gestMsg.ecrire(GestionMessages._INFO, '\t\tFichier(col)=  %s (%s)' % (self.fichier, (self.colonnes)))
        self.gestMsg.ecrire(GestionMessages._INFO, '\t\tStyle=  %s /TypeLigne= %s /TypePoint= %s' % (self.style, self.typeLigne, self.typePoints))
