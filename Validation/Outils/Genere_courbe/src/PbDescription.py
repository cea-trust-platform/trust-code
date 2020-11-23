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

#classe decrivant le paragraphe Problem Description

import sys, os

from Figure import Figure
from Visu import Visu
from SousChapitre import SousChapitre
from lib import GestionMessages
from lib import getNomFonction
from lib import extraireMotcleValeur,print_description
from lib import chaine2Ascii
from lib import _accoladeF,verifie_accolade_suivante

class PbDescription:
    '''Classe decrivant la partie Problem Description telle qu elle devra apparaitre dans le rapport de validation de TRUST.'''

    #constructeur
    def __init__(self, verbose=0, output=''):
        '''Constructeur.'''
        if output=='':
            self.gestMsg = GestionMessages(verbose,'log')
        else:
            self.gestMsg = output
        self.verbose = verbose
        #initialisations
        self.description = []
        self.listeFigures = []
        self.listeSSChap = []

    #lecture des parametres de la partie Problem Description dans le fichier de parametres
    def printFichierParametres(self,indice):
        print("Problem Description {")
        dec='\t'

        print_description(self.description,dec)

        for fig in self.listeFigures :
            print(dec,"# definition figure %d"%(indice))
            fig.printFichierParametres()
            indice += 1
            pass
        print("}")
        return indice
#
    def lireParametres(self, fichier,casTest):
        '''Lecture des parametres du paragraphe Problem Description.'''
        self.gestMsg.ecrire(GestionMessages._DEBOG, 'DEBUT %s.%s' % (self.__class__.__name__, getNomFonction()), niveau=15)
        fin = False
        dico=['description','figure','visu','geometrie','cicl','propfluide','phyecou']
        while not fin:
            ligne = fichier.readline()
            if not ligne:
                self.gestMsg.ecrire(GestionMessages._ERR, 'Unexpected end of file. We expected parameters of the paragraph Problem Description.')
            ligne = ligne.strip()
            if len(ligne)>0 and ligne[0]!='#':
                motcle,valeur,motcle_lu = extraireMotcleValeur(fichier,ligne, self.gestMsg)
                if motcle==_accoladeF:
                    fin = True
                elif motcle=='description':
                    self.description.append(valeur)
                elif motcle=='figure':
                    verifie_accolade_suivante(ligne,fichier,self.gestMsg)
                    figure = Figure(verbose=self.verbose, output=self.gestMsg)
                    figure.lireParametres(fichier)
                    self.listeFigures.append(figure)
                    print(self.listeFigures)
                elif motcle=='geometrie':
                    verifie_accolade_suivante(ligne,fichier,self.gestMsg)
                    sousChapitre = SousChapitre(verbose=self.verbose, output=self.gestMsg)
                    sousChapitre.titre = 'Geometry'
                    sousChapitre.lireParametres(fichier)
                    self.listeSSChap.append(sousChapitre)
                elif motcle=='cicl':
                    verifie_accolade_suivante(ligne,fichier,self.gestMsg)
                    sousChapitre = SousChapitre(verbose=self.verbose, output=self.gestMsg)
                    sousChapitre.titre = 'Initial Conditions and Boundary Conditions'
                    sousChapitre.lireParametres(fichier)
                    self.listeSSChap.append(sousChapitre)
                elif motcle=='propfluide':
                    verifie_accolade_suivante(ligne,fichier,self.gestMsg)
                    sousChapitre = SousChapitre(verbose=self.verbose, output=self.gestMsg)
                    sousChapitre.titre = 'Fluid Properties'
                    sousChapitre.lireParametres(fichier)
                    self.listeSSChap.append(sousChapitre)
                elif motcle=='phyecou':
                    verifie_accolade_suivante(ligne,fichier,self.gestMsg)
                    sousChapitre = SousChapitre(verbose=self.verbose, output=self.gestMsg)
                    sousChapitre.titre = 'Flow Physics'
                    sousChapitre.lireParametres(fichier)
                    self.listeSSChap.append(sousChapitre)
                elif motcle=='visu':
                    verifie_accolade_suivante(ligne,fichier,self.gestMsg)
                    figure = Visu(verbose=self.verbose, output=self.gestMsg)
                    figure.lireParametres(fichier)
                    self.listeFigures.append(figure)

                    pass
                else:
                    self.gestMsg.ecrire_usage(GestionMessages._ERR,'Problem Description', dico,motcle_lu,fichier=fichier)
                if motcle!=_accoladeF and not (motcle in dico): print("Missing code for ",motcle);1/0

    #generation des graphiques correspondant aux figures de la partie Problem Description
    def genererGraphes(self, dest, indice,debug_figure,novisit):
        '''Generation des graphiques correspondant a la partie Problem Description.'''
        from Visu import Visu as vis
        self.gestMsg.ecrire(GestionMessages._DEBOG, 'DEBUT %s.%s' % (self.__class__.__name__, getNomFonction()), niveau=15)
        for figure in self.listeFigures:
            if not isinstance(figure, vis) or novisit==False:
                figure.genererGraphe(dest, indice,debug_figure)
                indice += 1
        return indice


    # Methodes d'affichage des infos
    def afficherParametres(self):
        '''Affichage des parametres du paragraphe Problem Description.'''
        self.gestMsg.ecrire(GestionMessages._INFO, '\tDesc =  %s' % self.description)
        for figure in self.listeFigures:
            figure.afficherParametres()
            pass
        pass
    pass
