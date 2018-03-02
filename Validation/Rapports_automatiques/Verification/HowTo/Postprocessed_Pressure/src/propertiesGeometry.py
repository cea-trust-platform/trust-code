#Script de recuperation des proprietes physiques et de la geometrie

import os, sys, math
import numpy as np

def readPropertiesData(nomFic):
	#initialisation
	properties = {}
	properties['mu'] = -1
	properties['rho'] = -1
	properties['Vmax'] = -1
	properties['L'] = -1
	properties['R'] = -1
        properties['Vmoy']= -1
	properties['Re'] = -1
        properties['Qv'] = -1
	properties['delta_P_theo'] = -1
	# ouverture des fichiers
	fic = open(nomFic,'r')

	for ligne in fic:
		ligne = ligne.strip().lower()
		tLigne = ligne.split()
		if ligne.find('champ_uniforme')>-1:
			if ligne.startswith('mu'):
				properties['mu'] = float(tLigne[-1])
			elif ligne.startswith('rho'):
				properties['rho'] = float(tLigne[-1])
		elif ligne.find('vmax')>-1:
			properties['Vmax'] = float(tLigne[-2])
		elif ligne.find('longueur')>-1:
			properties['L'] = float(tLigne[-2])
		elif ligne.find('rayon')>-1:
			properties['R'] = float(tLigne[-2])
	fic.close()
        D=2*properties['R']
        K=-4*properties['mu']*properties['Vmax']/properties['R']**2
        properties['Vmoy']=-K*properties['R']**2/(8*properties['mu'])
        properties['Re']=properties['rho']*properties['Vmoy']*D/properties['mu']
        properties['Qv']=-K*np.pi*properties['R']**4/(8*properties['mu'])
        properties['delta_P_theo']=-K*properties['L']
	return properties

def dPtrustmoy(nomFic):
	fic = open(nomFic,'r')
	# lecture de ligne -> entetes
	fichier = fic.readlines()
	#tant que la derniere ligne est vide
	while fichier[-1]=="" or fichier[-1]=="\n":
		del fichier [-1]
	ligne = fichier[-1]
	tLigne = ligne.split()
	ptrust=float(tLigne[-1])
	fic.close()
	return ptrust

def dPtrust(nomFic):
	fic = open(nomFic,'r')
	# lecture de ligne -> entetes
	fichier = fic.readlines()
	#tant que la derniere ligne est vide
	while fichier[-1]=="" or fichier[-1]=="\n":
		del fichier [-1]
	ligne = fichier[-1]
	tLigne = ligne.split()
	dptrust=abs(float(tLigne[-1])-float(tLigne[-2]))
	fic.close()
	return dptrust

def meanPOutlet(nomFic):
	fic = open(nomFic,'r')
	# lecture de ligne -> entetes
	fichier = fic.readlines()
	#tant que la derniere ligne est vide
	while fichier[-1]=="" or fichier[-1]=="\n":
		del fichier [-1]
	ligne = fichier[-1]
	tLigne = ligne.split()
	meanPOutlet=float(tLigne[-1])
	fic.close()
	return meanPOutlet

def Qvtrust(nomFic):
	fic = open(nomFic,'r')
	# lecture de ligne -> entetes
	fichier = fic.readlines()
	#tant que la derniere ligne est vide
	while fichier[-1]=="" or fichier[-1]=="\n":
		del fichier [-1]
	ligne = fichier[-1]
	tLigne = ligne.split()
	Qvtrust=abs(float(tLigne[-2]))
	fic.close()
	return Qvtrust

def ecritureFichier(properties, values):
	nomFic = 'propertiesGeometry.dat'
	fichier = open(nomFic, 'w')
	fichier.write('%18.3e %18.3f %18.3f %18.3f %18.3e %18.3e %18.4e %18.4e %18.4e %18.4e %18.4e %18.4e %18.4e %18.4e %18.4e %18.4e %18.4e\n' % ( properties['mu'], properties['rho'], properties['Vmax'], properties['Vmoy'], properties['Qv'], properties['Re'], properties['delta_P_theo'], values['dptrust1'], values['delta_P1_error'], values['dptrust2'], values['delta_P2_error'], values['dptrust3'], values['delta_P3_error'], values['dptrustmoy2'], values['delta_Pmoy2_error'] , values['Qvtrust'], values['QvError'] ))
	fichier.close()
	nomFic = 'mean_pressure.dat'
	fichier = open(nomFic, 'w')
	fichier.write('%18.3e %18.3e %18.3e %18.3e\n' % ( values['meanPressionOutlet'], values['meanPressionPaOutlet'], values['meanPressureOutlet'], values['meanPstarOutlet'] ))
	fichier.close()

if __name__ == '__main__':
	#recuperation du fichier data
	import glob

	values = {}
	values['Qvtrust'] = -1
	values['QvError'] = -1
	values['delta_P1_error'] = -1
	values['delta_P2_error'] = -1
	values['delta_P3_error'] = -1
	values['delta_Pmoy2_error'] = -1
        values['dptrust1'] = -1
        values['dptrust2'] = -1
        values['dptrust3'] = -1
        values['meanPressionOutlet'] = -1
        values['meanPressionPaOutlet'] = -1
        values['meanPressureOutlet'] = -1
        values['meanPstarOutlet'] = -1
	values['ptrustmoyinlet'] = -1
	values['ptrustmoyoutlet'] = -1
	values['dptrustmoy2'] = -1

	properties = readPropertiesData('PAR_VEF_P0P1_g.data')

	values['dptrust1'] = dPtrust('PAR_VEF_P0P1_g_P_TOT1.son')
	values['dptrust3'] = dPtrust('PAR_VEF_P0P1_g_P_TOT2.son')
	values['ptrustmoyinlet'] = dPtrustmoy('PAR_VEF_P0P1_g_MEAN_P_TOT_INLET.son')
	values['ptrustmoyoutlet'] = dPtrustmoy('PAR_VEF_P0P1_g_MEAN_P_TOT_OUTLET.son')
#	values['dptrustmoy2'] = dPtrustmoy('PAR_VEF_P0P1_g_DELTA_P_TOT_MOY.son') #
	values['dptrustmoy2'] = float(abs(values['ptrustmoyinlet'] -values['ptrustmoyoutlet']))

        values['meanPressionOutlet'] = meanPOutlet('PAR_VEF_P0P1_g_MEAN_PRESSION_OUTLET.son')
        values['meanPressionPaOutlet'] = meanPOutlet('PAR_VEF_P0P1_g_MEAN_PRESSION_PA_OUTLET.son')
        values['meanPressureOutlet'] = meanPOutlet('PAR_VEF_P0P1_g_MEAN_PRESSURE_OUTLET.son')
        values['meanPstarOutlet'] = meanPOutlet('PAR_VEF_P0P1_g_MEAN_P_STAR_OUTLET.son')

	values['Qvtrust'] = Qvtrust('PAR_VEF_P0P1_g_pb_Debit.out')
	values['QvError'] = float(abs(properties['Qv']-values['Qvtrust'])/properties['Qv'])*100
	values['delta_P1_error'] = float(abs(properties['delta_P_theo']-values['dptrust1'])/properties['delta_P_theo'])*100
#	values['delta_P2_error'] = float(abs(properties['delta_P_theo']-values['dptrust2'])/properties['delta_P_theo'])*100
	values['delta_P3_error'] = float(abs(properties['delta_P_theo']-values['dptrust3'])/properties['delta_P_theo'])*100
	values['delta_Pmoy2_error'] = float(abs(properties['delta_P_theo']-values['dptrustmoy2'])/properties['delta_P_theo'])*100

	#ecriture du fichier
	ecritureFichier(properties, values)
#	else:
#		print 'Erreur propertiesGeometry : pas de fichier data trouve !'




