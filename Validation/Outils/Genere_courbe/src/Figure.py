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

import sys, os

from Courbe import Courbe

from lib import GestionMessages
from lib import getNomFonction
from lib import extraireMotcleValeur,print_description
from lib import chaine2Ascii
from lib import _accoladeF,verifie_accolade_suivante

class Figure:
    '''Classe decrivant une figure telle qu elle devra etre tracee dans le rapport de validation de TRUST.'''

    #constructeur
    def __init__(self, verbose=0, output=''):
        '''Constructeur.'''
        if output=='':
            self.gestMsg = GestionMessages(verbose,'log')
        else:
            self.gestMsg = output
        self.verbose = verbose
        #initialisations
        self.titre     = 'Undefined'
        self.titre_figure = 'Undefined'
        self.dimension = 2
        self.rangeX    = 'auto'
        self.rangeY    = 'auto'
        self.labelX    = 'Undefined'
        self.labelY    = 'Undefined'
        self.logX      = False
        self.logY      = False
        self.listeCourbes = []
        self.format = 'png'
        self.pointSize = 'Undefined'
        self.legende = 'Undefined'
        self.grid = 'Undefined'
        self.xtics = 'Undefined'
        self.ytics = 'Undefined'
        self.description = []
        self.inclureDescCourbes = 1
        self.image     = 'Undefined'
        self.width="15cm"
        self.nb_img_without_newline=-1
        self.ratio = 'noratio'
        pass
    def printFichierParametres(self):
        dec='\t'
        print dec,"Figure {"
        dec='\t\t'
        if self.titre != 'Undefined': print dec,"titre", self.titre
        if self.titre_figure != 'Undefined': print dec,"titre_figure", self.titre_figure
        print_description(self.description,dec)

        if self.dimension != 2 : print dec,"dimension", self.dimension
        from string import join
        if self.rangeX != 'auto' : print dec,"rangeX", self.rangeX
        if self.rangeY != 'auto' : print dec,"rangeY", self.rangeY
        if self.labelX != 'Undefined' : print dec,"labelX", self.labelX
        if self.labelY != 'Undefined' : print dec,"labelY", self.labelY
        if self.logX != False : print dec,"logX", self.logX
        if self.logY != False : print dec,"logY", self.logY
        if self.format != 'png' : print dec,"format", self.format
        if self.pointSize != 'Undefined' : print dec,"pointSize", self.pointSize
        if self.legende != 'Undefined' : print dec,"legende", self.legende
        if self.grid != 'Undefined' : print dec,"grid", self.grid
        if self.xtics != 'Undefined' : print dec,"xtics", self.xtics
        if self.ytics != 'Undefined' : print dec,"ytics", self.ytics


        if self.inclureDescCourbes != 1 : print dec,"inclureDescCourbes", self.inclureDescCourbes
        if self.image != 'Undefined' : print dec,"image", self.image
        if self.width != "15cm" : print dec,"width", self.width
        if self.nb_img_without_newline!=-1: print dec, "nb_img_without_newline",self.nb_img_without_newline
        dec='\t'
        for courbe in self.listeCourbes  : courbe.printFichierParametres()
        print dec,"}"
        pass
    #lecture des parametres de la figure dans le fichier de parametres
    def lireParametres(self, fichier):
        '''Lecture des parametres de la figure.'''
        self.gestMsg.ecrire(GestionMessages._DEBOG, 'DEBUT %s.%s' % (self.__class__.__name__, getNomFonction()), niveau=15)
        fin = False
        dico=['titre','titre_figure','dimension','logx','logy','rangex','rangey','labelx','labely','pointsize','legende','grid','xtics','ytics','description','courbe','incluredesccourbes','width','image','nb_img_without_newline','ratio']
        while not fin:
            ligne = fichier.readline()
            if not ligne:
                self.gestMsg.ecrire(GestionMessages._ERR, 'Unexpected end of file. We expected parameters of a figure.')
            ligne = ligne.strip()
            if len(ligne)>0 and ligne[0]!='#':
                motcle,valeur,motcle_lu = extraireMotcleValeur(fichier,ligne, self.gestMsg)
                if motcle==_accoladeF:
                    fin = True
                elif motcle == 'nb_img_without_newline':
                    self.nb_img_without_newline=int(valeur)
                elif motcle=='titre':
                    self.titre = valeur
                elif motcle=='titre_figure':
                    self.titre_figure = valeur
                elif motcle=='dimension':
                    self.dimension = int(valeur)
                elif motcle=='logx':
                    self.logX=True
                elif motcle=='logy':
                    self.logY=True
                elif motcle=='rangex':
                    if valeur.lower()=='auto':
                        self.rangeX = 'auto'
                    else:
                        valeur.strip()
                        valeurT = valeur.split()
                        if len(valeurT)!=2:
                            self.gestMsg.ecrire(GestionMessages._ERR, 'Error, expected format like %s : %s, and not %s' % (motcle_lu, 'valMin valMax', valeur),fichier=fichier)
                        #self.rangeX = '[%f:%f]' % (float(valeurT[0]), float(valeurT[1]))
                        self.rangeX=valeur
                elif motcle=='rangey':
                    if valeur.lower()=='auto':
                        self.rangeY = 'auto'
                    else:
                        valeur.strip()
                        valeurT = valeur.split()
                        if len(valeurT)!=2:
                            self.gestMsg.ecrire(GestionMessages._ERR, 'Error, expected format like %s : %s, and not %s' % (motcle_lu, 'valMin valMax', valeur),fichier=fichier)
                        #self.rangeY = '[%f:%f]' % (float(valeurT[0]), float(valeurT[1]))
                        self.rangeY=valeur
                elif motcle=='labelx':
                    self.labelX = valeur
                elif motcle=='labely':
                    self.labelY = valeur
                elif motcle=='pointsize':
                    self.pointSize = valeur
                elif motcle=='legende':
                    self.legende = valeur
                elif motcle=='grid':
                    self.grid = valeur
                elif motcle=='xtics':
                    self.xtics = valeur
                elif motcle=='ytics':
                    self.ytics = valeur
                elif motcle=='description':
                    self.description.append(valeur)
                elif motcle=='courbe':
                    verifie_accolade_suivante(ligne,fichier,self.gestMsg)
                    courbe = Courbe(verbose=self.verbose, output=self.gestMsg)
                    courbe.lireParametres(fichier)
                    self.listeCourbes.append(courbe)
                elif motcle=='incluredesccourbes':
                    self.inclureDescCourbes = int(valeur)
                elif motcle=='width':
                    self.width=valeur
                elif motcle=='image':
                    self.image = valeur
                elif motcle == 'ratio':
                    self.ratio = valeur
                else:
                    self.gestMsg.ecrire_usage(GestionMessages._ERR, 'Figure', dico,motcle_lu,fichier=fichier)
                if motcle!=_accoladeF and not (motcle in dico): print "Missing code for ",motcle;1/0

        if self.image!='Undefined' and len(self.listeCourbes)>0:
            self.gestMsg.ecrire(GestionMessages._ERR, 'Warning : a figure can not contain a picture AND plots ! (Problem figure "%s")' % self.titre)

    #generation du graphique correspondant a la figure
    def extract_range(self,valeurT):
        ''' pour retrouver les bornes min max des range'''
        bornes=['','']
        for i in [0,1]:
            if (valeurT[i]!='auto'):
                bornes[i]='%s'%float(valeurT[i])
                pass
            pass
        range = '[%s:%s]' % (bornes[0], bornes[1])
        return range
    def genererGraphe(self, dest, indice,debug_figure):
        '''Generation du graphique correspondant a la figure.'''
        if len(self.listeCourbes)>0:
            self.gestMsg.ecrire(GestionMessages._DEBOG, 'DEBUT %s.%s' % (self.__class__.__name__, getNomFonction()), niveau=15)
            nomFichierPlot = 'fic%03d.plot' % (indice)
            nomFichierPlotComplet = dest + '/fic%03d.plot' % (indice)
            ficPlot = open(nomFichierPlotComplet, 'w')
            ficPlot.write('#Fichier de plot correspondant a la figure "%s"\n' % chaine2Ascii(self.titre))
            ficPlot.write('#set encoding iso_8859_15\n')
            if self.pointSize!='Undefined':
                ficPlot.write('set pointsize %s\n' % (self.pointSize))
            titre=None
            if (self.titre_figure!='Undefined'):
                titre=self.titre_figure
            elif (self.titre!='Undefined'):
                titre=self.titre
            if (titre):
                ficPlot.write('set title "%s"\n' % chaine2Ascii(titre))
            ficPlot.write('set autoscale\n')
            if self.rangeX!='auto':
                valeurT=self.rangeX.split()
                range=self.extract_range(valeurT)

                ficPlot.write('set xrange %s\n' % range)
            if self.rangeY!='auto':
                valeurT=self.rangeY.split()
                range =self.extract_range(valeurT)
                ficPlot.write('set yrange %s\n' % range)
            if self.logX:
                ficPlot.write('set log x\n')
                ficPlot.write('set format x "%g"\n')
            if self.logY:
                ficPlot.write('set log y\n')
                ficPlot.write('set format y "%g"\n')
            ficPlot.write('set xlabel "%s"\n' % chaine2Ascii(self.labelX))
            ficPlot.write('set ylabel "%s"\n' % chaine2Ascii(self.labelY))
            if self.legende!='Undefined':
                if self.legende.lower()=='sans':
                    ficPlot.write('set key off\n')
                else:
                    ficPlot.write('set key %s\n' % self.legende)
            if self.grid!='Undefined':
                ficPlot.write('set grid %s\n' % self.grid)
            if self.xtics!='Undefined':
                ficPlot.write('set xtics %s\n' % self.xtics)
            if self.ytics!='Undefined':
                ficPlot.write('set ytics %s\n' % self.ytics)
            if self.ratio != 'noratio':
                ficPlot.write('set size ratio %s\n' % (self.ratio))
            cmd = ''
            if self.dimension==2:
                cmd = 'plot'
            elif self.dimension==3:
                cmd = 'splot'
            else:
                self.gestMsg.ecrire(GestionMessages._ERR, 'Error, impossible dimension : %d instead of 2 or 3' % (self.dimension))
            for courbe in self.listeCourbes:
                cmd = courbe.ajouterParametresGnuplot(cmd)
            if self.verbose>50:
                ficPlot.write('%s\n' % cmd)
                ficPlot.write('pause -1\n')
            if self.format=='ps':
                #postscript
                ficPlot.write('set terminal postscript eps color\n')
                self.fichierGraphique = 'fic%03d.ps' % (indice)
            elif self.format=='png':
                #png
                #ficPlot.write('set size 1.3,1.3\n')
                ficPlot.write('set terminal png large\n')
                self.fichierGraphique = 'fic%03d.png' % (indice)
            elif self.format=='epslatex':
                #latex
                ficPlot.write('set terminal epslatex color\n')
                self.fichierGraphique = 'fic%03d.tex' % (indice)
            elif self.format=='latex':
                #latex
                ficPlot.write('set terminal latex \n')
                self.fichierGraphique = 'fic%03d.tex' % (indice)
            elif self.format=='fig':
                ficPlot.write('set terminal fig color\n')
                self.fichierGraphique = 'fic%03d.fig' % (indice)
            else:
                print "No output format selected for gnuplot !"
                sys.exit(-1)
            self.fichierGraphiqueComplet = dest + '/' + self.fichierGraphique
            ficPlot.write('set output "%s"\n' % (self.fichierGraphiqueComplet))
            ficPlot.write('%s\n' % cmd)
            ficPlot.close()

            #lancement de gnuplot pour generation du fichier graphique
            #self.cmd = '/usr/bin/gnuplot %s' % (nomFichierPlotComplet)
            #import subprocess
            #subprocess.Popen(self.cmd, shell=True, bufsize=0, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
            if (debug_figure==None) or (debug_figure==-1) or (debug_figure==indice):
                pipe = os.system('gnuplot %s' % nomFichierPlotComplet)
                if self.format=='epslatex':
                    cmd='sed "s/\.\/\.tmp/\\\\\orig/.tmp/" %s > .pp; mv .pp %s '%(self.fichierGraphiqueComplet,self.fichierGraphiqueComplet)

                    os.system(cmd)
                    cmd='epstopdf %s'%(self.fichierGraphiqueComplet.replace('.tex','.eps'))
                    print cmd
                    os.system(cmd)
                #       raise "000"
                    pass

                pass
        else:
            self.fichierGraphiqueComplet = self.image
            self.fichierGraphique = '../' + self.image
            # dans le cas d'un fichier inclus on verifie son existence
            # warning juste, car on peut la generer plus tard

            if (not os.path.isfile(self.image)):
                print "No such file or directory:",self.image
                pass
        pass

    #---------------------------------------------
    # Methodes d'affichage des infos
    def afficherParametres(self):
        '''Affichage des parametres de la figure.'''
        self.gestMsg.ecrire(GestionMessages._INFO, '\tTitre=  %s' % self.titre)
        self.gestMsg.ecrire(GestionMessages._INFO, '\tDim=    %s' % self.dimension)
        self.gestMsg.ecrire(GestionMessages._INFO, '\tRange=  %s / %s' % (self.rangeX, self.rangeY))
        self.gestMsg.ecrire(GestionMessages._INFO, '\tLabels= %s / %s' % (self.labelX, self.labelY))
        for courbe in self.listeCourbes:
            courbe.afficherParametres()

            pass
        pass
    def modifie_pour_comparaison(self,old_path):
        '''modification pour ajouter courbes anciennes versions'''
        newc=[]
        for i in range(len(old_path)):
            old=old_path[i]
            ajout_str=str(i)
            if i==0: ajout_str=""
            for courbe in self.listeCourbes:
                if courbe.origine.lower().replace('"','')=='trio_u':
                    from copy import copy
                    courbe2=copy(courbe)
                    courbe2.version="old"+ajout_str
                    courbe2.legende+=" old"+ajout_str
                    if courbe2.fichier!='Undefined':
                        courbe2.fichier=old+courbe2.fichier
                        pass
                    if courbe2.segment!='Undefined':
                        courbe2.segment=old+courbe2.segment
                        pass
                    if courbe2.plan!='Undefined':
                        courbe2.plan=old+courbe2.plan
                        pass
                    newc.append(courbe2)
                    pass
                pass
            pass
        print "on a ajoute ",len(newc) ," courbes"
        for c in newc:
            self.listeCourbes.append(c)
            pass
        return []
