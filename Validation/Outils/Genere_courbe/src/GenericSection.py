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

from Figure import Figure
from Visu import Visu
from lib import GestionMessages
from lib import getNomFonction
from lib import extraireMotcleValeur,print_description
from lib import chaine2Ascii
from lib import _accoladeF,verifie_accolade_suivante

class GenericSection(object):
    """ Classe servant de base aux differents types de section possible au sein de la fiche
    """
    def __init__(self, verbose=0, output='', namePart=""):
        '''Constructeur.'''
        if output=='':
            self.gestMsg = GestionMessages(verbose,'log')
        else:
            self.gestMsg = output
        self.verbose = verbose
        #initialisations
        self.description = []
        self.listeFigures = []
        self.namePart = namePart


    def printFichierParametres(self,indice):
        """ Lecture des parametres de la partie concernee dans le fichier de parametres """
        print("%s {" % self.namePart)
        dec='\t'

        print_description(self.description,dec)

        for fig in self.listeFigures :
            print(dec,"# definition figure %d"%(indice))
            fig.printFichierParametres()
            indice += 1
            pass
        print("}")
        return indice

    def lireParametres(self, fichier,casTest):
        '''Lecture des parametres du paragraphe. This should be overriden by subclasses. '''
        raise NotImplementedError

    def genererGraphes(self, dest, indice,debug_figure,novisit):
        '''Generation des graphiques correspondant a la partie.'''
        from Visu import Visu as vis
        self.gestMsg.ecrire(GestionMessages._DEBOG, 'DEBUT %s.%s' % (self.__class__.__name__, getNomFonction()), niveau=15)
        for figure in self.listeFigures:
            if not isinstance(figure, vis) or novisit==False:
                figure.genererGraphe(dest, indice,debug_figure)
                indice += 1
        return indice

    def afficherParametres(self):
        '''Affichage des parametres du paragraphe.'''
        self.gestMsg.ecrire(GestionMessages._INFO, '\tDesc =  %s' % self.description)
        for figure in self.listeFigures:
            figure.afficherParametres()

