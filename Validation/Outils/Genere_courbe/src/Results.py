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
from Tableau import Tableau
from GenericSection import GenericSection
from lib import GestionMessages
from lib import getNomFonction
from lib import extraireMotcleValeur
from lib import _accoladeF,verifie_accolade_suivante

class Results(GenericSection):
    '''Classe decrivant la partie Results telle qu elle devra apparaitre dans le rapport de validation de TRUST.'''

    def __init__(self, verbose=0, output=''):
        '''Constructeur.'''
        super().__init__(verbose=verbose, output=output, namePart="Results")
        self.dicoresult = {}
        self.listeSSChap = []

    def lireParametres(self, fichier,casTest):
        '''Lecture des parametres du paragraphe Results.'''
        self.gestMsg.ecrire(GestionMessages._DEBOG, 'DEBUT %s.%s' % (self.__class__.__name__, getNomFonction()), niveau=15)
        fin = False
        dico=['description','figure','visu','tableau']
        i=0
        while not fin:
            ligne = fichier.readline()
            if not ligne:
                self.gestMsg.ecrire(GestionMessages._ERR, 'Unexpected end of file. We expected parameters of the paragraph Results.')
            ligne = ligne.strip()
            if len(ligne)>0 and ligne[0]!='#':
                motcle,valeur,motcle_lu = extraireMotcleValeur(fichier,ligne, self.gestMsg)
                if motcle==_accoladeF:
                    fin = True
                elif motcle=='description':
                    self.description.append(valeur)
                    self.dicoresult[i]={}
                    self.dicoresult[i]['motcle']='description'
                    self.dicoresult[i]['valeur']=valeur
                    i=i+1
                elif motcle=='figure':
                    verifie_accolade_suivante(ligne,fichier,self.gestMsg)
                    figure = Figure(verbose=self.verbose, output=self.gestMsg)
                    figure.lireParametres(fichier)
                    self.listeFigures.append(figure)
                    self.dicoresult[i]={}
                    self.dicoresult[i]['motcle']='figure'
                    self.dicoresult[i]['valeur']=figure
                    i=i+1
                elif motcle=='visu':
                    verifie_accolade_suivante(ligne,fichier,self.gestMsg)
                    figure = Visu(verbose=self.verbose, output=self.gestMsg)
                    figure.lireParametres(fichier)
                    self.listeFigures.append(figure)
                    self.dicoresult[i]={}
                    self.dicoresult[i]['motcle']='figure'
                    self.dicoresult[i]['valeur']=figure
                    i=i+1
                elif motcle=='tableau':
                    verifie_accolade_suivante(ligne,fichier,self.gestMsg)
                    tableau = Tableau(verbose=self.verbose, output=self.gestMsg)
                    tableau.lireParametres(fichier)
                    self.listeFigures.append(tableau)
                    self.dicoresult[i]={}
                    self.dicoresult[i]['motcle']='figure'
                    self.dicoresult[i]['valeur']=tableau
                    i=i+1
                else:
                    self.gestMsg.ecrire_usage(GestionMessages._ERR,'Results', dico,motcle_lu,fichier=fichier)
                if motcle!=_accoladeF and not (motcle in dico): print("Missing code for ",motcle);1/0

