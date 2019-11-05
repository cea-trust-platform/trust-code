#Script de recuperation des donnees u* et calcul de U+ et y+

import optparse
import math
import os

#copie du fichier dans un fichier au nom generique
def copieFichier(nomFic,nomFicGen):
	#ecriture du fichier
	ficRead = open(nomFic,'r')
	ficWrite = open(nomFicGen, 'w')

	fin=False
	while not fin:
		ligne = ficRead.readline()
		if not ligne:
			fin=True
		else:
			ficWrite.write('%s' % (ligne))
	
	ficWrite.close()
	ficRead.close()

if __name__ == '__main__':
	
	parser = optparse.OptionParser()
	(options, args) = parser.parse_args()
	
	#recuperation du fichier avec le dernier temps dans le nom
	#derniere ligne du ls
	ficLS = os.popen('ls -rt '+args[0]+'_*')
	lignes = ficLS.readlines()
	derLigne = lignes[-1]
	#suppression du \n en fin de nom
	nomFic = derLigne[:len(derLigne)-1]
	
	#ecriture du fichier gnuplot
	copieFichier(nomFic,args[0])
