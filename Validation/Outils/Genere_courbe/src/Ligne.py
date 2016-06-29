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

import sys, os,  shutil
from lib import GestionMessages
from lib import getNomFonction
from lib import extraireMotcleValeur,print_description
from lib import chaine2Ascii
from lib import _accoladeF
from lib import _TypesReconnus
from lib import get_nom_cas

class Ligne:
	'''Classe decrivant une ligne telle qu elle devra etre
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
		self.alias       =  None
		#self.origine     = 'Undefined'
		self.origine     = 'trio_u'
		self.version     = 'Undefined'
		self.fichier     = 'Undefined'
		self.valeurs     = 'Undefined'
		self.difference  = 'Undefined'
		self.derniere_ligne = 0
		self.afficher_ligne = 1
		self.description = []
		self.colonnes    = None
		self.nb_colonnes_fichier = None
		pass
	def printFichierParametres(self):
		dec='\t\t'
		print dec,"Ligne {"
		dec='\t\t\t'
		print_description(self.description,dec)
		
		if self.legende != 'Undefined' : print dec,"legende",self.legende
		if self.alias != None : print dec,"alias",self.alias
		if self.origine != 'trio_u' : print dec,"origine",self.origine
		if self.version != 'Undefined' : print dec,"version",self.version
		if self.fichier != 'Undefined' : print dec,"fichier",self.fichier
		if self.valeurs != 'Undefined' : print dec,"valeurs", self.valeurs
		if self.difference != 'Undefined' : print dec,"difference",self.difference
		if self.derniere_ligne != 0 : print dec, "derniere_ligne" 
		if self.afficher_ligne != 1 : print dec,"afficher_ligne",self.afficher_ligne
		if self.colonnes != None : print dec,"colonnes",self.colonnes
		if self.nb_colonnes_fichier != None : print dec,"nb_colonnes_fichier",self.nb_colonnes_fichier
		dec='\t\t'
		print dec,"}"
		pass
	#lecture des parametres de la ligne dans le fichier de parametres
	def lireParametres(self, fichier):
		'''Lecture des parametres de la ligne.'''
		self.gestMsg.ecrire(GestionMessages._DEBOG, 'DEBUT %s.%s' % (self.__class__.__name__, getNomFonction()), niveau=15)
		fin = False
		dico=['legende','alias','origine','version','fichier','valeurs','difference','derniere_ligne','afficher_ligne','nb_colonnes_fichier','colonnes','description']
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
				elif motcle=='alias':
					self.alias = valeur
				elif motcle=='origine':
					self.origine = valeur
				elif motcle=='version':
					self.version = valeur
				elif motcle=='fichier':
					self.fichier = valeur
				elif motcle=='valeurs':
					
					#valeur_f=[ eval(x) for x in ligne.split()[1:] ]
					self.valeurs = valeur
				elif motcle=='difference':
					self.difference=valeur.strip()
				elif motcle=='derniere_ligne':
					self.derniere_ligne=1	
				elif motcle=='afficher_ligne':
					self.afficher_ligne = int(valeur)
				elif motcle=='nb_colonnes_fichier':
					self.nb_colonnes_fichier = int(valeur)
				elif motcle=='colonnes':
                                        valeur = valeur.strip()
					self.colonnes=valeur
				elif motcle=='description':
					self.description.append(valeur)
				else:
					self.gestMsg.ecrire_usage(GestionMessages._ERR, 'Line', dico,motcle_lu,fichier=fichier)
					pass
				if motcle!=_accoladeF and not (motcle in dico): print "Missing code for ",motcle;1/0
				pass
			pass
		if not(self.alias): self.alias=self.legende.replace(' ','_').replace('"','')
		pass
	
	def get_values(self,nb_colonnes,dico):
		val={}
		if self.valeurs!='Undefined':
			valeur_f= self.valeurs.split()
			for i in xrange(len(valeur_f)):
				try:
					valeur_f[i]=eval(valeur_f[i])
				except:
					pass
				
			for i in range(nb_colonnes):
				val[i]=valeur_f[i]
				pass
			nb_colonnes_f=nb_colonnes
			pass
		elif self.fichier!='Undefined':
			#print "lire_valeur dans",self.fichier
			if not os.path.isfile(self.fichier):
				print "File not found ",self.fichier
				pass 
			file=open(self.fichier,'r')
			nb_colonnes_f=nb_colonnes
       			if (self.nb_colonnes_fichier): nb_colonnes_f=self.nb_colonnes_fichier
			try:
				content_file=file.read()
				if self.derniere_ligne:
					from string import rfind
					last=rfind(content_file[:-1],'\n')
					#print "uu",content_file[last+1:]
					content_file=content_file[last+1:]
					if self.verbose>1:
						print "Last line of ",self.fichier,":\n %s#"%content_file
						pass
					pass
				content_file=content_file.split()
			
				for i in range(nb_colonnes_f):
					val[i]=(content_file[i])
					try:
						val[i]=eval(val[i])
					except:
						pass
					#print "iiiii",i,val[i]
					pass
				
			except:
				self.gestMsg.ecrire(GestionMessages._ERR, 'unable to read %d values in file %s.'% (nb_colonnes_f,self.fichier))
				pass
			pass
		
		elif self.difference!='Undefined':
			args=self.difference.split()
			methode=args[0]
			ligne1=dico[args[1]]
			ligne2=dico[args[2]]
			if methode.upper()=="delta".upper():
				for i in range(nb_colonnes):
					val[i]=ligne1[i]-ligne2[i]
			elif methode.upper()=="ecart_relatif".upper():
				for i in range(nb_colonnes):
					v1=ligne1[i]
					v2=ligne2[i]
					val[i]=(v1-v2)/(abs(v1)+abs(v2))*2.
					pass
			elif methode.upper()=="formule".upper() or methode.upper()=="FORMULA":
				formule=args[3]
				for i in range(nb_colonnes):
					v1=ligne1[i]
					v2=ligne2[i]
					val[i]=eval(formule)
					pass
			else:
				self.gestMsg.ecrire(GestionMessages._ERR, 'Unknown parameter. We were expecting a difference parameter, and not %s.' % (methode))
       		else:
			# les verif auraient du etre faite au dessus
			1/0
			pass
		if (self.colonnes):
			colonnes=self.colonnes.split()
			val2={}
			for i in range(nb_colonnes):
				formule=colonnes[i]
				if formule!="rien":
					for j0 in range(nb_colonnes_f):
						j=nb_colonnes_f-j0-1
						cmd="formule=formule.replace(\"$%d\",\"val[%d]\")"%(j+1,j)						
						exec(cmd)
						pass
					for key in dico.keys():
						cmd="formule=formule.replace(\"%s.val\",\"dico[\'%s\']\")"%(key,key)
						exec( cmd)
						pass
					# print formule
					val2[i]=eval(formule)
					pass
				else:
					val2[i]=""
					pass
				pass
			val=val2
			pass
		
		dico[self.alias]=val
		return val
	
	#---------------------------------------------
	# Methodes d'affichage des infos
	def afficherParametres(self):
		'''Affichage des parametres de la ligne.'''
		self.gestMsg.ecrire(GestionMessages._INFO, '\t\tLegende=  %s' % self.legende)
		self.gestMsg.ecrire(GestionMessages._INFO, '\t\tOrigine=  %s' % self.origine)
		self.gestMsg.ecrire(GestionMessages._INFO, '\t\tVersion=  %s' % self.version)
		pass
	pass

class Lignes(Ligne):
	'''Classe decrivant une ligne telle qu elle devra etre
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
		self.origine     = 'trio_u'
		self.fichier     = 'Undefined'
		self.numero_premiere_ligne = 1
		self.numero_derniere_ligne = -1
		self.afficher_ligne = 1
		self.nb_colonnes_fichier = None
		self.description = []
		self.colonnes    = None
		self.legende     = "" # pas utilise pour de vrai
		pass
	def printFichierParametres(self):
		dec='\t\t'
		print dec,"Lignes {"
		dec='\t\t\t'
		print_description(self.description,dec)
		
		if self.origine != 'trio_u' : print dec,"origine",self.origine
		if self.fichier != 'Undefined' : print dec,"fichier",self.fichier
		if self.numero_premiere_ligne != -1 : print dec, "numero_premiere_ligne",self.numero_premiere_ligne
		
		if self.numero_derniere_ligne != -1 : print dec, "numero_derniere_ligne",self.numero_derniere_ligne
		if self.afficher_ligne != 1 : print dec,"afficher_ligne",self.afficher_ligne
		if self.nb_colonnes_fichier != None : print dec,"nb_colonnes_fichier",self.nb_colonnes_fichier
		if self.colonnes != None : print dec,"colonnes",self.colonnes
		dec='\t\t'
		print dec,"}"
		pass
	#lecture des parametres de la ligne dans le fichier de parametres
	def lireParametres(self, fichier):
		'''Lecture des parametres de la ligne.'''
		self.gestMsg.ecrire(GestionMessages._DEBOG, 'DEBUT %s.%s' % (self.__class__.__name__, getNomFonction()), niveau=15)
		fin = False
		dico=['origine','version','fichier','numero_premiere_ligne','numero_derniere_ligne','afficher_ligne','nb_colonnes_fichier','colonnes','description']
		while not fin:
			ligne = fichier.readline()
			if not ligne:
				self.gestMsg.ecrire(GestionMessages._ERR, 'Unexpected end of file. We were expecting parameters of a figure.')
			ligne = ligne.strip()
			if len(ligne)>0 and ligne[0]!='#':
				motcle,valeur,motcle_lu = extraireMotcleValeur(fichier,ligne, self.gestMsg)
				if motcle==_accoladeF:
					fin = True
				elif motcle=='origine':
					self.origine = valeur
				elif motcle=='version':
					self.version = valeur
				elif motcle=='fichier':
					self.fichier = valeur
				elif motcle=='numero_premiere_ligne':
					self.numero_premiere_ligne=int(valeur)
				elif motcle=='numero_derniere_ligne':
					self.numero_derniere_ligne=int(valeur)	
				elif motcle=='afficher_ligne':
					self.afficher_ligne = int(valeur)
				elif motcle=='nb_colonnes_fichier':
					self.nb_colonnes_fichier = int(valeur)
				elif motcle=='colonnes':
					valeur = valeur.strip()
					self.colonnes=valeur	
				elif motcle=='description':
					self.description.append(valeur)
				else:
					self.gestMsg.ecrire_usage(GestionMessages._ERR, 'Lines', dico,motcle_lu,fichier=fichier)
					pass
				if motcle!=_accoladeF and not (motcle in dico): print "Missing code for ",motcle;1/0
				pass
			pass
		pass
	
	def get_values(self,nb_colonnes,dico):
		val={}
		if self.fichier!='Undefined':
			#print "lire_valeur dans",self.fichier
			if not os.path.isfile(self.fichier):
				print "File not found ",self.fichier
				pass 
			file=open(self.fichier,'r')
			nb_colonnes_f=nb_colonnes+1
       			if (self.nb_colonnes_fichier): nb_colonnes_f=self.nb_colonnes_fichier
			try:
				content_file=file.read()
				first=0
				nb_ligne=0;
				while(first>=0):
					first=content_file.find('\n',first+1)
					nb_ligne+=1
					pass
				if (self.numero_premiere_ligne<0):
					premiere=nb_ligne+self.numero_premiere_ligne
				else:
					premiere=self.numero_premiere_ligne
					pass
				if (self.numero_derniere_ligne<0):
					derniere=nb_ligne+self.numero_derniere_ligne
				else:
					derniere=self.numero_derniere_ligne
					pass
				nb_ligne-=1
				# print "nb_ligne",nb_ligne

				first=0
				for i in  range(premiere-1):
					first=content_file.find('\n')
					content_file=content_file[first+1:]
					pass
				
				nb_lignes=derniere-premiere+1
				print nb_lignes,"lignes  a lire "
				for ll in range(nb_lignes):
					split_content_file=content_file.split()
					for k in range(nb_colonnes_f):
						i=k+ll*nb_colonnes_f
						val[i]=(split_content_file[k])
						try:
							val[i]=eval(val[i])
						except:
							pass
						# print "iiiii",i,val[i]
						pass
					
					first=content_file.find('\n')
					content_file=content_file[first+1:]
					
					pass
				if (self.colonnes):
					val2={}
					for ll in range(nb_lignes):
						colonnes=self.colonnes.split()
						
						for i in range(nb_colonnes+1):
							formule=colonnes[i]
							
							for j0 in range(nb_colonnes_f):
								j=nb_colonnes_f-j0-1
								cmd="formule=formule.replace(\"$%d\",\"val[%d]\")"%(j+1,j+ll*nb_colonnes_f)						
								exec(cmd)
								pass
							# print formule
							val2[i+ll*(nb_colonnes+1)]=eval(formule)
							pass
						pass
					val=val2
					pass				
				pass
				
			except:
				self.gestMsg.ecrire(GestionMessages._ERR, 'unable to read %d values in file %s.'% (nb_colonnes_f,self.fichier))
				pass
			pass
		else:
			# les verif auraient du etre faite au dessus
			1/0
			pass
		return val
	
	#---------------------------------------------
	# Methodes d'affichage des infos
	def afficherParametres(self):
		'''Affichage des parametres de la ligne.'''
		self.gestMsg.ecrire(GestionMessages._INFO, '\t\tfichier=  %s' % self.fichier)
		self.gestMsg.ecrire(GestionMessages._INFO, '\t\tOrigine=  %s' % self.origine)
		
		pass
	pass

