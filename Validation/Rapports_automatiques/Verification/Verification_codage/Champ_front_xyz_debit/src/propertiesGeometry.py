#Script de recuperation des proprietes physiques et de la geometrie

import os, sys, math
import numpy as np

def readPropertiesData(nomFic):
	#initialisation
	properties = {}
	properties['debit'] = -1
	# ouverture des fichiers
	fic = open(nomFic,'r')

	for ligne in fic:
		ligne = ligne.strip().lower()
		tLigne = ligne.split()
		if ligne.find('flow_rate')>-1:
#			if ligne.startswith('mu'):
				properties['debit'] = float(tLigne[-1])
#			elif ligne.startswith('rho'):
#				properties['rho'] = float(tLigne[-1])
#		elif ligne.find('vmax')>-1:
#			properties['Vmax'] = float(tLigne[-2])
#		elif ligne.find('longueur')>-1:
#			properties['L'] = float(tLigne[-2])
#		elif ligne.find('rayon')>-1:
#			properties['R'] = float(tLigne[-2])
	fic.close()
	return properties

def debit(nomFic):
	fic = open(nomFic,'r')
	# lecture de ligne -> entetes
	fichier = fic.readlines()
	#tant que la derniere ligne est vide
	while fichier[-1]=="" or fichier[-1]=="\n":
		del fichier [-1]
	ligne = fichier[-1]
	tLigne = ligne.split()
	debit=float(tLigne[-1])
	fic.close()
	return debit

def debit_trust(nomFic):
	fic = open(nomFic,'r')
	# lecture de ligne -> entetes
	fichier = fic.readlines()
	#tant que la derniere ligne est vide
	while fichier[-1]=="" or fichier[-1]=="\n":
		del fichier [-1]
	ligne = fichier[-1]
	tLigne = ligne.split()
	debit=float(tLigne[4])
	fic.close()
	return debit


def ecritureFichier(properties, values):
	nomFic = 'propertiesGeometry.dat'
	fichier = open(nomFic, 'w')
	fichier.write('%18.3e %18.3f %18.3f \n' % ( properties['debit'], values['debit'], values['debit_trust']))
	fichier.close()

if __name__ == '__main__':
	#recuperation du fichier data
	import glob

	values = {}
	values['debit'] = -10
	values['debit'] = debit('ENTREE_DEBIT_OUTLET.son')
	values['debit_trust'] = -10
	values['debit_trust'] = debit_trust('ENTREE_pb_Debit.out')

	properties = readPropertiesData('ENTREE.data')

	#ecriture du fichier
	ecritureFichier(properties, values)

