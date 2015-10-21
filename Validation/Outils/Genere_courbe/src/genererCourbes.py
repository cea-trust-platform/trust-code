#!/usr/bin/python
#script pour generer un fichier pdf contenant les courbes permettant de valider des resultats de calculs pour TRUST.
#****************************************************************************
# Copyright (c) 2015, CEA
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

import sys, os, getopt, shutil

try: 
	sys.dont_write_bytecode = True
except:
        pass

from lib import GestionMessages
from lib import getNomFonction
from lib import extraireMotcleValeur,print_description
#from lib import chaine2Tex
from lib import _accoladeF,verifie_accolade_suivante


display_figure=True
from Chapitre import Chapitre




def Usage(gestMsg):
	'''Renvoie l usage du script.'''
	gestMsg.ecrire(GestionMessages._DEBOG, 'DEBUT %s' % getNomFonction(), niveau=15)
	usage = genererCourbes.__doc__
	return usage

def getOptions(argv, gestMsg):
	'''Lecture des options de ligne de commande.'''
	global display_figure
	gestMsg.ecrire(GestionMessages._DEBOG, 'DEBUT %s' % getNomFonction(), niveau=15)
	verbose = 0
	ficParametres = ''
	get_cmd_to_run=None
	old_path=[]
	debug_figure=None
	sortie = 'rapport.pdf'
	#recuperation des options
	try:
		options,arguments = getopt.getopt(argv, 'g:hfp:v:o:d:c:Dn', ['get_cmd_to_run=','help', 'format', 'parametres=', 'verbose=', 'output=','debug_figure=','compare=','DEBOG','nodisplay'])
	except getopt.GetoptError:
		gestMsg.ecrire(GestionMessages._ERR, '-> Error, unknown option. Options list is %s' % (argv), arret=True, usage=True, texteUsage=Usage(gestMsg))
		
	# balayage pour extraire les options utiles
	if (arguments):
		gestMsg.ecrire(GestionMessages._ERR, '-> Error, unknown option. Options list is %s' % (arguments), arret=True, usage=True)	
		
	
	for opt, arg in options:
		if opt in ('-h', '--help'):
			gestMsg.ecrire(GestionMessages._INFO, '-- Help --', arret=True, usage=True, texteUsage=Usage(gestMsg))
		elif opt in('-g','--get_cmd_to_run'):
			get_cmd_to_run=arg
		elif opt in ('-f', '--format'):
			gestMsg.ecrire(GestionMessages._INFO, '-- Help --', arret=True, usage=True, texteUsage=genererCourbes.__format_fichier__)
		elif opt in ('-v', '--verbose'):
			if arg.isdigit():
				verbose = int(arg)
			else:
				gestMsg.ecrire(GestionMessages._ERR, 'The verbosity argument should be an integer (and not \'%s\') !' % arg, texteUsage=Usage(gestMsg))
		elif opt in ('-p', '--parametres'):
			ficParametres = arg
		elif opt in ('-o', '--output'):
			sortie = arg
		elif opt in ('--compare'):
			if arg[-1]!='/': arg=arg+'/'
			old_path.append(arg)
		elif opt in ('-D','--DEBOG'):
			old_path.append('PAR_')
		elif opt in ('-d','--debug_figure'):
			if (arg=="text"): arg="-2"
			debug_figure=int(arg)
		elif opt in ('--nodisplay'):
			display_figure=False
		else:
			print "codage manquant pour "+str(opt)
			1/0
		
	if ficParametres=='':
		gestMsg.ecrire(GestionMessages._ERR, 'The parameter file has not been given !', texteUsage=Usage(gestMsg))

	return verbose, ficParametres, sortie, get_cmd_to_run,debug_figure,old_path


class genererCourbes:
	'''
NAME
	genererCourbes.py

SYNOPSIS
	python genererCourbes.py [OPTIONS]

DESCRIPTION
	Tool used to generate a validation report from TRUST simulation and reference curves.

	The options are:
		-p parameters.file
		--parameters=parameters.file
			to give le path of the parameters file. The format of this file is describer using the -f option.

	Other possible options are:
		-o
		--output
			to specify a output file path (default: rapport.pdf)
		-h
		--help
			to show this message
	
		-v N
		--verbose=N
			to set the verbosity level. The higher N is, the more talkative the script is.
			Default is 0.
	
		-f
		--format
			to show the parameters file description.
			
		--get_cmd_to_run=yes
		        to show command lines to execute to run cases
	        --get_cmd_to_run=post_run
		        to show command lines to execute to run post_run
			
	        --debug_figure=n
		        to debug the figure n (-1 all figures, text ou -2 only tex) 
			
	        --compare=old_path
		        to generate a rapport including the courbes of the old_path

	        --DEBOG
		        to generate a rapport including the courbes from parallel calculation
			
SEE ALSO
	NA.

BUGS
	No known bug today...

TODO
	Certainly some things.
'''
	#aide sur le format du fichier de parametres
	__format_fichier__ = 'ADU'

	#methode d'initialisation / constructeur
	def __init__(self, parametersFile, verbose=0, output='', out='rapport.pdf'):
		'''Constructeur.'''
		if output=='':
			self.gestMsg = GestionMessages(verbose,'log')
		else:
			self.gestMsg = output

		self.gestMsg.parametersFile=parametersFile
		logFile = parametersFile + '.log'
		self.gestMsg.setOutputFile(logFile)
		self.verbose = verbose
		self.parametersFile = parametersFile
		#initalisations
		self.titre = 'Undefined'
		self.auteur = 'Undefined'
		self.description = []
		self.reference = []
		self.casTest = []
		self.versionTrioU = ''
		self.parametresTrioU = []
		#import time
		# self.date = time.strftime('%d/%m/%Y')
		self.listeChapitres = []
		self.inclureData = 2
		self.compile_latex=1
		self.sortie = out
		self.preRequis = []


		pass
	def printFichierParametres(self):
		dec='\t'
		print "parametres {"
		if self.titre != 'Undefined' : print dec,"titre" ,self.titre  
		if self.auteur != 'Undefined': print dec,"auteur" , self.auteur
		print_description(self.description,dec)
		
		for ref in self.reference :print dec,"reference",ref
		for cas in self.casTest: print dec,"casTest", cas
		if self.versionTrioU != '': print dec,"versionTrio_u", self.versionTrioU
		for param in  self.parametresTrioU : print dec,"parametresTrio_u",param
	
		if self.inclureData != 2: print dec,'inclureData',self.inclureData
		for pre in self.preRequis : print dec,'preRequis', pre
		print "}"
		indice=0
		for chap in self.listeChapitres:
			indice=chap.printFichierParametres(indice)
			pass
		pass
	
		
	#---------------------------------------------
	# Methodes de lecture du fichier de parametres

	#methode de lecture du fichier de parametres
	def lireFichierParametres(self):
		'''Lecture du fichier de parametres.'''
		self.gestMsg.ecrire(GestionMessages._DEBOG, 'DEBUT %s.%s' % (self.__class__.__name__, getNomFonction()), niveau=15)
		if not os.path.isfile(self.parametersFile):
			self.gestMsg.ecrire(GestionMessages._ERR, 'The parameter file does not exist : %s' % self.parametersFile, texteUsage=Usage(gestMsg))
		fichier = open(self.parametersFile, 'r')

		#On commence par lire les parametres generaux du rapport
		self.lireParametresGeneraux(fichier)

		#Lecture des Chapitres
		fin = False
		while not fin:
			ligne = fichier.readline()
			if not ligne:
				fin = True
			ligne = ligne.strip()
			if len(ligne)>0 and ligne[0]!='#':
				motcle,valeur,motcle_lu = extraireMotcleValeur(fichier,ligne, self.gestMsg)
				if motcle=='chapitre':
					verifie_accolade_suivante(ligne,fichier,self.gestMsg)
					fig = self.lireParametresChapitre(fichier)
					if fig!=None:
						self.listeChapitres.append(fig)
				else:
					self.gestMsg.ecrire(GestionMessages._ERR, 'We were expecting a keyword like Chapitre or Chapter, and not %s' % ( motcle_lu),fichier=fichier)

		fichier.close()

	#lecture des parametres generaux
	def lireParametresGeneraux(self, fichier):
		'''Lecture des parametres generaux.'''
		self.gestMsg.ecrire(GestionMessages._DEBOG, 'DEBUT %s.%s' % (self.__class__.__name__, getNomFonction()), niveau=15)
		ok=0
		while (ok==0):
			ligne = fichier.readline()
			if not ligne:
				self.gestMsg.ecrire(GestionMessages._ERR, 'Unexpected end of file. We were expecting Parametres {...}".')
			ligne = ligne.strip()
			if ligne[0]!='#': ok=1
			pass
		
		motcle,valeur,motcle_lu = extraireMotcleValeur(fichier,ligne, self.gestMsg)
		if motcle=='parametres':
			#Lecture des parametres generaux
			verifie_accolade_suivante(ligne,fichier,self.gestMsg)
			fin = False
			dico=['titre','auteur','description','reference','castest','versiontrio_u','parametrestrio_u','incluredata','prerequis']
			while not fin:
				ligne = fichier.readline()
				if not ligne:
					self.gestMsg.ecrire(GestionMessages._ERR, 'Unexpected end of file. We expected a parameter from the general parameters.')
			
				ligne = ligne.strip()
				if len(ligne)>0 and ligne[0]!='#':
					motcle,valeur,motcle_lu = extraireMotcleValeur(fichier,ligne, self.gestMsg)
					
					if motcle==_accoladeF:
						fin = True
					elif motcle=='titre':
						self.titre = valeur
					elif motcle=='auteur':
						self.auteur = valeur
					elif motcle=='description':
						self.description.append(valeur)
					elif motcle=='reference':
						self.reference.append(valeur)
					elif motcle=='castest':
						self.casTest.append(valeur)
					elif motcle=='versiontrio_u':
						self.versionTrioU = valeur
					elif motcle=='parametrestrio_u':
						self.parametresTrioU.append(valeur)
					elif motcle=='incluredata':
						self.inclureData = int(valeur)
					elif motcle=='prerequis':
						self.preRequis.append(valeur.replace('"',''))
					else:
						self.gestMsg.ecrire_usage(GestionMessages._ERR, 'Parameters', dico,motcle_lu,fichier=fichier)
					if motcle!=_accoladeF and not (motcle in dico): print "Missing code for ",motcle;1/0

		else:
			self.gestMsg.ecrire(GestionMessages._ERR, 'We were expecting "Parametres {...} or Parameters {...} ", and not %s' % motcle_lu,fichier=fichier)

	#lecture des parametres d'un chapitre
	def lireParametresChapitre(self, fichier):
		'''Lecture des parametres d un chapitre.'''
		self.gestMsg.ecrire(GestionMessages._DEBOG, 'DEBUT %s.%s' % (self.__class__.__name__, getNomFonction()), niveau=15)
		#creation du chapitre
		chapitre = Chapitre(verbose=self.verbose, output=self.gestMsg)
		#puis lecture du chapitre
		chapitre.lireParametres(fichier,self.casTest)

		return chapitre

	#---------------------------------------------
	# Methodes d'affichage des infos
	def afficherParametres(self):
		'''Affichage des parametres du document.'''
		self.gestMsg.ecrire(GestionMessages._INFO, 'AFFICHAGE parametres')
		self.gestMsg.ecrire(GestionMessages._INFO, 'Titre=  %s' % self.titre)
		self.gestMsg.ecrire(GestionMessages._INFO, 'Auteur= %s' % self.auteur)
		for descr in self.description:
			self.gestMsg.ecrire(GestionMessages._INFO, 'Ref=    %s' % descr)
			pass
		self.gestMsg.ecrire(GestionMessages._INFO, 'Ref=    %s' % self.reference)
		for chapitre in self.listeChapitres:
			chapitre.afficherParametres()


	#---------------------------------------------
	# Methodes de generation du rapport

	#methode de generation du fichier de validation
	def genererRapport(self,debug_figure):
		'''Generation du rapport de validation.'''
		self.gestMsg.ecrire(GestionMessages._DEBOG, 'DEBUT %s.%s' % (self.__class__.__name__, getNomFonction()), niveau=15)

		#lancement des prerequis
		if self.preRequis!=[]:
			#print os.getcwd()
			for pre in self.preRequis:
				res = os.system('%s' % pre)
				self.gestMsg.ecrire(GestionMessages._INFO, 'Running %s\n%s' % (pre, res))
		#creation d'un repertoire temporaire
		destTMP = './.tmp'
		if (debug_figure==None):
			if os.path.isdir(destTMP):
				shutil.rmtree(destTMP,True)
			os.mkdir(destTMP)
		else:
			if not(os.path.isdir(destTMP)):
				os.mkdir(destTMP)
				pass
			pass
		

		#balayage des chapitres, pour les ajouter au rapport
		indice = 0
		for chapitre in self.listeChapitres:
			indice = chapitre.genererGraphes(destTMP, indice,debug_figure)
			#indice += 1
			pass
		if (self.compile_latex==1):
			nomFichierTex = 'fic.tex'
			nomFichierTexComplet = destTMP + '/' + nomFichierTex
			try:
				import sys
				sys.path.append(".")
				from User_Write_tex import User_Write_tex
				w=User_Write_tex()
				print "User_write_tex uses to generate tex"
			except:
				from Write_tex import Write_tex
				w=Write_tex()
				pass
			dico={}
			dico["nomFichierTex"]=nomFichierTex
			dico["nomFichierTexComplet"]=nomFichierTexComplet
			dico["destTMP"]=destTMP
			w.write_fichiers(self,dico)

			# generation du fichier pdf
			# import subprocess
			# res = subprocess.Popen('/usr/bin/pdflatex %s' % nomFichierTex, shell=True, cwd=destTMP, bufsize=0, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
			# err = res.stderr.readlines()
			# if err!=[]:
			#	self.gestMsg.ecrire(GestionMessages._ERR, 'Error when generating pdf file, see log file (%s)' % (nomFichierTexComplet[:-3] + 'log'))
	
			ici = os.getcwd()
			os.chdir(destTMP)
			res = os.system('pdflatex -interaction=nonstopmode %s >pdflatex.log' % nomFichierTex)
			nomFicPdf_court = nomFichierTex[:-3] + 'pdf'
			res = os.system('pdflatex -interaction=nonstopmode %s >pdflatex.log; [ $? != 0 ] && cat pdflatex.log&& echo &&  rm -f %s' %(nomFichierTex , nomFicPdf_court))
			os.chdir(ici)
			nomFicPdf = nomFichierTexComplet[:-3] + 'pdf'
			if os.path.exists(nomFicPdf):
				print '%s -> %s' % (nomFicPdf, self.sortie)
				os.rename(nomFicPdf, self.sortie)
				print '-> The PDF report %s is available in the build directory.' % (self.sortie)
				pass
			else:
				print "-> PDF generation failed. The PDF report is deleted. ",self.sortie
				if os.path.isfile(self.sortie): os.remove(self.sortie)
				raise Exception("Generation failed!")
			pass
		if (debug_figure!=None)and (debug_figure>=0):
			if indice<debug_figure:
				raise Exception("figure %d not generated nmax=%d"%(debug_figure,indice))
			#print "ici", indice , debug_figure
			if  (display_figure):
				cmd='ls -rt %s/*.png | tail -1 | xargs display &'%destTMP
				from os import system
				system(cmd)
				pass
			pass
		pass
	
	pass

        def modifie_figures(app,old_path):
	    ''' double les figures trio pour ajouter les anciens resultats situes dans old/lesnouveaux ... souvent old commence par ../ et finit par / '''
	  
	    for chap in app.listeChapitres:
		    newfig=[]
		    for figure in chap.listeFigures:
			    new_figs=figure.modifie_pour_comparaison(old_path)
			    newfig.append(figure)
			    for fig in new_figs: newfig.append(fig) 
			    pass
		    chap.listeFigures=newfig
		    pass
	    pass
    



if __name__ == "__main__":
	# global debug_figures
	argv = sys.argv[1:]
	gestMsg = GestionMessages(verbose=10, output='log', ecran=True)
	verbose, ficParam, sortie, get_cmd_to_run,debug_figure,old_path= getOptions(argv, gestMsg)
	gestMsg.setNiveauMessage(verbose)
	#creation de l'objet de generation des courbes
	app = genererCourbes(parametersFile=ficParam, verbose=verbose, output=gestMsg, out=sortie)

	#lecture du fichier de parametres
	app.lireFichierParametres()

	if verbose>9:
		app.afficherParametres()
		# raise 'te'
	if get_cmd_to_run:
		list_cas=[]
		for cas0 in app.casTest:
			from lib import get_detail_cas
			case,dir,nb_proc,comment=get_detail_cas(cas0)
			list_cas.append(dir+"/"+case+".perf")
			data=case+'.data'
			cmd_0='( echo;echo "-> Running the calculation of the %s data file in the %s directory ...";cd %s ;'%(case,dir,dir)
			test_cmd='extract_perf %s 2>/dev/null; if [ $? -ne 0 ]; then '%(case)
			if (nb_proc==1):
				
				cmd=' [ -f pre_run ] && chmod +x pre_run && echo "-> Running the pre_run script in the %s directory ..." && ./pre_run %s; triou %s     1>%s.out 2>%s.err ;'%(dir,case,case,case,case)
			else:
				cmd=' [ -f pre_run ] && chmod +x pre_run && echo "-> Running the pre_run script in the %s directory ..." && ./pre_run %s; triou %s %d  1>%s.out 2>%s.err ;'%(dir,case,case,nb_proc,case,case)
				pass
			cmd_2='extract_perf %s; [ -f post_run ] && chmod +x post_run && echo "-> Running the post_run script in the %s directory ..." && ./post_run %s)'%(case,dir,case)
			if (get_cmd_to_run=='yes'):
				print cmd_0,cmd,cmd_2
			elif (get_cmd_to_run=='post_run'):
				print cmd_0,cmd_2
			elif (get_cmd_to_run=='test_run'):
				print cmd_0,test_cmd," echo KO ; else echo not run;echo;fi)"
				#print cmd_0,test_cmd,cmd,cmd_2.replace(')','; else echo not run;echo;fi)')
			elif (get_cmd_to_run=='suite_run'):
				print cmd_0,test_cmd,cmd,cmd_2.replace(')','; else echo not run;echo;fi)')
			else:
				print '# ',get_cmd_to_run , cmd_0,cmd,cmd_2
				pass
			pass
		cmd="get_total_time "+" ".join(list_cas)
		if (get_cmd_to_run!='not_run'):
			print cmd
		else:
			print '# ',cmd
			pass
		pass
	if len(old_path):
		app.modifie_figures(old_path)
		pass
	if verbose>0:
		print "-> Generation of test_lu.prm file."
		import cStringIO
		s = cStringIO.StringIO()
		import sys
		sys.stdout = s
		app.printFichierParametres()
		sys.stdout = sys.__stdout__
		f=open("test_lu.prm","w")
		f.write(s.getvalue())
		f.close()
		pass
	
	if get_cmd_to_run:
		from sys import exit
		if (get_cmd_to_run!='not_run'):
		     exit(0)
		     pass
	        pass

	# generation du rapport de validation
	if (debug_figure!=None)and(debug_figure!=-2):
		app.compile_latex=0
	app.genererRapport(debug_figure)

