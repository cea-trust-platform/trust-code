Parametres {
	Titre "Convergence du calcul"
#Pset(_listresol_ %list% )
#Pset(_listcas_ %cas% )
#Pforeach _resol_ (_listresol_)	#Pforeach _cas_ (_listcas_) 
	Castest  "_cas___resol_" "cas.data" #Pif(("_cas_"=="amont")and("_resol_"=="20")) jdd en annexe #Pendif
#Pendforeach(_cas_) #Pendforeach(_resol_)

	Auteur "GF "
	InclureData 2
	Prerequis "./extract_convergence"
	Description "Cas test periodique, pour tester convergence en maillage des opérateurs de convection"
}


Chapitre {
	titre Convergence en maillage en vitesse 
	Figure {
	labelx N
	labely erreur max 
	logx
	logy
	incluredesccourbes 0
	width 12cm
	titre "convergence de la convection vitesse en maillage "
	#Pforeach _cas_ (_listcas_) 
		courbe {
		legende _cas_
		fichier conv_glob__cas__qdm
		  style linespoints
		}
		#Pendforeach(_cas_) 
		courbe {
		legende N^-1 
		fonction 1./x
		  style lines
		}
		courbe {
		legende N^-2 
		fonction 1./x/x
		  style lines
		}
	}			
} 
Chapitre {
	titre Convergence en maillage scalaire 
	Figure {
	labelx N
	labely erreur max 
	logx
	logy
	incluredesccourbes 0
	width 12cm
	titre "convergence de la convection T en maillage "
	#Pforeach _cas_ (_listcas_) 
		courbe {
		legende _cas_
		fichier conv_glob__cas__T
		  style linespoints
		}
		#Pendforeach(_cas_) 
	      courbe {
		legende N^-1 
		fonction 1./x
		  style lines
		}
		courbe {
		legende N^-2 
		fonction 1./x/x
		  style lines
		}
	}			
} 
