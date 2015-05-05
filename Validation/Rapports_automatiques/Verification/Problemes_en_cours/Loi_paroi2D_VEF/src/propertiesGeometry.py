#Script de recuperation des proprietes physiques et de la geometrie

import os, sys, math

def readPropertiesData(nomFic):
	#initialisation
	properties = {}
	properties['mu'] = -1
	properties['rho'] = -1
	properties['Lx'] = -1	
	# ouverture des fichiers
	fic = open(nomFic,'r')
	
#
	
	for ligne in fic:
		ligne = ligne.strip().lower()
		tLigne = ligne.split()
		if ligne.find('champ_uniforme')>-1:
			if ligne.startswith('mu'):
				properties['mu'] = float(tLigne[-1])
			elif ligne.startswith('rho champ_uniforme'):
				properties['rho'] = float(tLigne[-1])
		elif ligne.find('longueurs')>-1:
		    	properties['Ly'] = float(tLigne[-1])
	fic.close()
	return properties


def ecritureFichier(properties):
	#ecriture du fichier
	nomFic = 'propertiesGeometry.dat'
	fichier = open(nomFic, 'w')
	fichier.write('%18.7f %18.4f %18.3f\n' % ( properties['mu'], properties['rho'], properties['Ly'] ))
	fichier.close()

def getPropertiesFromdat():
	properties = {}
	nomFichier = 'propertiesGeometry.dat'
	if os.path.isfile(nomFichier):
		#recupere les donnees du fichier
		f = open(nomFichier, 'r')
		lignes = f.readlines()
		f.close()
	else:
		print 'Erreur getPropertiesFromdat : fichier %s non trouve !' % (nomFichier)
		sys.exit()
	ligne = (lignes[0]).strip()
	tabLigne = ligne.split()
	ind = 0
	try:
		properties['mu'] = float(tabLigne[ind])
		ind += 1
		properties['rho'] = float(tabLigne[ind])
		ind += 1
		properties['Ly'] = float(tabLigne[ind])
	except IndexError:
		print 'Erreur getPropertiesFromdat : lecture element %d pour 0-%d elements...' % (ind, len(tabLigne)-1)
		sys.exit()
	except ValueError:
		print 'Erreur getPropertiesFromdat : lecture element %d n\'est pas un float (%s)...' % (ind, tabLigne[ind])
		sys.exit()
	return properties



if __name__ == '__main__':
	
	#recuperation du fichier data
	import glob
	#derniere ligne du ls
	#ficLS = os.popen('ls *.data')
	#lignes = ficLS.readlines()
	#Ligne = lignes[0]
	#suppression du \n en fin de nom
	#nomFic = Ligne[:len(Ligne)-1]
	listFics = glob.glob('*.data')
	if len(listFics)>0:
		nomFic = listFics[0]
		properties = readPropertiesData(nomFic)
	
		#ecriture du fichier
		ecritureFichier(properties)
	else:
		print 'Erreur propertiesGeometry : pas de fichier data trouve !'
