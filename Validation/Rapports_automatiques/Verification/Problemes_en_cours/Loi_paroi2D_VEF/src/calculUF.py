import os,os.path, sys, math
##
##
def properties():
	# ouverture des fichiers
	nomFic = 'propertiesGeometry.dat'
			
	fic = open(nomFic,'r')
	
	# lecture de ligne -> entetes
	fichier = fic.readlines()
	
	ligne = fichier [0]
	tLigne = ligne.split()
	mu=float(tLigne[0])
	rho=float(tLigne[1])
	Ly=float(tLigne[2])
	fic.close()
	return Ly,mu,rho
##
##
def UU(USTAR, force, velocity, Ly, mu, rho):
#
	Umean = 2
	nu = mu/rho
	REb = Umean*Ly/(2*nu)
	pu = 0.875
	REtau = 0.175*(REb)**pu
	uth = REtau*(nu)/(Ly/2)
	Ftheoric = uth*uth
	#print 'Ftheoric= %s' % (Ftheoric)	
#
	f = open(USTAR,'r')
	l = f.readlines()
	f.close()
	ligne = l[-1].strip()
	tLigne = ligne.split()
	Utau = float(tLigne[1])
	DELTA_U = 100 * abs(uth-Utau) / max(Utau,uth)
#
	ff = open(force, 'r')
	lf = ff.readlines()
	ff.close()
	ligne = lf[-1]
	#for ligne in lf:
	ligne = ligne.strip()
	tLigne = ligne.split()
	valfP = abs(float(tLigne[1]))
	DELTA_F = 100 * abs(valfP-Ftheoric) / max(valfP,Ftheoric)
##
## normalization
	vv = open(velocity, 'r')
	lv = vv.readlines()
	ficn = open('UNORM.dat', 'w')
	for lignev in lv:
		lignev = lignev.strip()
		if len(lignev)>0 and lignev[0]!='#':
			tLignev = lignev.split()
			yl = float(tLignev[0])
			yy = yl*Utau*rho/mu
			vx = float(tLignev[1])
			VX = vx/Utau
			ficn.write('%18.3f %18.3f\n' % (yy, VX))
	ficn.close()

##
##
	fic = open('UF.dat', 'w')
	fic.write('%18.3f %18.3f %18.3f %18.3f\n' % (Utau,DELTA_U,valfP,DELTA_F))
	fic.close()
#

#########1
#########
if __name__=='__main__':
	#recuperation des parametres passes en ligne de commande
	args = sys.argv
	if len(args)!=4:
		print 'Erreur sur le nb d\'arguments fournis : Usage\npython ustar gradP sonde_velocity'
		sys.exit()
# 
	USTAR = args[1]
	force = args[2]
	velocity = args[3]
#
	#recuperation des donnees
	Ly,mu,rho = properties()
	UU(USTAR, force, velocity, Ly, mu, rho)
