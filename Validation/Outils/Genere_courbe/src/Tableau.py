#classe decrivant une tableau
# -*- coding: latin-1 -*-
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

from Ligne import Ligne,Lignes

from lib import GestionMessages
from lib import getNomFonction
from lib import extraireMotcleValeur,print_description
from lib import chaine2Ascii
from lib import _accoladeF,verifie_accolade_suivante

class Tableau:
	'''Classe decrivant une tableau telle qu elle devra etre tracee dans le rapport de validation de TRUST.'''

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
		self.dimension = -1
		self.label    = "Undefined"
		self.nb_colonnes= -1
		self.textsize= None
		self.listeLignes = []
		self.legende = 'Undefined'
		self.formule=  None
		self.description = []
		self.inclureDescLignes = 0
		self.transposed_display = 0
		pass
	def printFichierParametres(self):
		dec='\t'
		print dec,"Tableau {"
		dec='\t\t'
		if self.titre != 'Undefined' : print dec,"titre" ,self.titre
		print_description(self.description,dec)
		
		if self.nb_colonnes != -1: print dec,"nb_colonnes",self.nb_colonnes
		if self.textsize: print dec,"textsize",self.textsize
		if self.formule !=  None: print dec,"formule",self.formule
		
		if self.label != 'Undefined' : print dec,"label ",self.label	
		if self.legende != 'Undefined' : print dec,"legende",self.legende
	
		if self.inclureDescLignes != 0: print dec,"inclureDescLignes", self.inclureDescLignes
		if self.transposed_display != 0: print dec,"transposed_display", self.transposed_display
		for ligne in self.listeLignes :  ligne.printFichierParametres()
		dec='\t'
		print dec,"}"
		pass
	#lecture des parametres du tableau dans le fichier de parametres
	def lireParametres(self, fichier):
		'''Lecture des parametres du tableau.'''
		self.gestMsg.ecrire(GestionMessages._DEBOG, 'DEBUT %s.%s' % (self.__class__.__name__, getNomFonction()), niveau=15)
		fin = False
		dico=['titre','nb_colonnes','formule','label','legende','description','ligne','lignes','incluredesclignes','transposed_display','textsize']
		sizedico = {'tiny':r'\tiny', 
					'script':r'\scriptsize',
					'footnote':r'\footnotesize',
					'small':r'\small',
					'large':r'\large',
					'Large':r'\Large'}
		while not fin:
			ligne = fichier.readline()
			if not ligne:
				self.gestMsg.ecrire(GestionMessages._ERR, 'Unexpected end of file. We expected parameters of a tableau (array).')
			ligne = ligne.strip()
			if len(ligne)>0 and ligne[0]!='#':
				motcle,valeur,motcle_lu = extraireMotcleValeur(fichier,ligne, self.gestMsg)
				if motcle==_accoladeF:
					fin = True
				elif motcle=='titre':
					self.titre = valeur
				elif motcle=='nb_colonnes':
					self.nb_colonnes = int(valeur)
				elif motcle=='textsize':
					if valeur in sizedico.keys():
						self.textsize = sizedico[valeur]
				elif motcle=='formule':
					self.formule=valeur
				elif motcle=='label':
					valeurT = valeur.split('|')
					nbc=self.nb_colonnes
					if (self.formule): nbc+=1
					if len(valeurT)!=nbc:
						self.gestMsg.ecrire(GestionMessages._ERR, 'Error, the label %s is not compatible with nb_colonnes %d' % ( valeur,self.nb_colonnes),fichier=fichier)
					
					self.label = valeur
					#[ x.strip() for x in valeurT ]
					#print self.label
				elif motcle=='legende':
					self.legende = valeur
				elif motcle=='description':
					self.description.append(valeur)
				elif motcle=='ligne':
					verifie_accolade_suivante(ligne,fichier,self.gestMsg)
					ligne = Ligne(verbose=self.verbose, output=self.gestMsg)
					ligne.lireParametres(fichier)
					self.listeLignes.append(ligne)
				elif motcle=='lignes':
					verifie_accolade_suivante(ligne,fichier,self.gestMsg)
					ligne = Lignes(verbose=self.verbose, output=self.gestMsg)
					ligne.lireParametres(fichier)
					self.listeLignes.append(ligne)
				elif motcle=='incluredesclignes':
					self.inclureDescLignes = int(valeur)
				elif motcle=='transposed_display':
					self.transposed_display = int(valeur)
				else:
					self.gestMsg.ecrire_usage(GestionMessages._ERR, 'Table', dico,motcle_lu,fichier=fichier)
				if motcle!=_accoladeF and not (motcle in dico): print "Missing code for ",motcle;1/0

	
	#generation du graphique correspondant au tableau
	def genererGraphe(self, dest, indice,debug_figure):
		'''Generation du graphique correspondant au tableau. ne fait rien'''
		# print "iiii", self.titre,debug_figure,indice,self.listeLignes
		# on fait charger les valeurs pour trouver les fichiers manquants
		if (debug_figure==indice):
			self.get_tableau()
		return
		
	def get_tableau(self):
		'''remplissage du tableau de valeurs (list de dico....)'''
		
		nbc=self.nb_colonnes
		if (self.formule): nbc+=1
		dico={}
		first=None
		tableau=[]

		err=0
		for ligne in self.listeLignes:
			try:
				values=ligne.get_values(self.nb_colonnes,dico)
				# on charge toujours les valeurs mais on ne les affiches pas forcement
			
			
				if self.formule:
					if not(first):
						first=values
						values[self.nb_colonnes]=""
					else:
						ref=first[0]
						val=values[0]
						expr=eval(self.formule)
						# print "uuuuu",expr
						values[self.nb_colonnes]=(expr)
						pass
					pass
				
				tableau.append(values)
			except:
				err+=1
				pass
			
			pass
		if err:
			print "Error when reading tableau (array) file" , err
			values=[]
			for i in range(nbc): values.append("erreur-compil_  voir plus haut erreur lecture fichier tableau {")
			tableau.append(values)
		return tableau
						
	
	#---------------------------------------------
	# Methodes d'affichage des infos
	def afficherParametres(self):
		'''Affichage des parametres du tableau.'''
		self.gestMsg.ecrire(GestionMessages._INFO, '\tTitre=  %s' % self.titre)
		self.gestMsg.ecrire(GestionMessages._INFO, '\tDim=    %s' % self.dimension)
		self.gestMsg.ecrire(GestionMessages._INFO, '\tLabels=   %s' % (self.label))
		for ligne in self.listeLignes:
			ligne.afficherParametres()
			pass
		pass
		pass
	def modifie_pour_comparaison(self,old_path):
		'''modification pour ajouter ligness anciennes versions'''
		newt=[]
		newc=[]
		for i in range(len(old_path)):
			old=old_path[i]
			ajout_str=str(i)
			if i==0: ajout_str=""
			for ligne in self.listeLignes:
				if ligne.origine.lower().replace('"','')=='trio_u':
					from copy import copy
					ligne2=copy(ligne)
					ligne2.version="old"+ajout_str
					ligne2.legende+=" old"+ajout_str
					if ligne2.fichier!='Undefined':
						ligne2.fichier=old+ligne2.fichier
						pass
					newt.append(ligne2)
					if ligne2.fichier!='Undefined':
						fichier_delta=ligne.fichier+'_delta'+ajout_str
						
						if os.path.isfile(fichier_delta):
							ligne2=copy(ligne)
							ligne2.version="delta"+ajout_str
							ligne2.legende+=" delta"+ajout_str
							ligne2.fichier=fichier_delta
							newc.append(ligne2)
							pass
					
							
						pass
					pass
				pass
			pass
		print "on a ajoute ",len(newt) ," lignes +", len(newc), " delta"
		for c in newt:
			self.listeLignes.append(c)
			pass
		for c in newc:
			self.listeLignes.append(c)
			pass
		return []
			
	pass
class Tableau_performance(Tableau):
	'''Classe decrivant une tableau telle qu elle devra etre tracee dans le rapport de validation de TRUST.'''

	#constructeur
	def __init__(self, verbose=0, output=''):
		Tableau.__init__(self,verbose,output)
		self.label    = 'host|system|Total CPU Time|CPU time/step|number of cell'
		self.nb_colonnes= 5
		pass
	def  lireParametres(self, fichier,casTest):
		Tableau.lireParametres(self,fichier)
		for cas in casTest:
			from lib import get_detail_cas
			case,dir,nb_proc,comment=get_detail_cas(cas)
			ligne = Ligne(verbose=self.verbose, output=self.gestMsg)
			CC=dir+"/"+case
			ligne.legende=case
			ligne.legende=CC
			ligne.fichier=CC+".perf"
			ligne.origine='TRUST'
			ligne.nb_colonnes_fichier=6
			ligne.colonnes="$2 $3 $4 $5 $6"
			self.listeLignes.append(ligne)
			
			pass
		ligne = Ligne(verbose=self.verbose, output=self.gestMsg)
		ligne.legende="Total"
		ligne.fichier="temps_total"
		ligne.origine='TRUST'
		ligne.nb_colonnes_fichier=1
		ligne.colonnes="rien rien $1 rien rien"
		if (len(casTest)>1):
			self.listeLignes.append(ligne)
			pass
		return
	
	pass
