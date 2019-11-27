Parameters {
	Title "Convergence du calcul"
#Pset(_listresol_ %list% )
#Pset(_listcas_ %cas% )
#Pforeach _resol_ (_listresol_)	#Pforeach _cas_ (_listcas_) 
	TestCase  "_cas___resol_" "cas.data" #Pif(("_cas_"=="amont")and("_resol_"=="20")) jdd en annexe #Pendif
#Pendforeach(_cas_) #Pendforeach(_resol_) 

	Author "GF "
	IncludeData 2
	Prerequisite "./extract_convergence"
	Description "Cas test periodique, pour tester convergence en maillage des opérateurs de convection"
}


Chapter {
	Title Convergence en maillage en vitesse 
	Figure {
	labelx N
	labely erreur max 
	logx
	logy
	Include_Description_Curves 0
	width 12cm
	Title "convergence de la convection vitesse en maillage "
	#Pforeach _cas_ (_listcas_) 
		Curve {
		legend _cas_
		file conv_glob__cas__qdm
		  style linespoints
		}
		#Pendforeach(_cas_) 
		Curve {
		legend N^-1 
		fonction 1./x
		  style lines
		}
		Curve {
		legend N^-2 
		fonction 1./x/x
		  style lines
		}
	}			
} 
Chapter {
	Title Convergence en maillage scalaire 
	Figure {
	labelx N
	labely erreur max 
	logx
	logy
	Include_Description_Curves 0
	width 12cm
	Title "convergence de la convection T en maillage "
	#Pforeach _cas_ (_listcas_) 
		Curve {
		legend _cas_
		file conv_glob__cas__T
		  style linespoints
		}
		#Pendforeach(_cas_) 
	      Curve {
		legend N^-1 
		fonction 1./x
		  style lines
		}
		Curve {
		legend N^-2 
		fonction 1./x/x
		  style lines
		}
	}			
} 
