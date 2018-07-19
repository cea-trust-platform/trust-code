#!/bin/bash
# Teste une liste de solveurs sur un jeu de donnees passa ou non en argument
# Par defaut on teste le solveur en pression
# Mais si on peut tester le solveur implicite, on le fait prioritairement
# echo "Usage: `basename $0` [-all] [-gpu_only] [-not_lml] [-test_solveur] [-solver n1:n2:n3] [-mail] [datafile]"
echo "Usage: `basename $0` [-all] [-gpu_only] [-not_lml] [-test_solveur] [-solver n1:n2:n3] [datafile]"
OK()
{
   if [ $1 != 0 ]
   then
      err=$err$2" " && exit=-1
      echo "KO, voir $err"
   fi
   # On recupere des infos sur le maillage et la matrice
   size=`awk '/Total number/ && /elements/ {elem=$NF} /Nombre total/ && /elements/ {elem=$NF} /Ordre de la matrice assemblee/ {n=$NF} END {print "(Tailles maillage: "elem" matrice: "(n!=""?n:elem)" )"}' $output`
}

print()
{
    out=$1
    cpu=""
    cpu0=""
    cpu=""
    its=""
    ram=""
    res=""
    OK="KO"   
    if [ "$err" = "" ]
    then
       # Numero de la resolutions a partir de laquelle on mesure (4 pour solveur_pression, 1 pour le solveur_implicite)
       [ $resolution = solveur_pression ]  && nr=4  # Car avant: solveur_optimal eventuel+projection+resolution laplacien(P)=0
       [ $resolution = solveur_implicite ] && nr=2  # Car avant: solveur_optimal eventuel
       nb_resolution=`grep 'clock Ax=B' $out | grep $resolution | $TRUST_Awk '{n++} END {print n}'` 
       if [ $nb_resolution -lt $nr ]
       then
          echo "Le cas $ref.data n'a pas assez de resolutions avec $resolution" && exit -1
       fi
       # Calcul du cpu moyen passe dans le solveur entre la resolution nr et la derniere
       cpu=` grep 'clock Ax=B' $out     | grep $resolution | $TRUST_Awk -v nr=$nr '(NR>=nr) {n++;s+=$3} END {printf("%7.4f",s/n)}'` 
       # Calcul du cpu passe dans le solveur lors de la 1ere resolution (mal connu encore pour solveur optimal)
       if [ "`echo $solver | grep -i optimal`" = "" ]
       then
          cpu0=`grep 'clock Ax=B' $out     | grep $resolution | $TRUST_Awk '(NR==1) {s=$3} END {printf("%7.3f",s)}'`
       else
          cpu0="  nc"
       fi
       # Nombre d'iterations moyen passes dans le solveur
       its=` grep -E 'Convergence [ei]n' $out | grep $resolution | $TRUST_Awk -v nr=$nr '(NR>=nr) {n++;s+=$3} END {printf("%3d",s/n)}'`
        # Calcul du residu relatif
       res=` grep -E 'Residu final:|Final residue:' $out  | $TRUST_Awk -v nr=$nr '(NR>=nr) {n++;s+=$5} END {printf("%2.0e",(n>0?s/n:s))}'`      
       # Calcul de la RAM
       ram=`$TRUST_Awk '/ RAM / && ($2=="MBytes") {if ($1>r) r=$1} END {printf("%5d",r)}' $1`
       OK="OK"
    fi
    echo $ECHO_OPTS "$cpu\t$cpu0\t$its\t$res\t$ram\t$OK\t[$i]\t$solver\t" | tee -a rank
}
print_init()
{
    echo $ECHO_OPTS "cpu\tcpu0\tits\tres\tram[Mo]\tEtat\tSolveur" | tee -a rank
}

# On regarde si le solveur est operationnel en retournant run=0 ou 1
evalue_solveur()
{
   nc=0
   run=1 && [ "${par[$i]}" = -1 ] && run=0 && nc=1						# Le solveur ne marche pas
   [ "$NB_PROCS" != "" ] && [ "${par[$i]}" = 0 ] && run=0 && nc=1				# Le solveur ne marche pas en parallele
   [ "$NB_PROCS" = "" ]  && [ "${par[$i]}" = 1 ] && run=0 					# Le solveur ne marche pas en sequentiel
   [ "$resolution" = solveur_implicite ] && [ "${sym[$i]}" = 1 ] && run=0 			# Le solveur resout uniquement des matrices symetriques
   [ "$resolution" = solveur_pression ] && [ "${sym[$i]}" = 0 ] && run=0 			# Le solveur ne marche pas sur certaines matrices symetriques
   [ "$all" = 0 ] && [ "`echo ${solver[$i]} | grep CLI`" != "" ] && run=0			# Par defaut, on ne passe pas les solveurs definis par CLI
   [ "$all" = 0 ] && [ "$resolution" = solveur_pression ] && [ "${sym[$i]}" = "" ] && run=0	# Par defaut, on ne passe pas les solveurs symetriques sur solveur_pression 
   [ "$all" = 0 ] && [ "${rank[$i]}" = "" ] && run=0						# Par defaut, on ne passe que les solveurs ayant un ranking
   [ "$PETSC_HAVE_CUDA" = 0 ] && [ "${gpu[$i]}" = 1 ] && run=0					# Discards solvers tested on GPU if PETSC_HAVE_CUDA=0
   [ "$gpu_only" = 1 ] && [ "${gpu[$i]}" != 1 ] && run=0					# Run solvers tested on GPU only
}

#####################
# Lecture des options
##################### 
all=0 && [ "$1" = -all ] && all=1 && shift
gpu_only=0 && [ "$1" = -gpu_only ] && gpu_only=1 && shift
lml=1 && [ "$1" = -not_lml ] && lml=0 && shift
test_solveur=0 && [ "$1" = -test_solveur ] && test_solveur=1 && lml=0 && shift
mail=0 && [ "$1" = -mail ] && mail=1 && shift
[ "$1" = -solver ] && shift && liste_solveurs=`echo $1 | awk '{gsub(":"," ",$0);print $0}'` && shift
ref=${1%.data} && shift

# Determination du jeu de donnees
if [ "$ref" = "" ]
then
   ref=`pwd`
   ref=`basename $ref`
   # Le parallele a deja tourne:
   [ -f PAR_$ref.out ] && ref=PAR_$ref 
fi
[ ! -f $ref.data ] && echo "Fichier $ref.data inexistant" && exit -1
# On regarde si le jeu de donnees est parallele (contient Scatter)
PARALLEL=`awk 'BEGIN {np=""} /#/ {n=n+split($0,a,"#")-1} (tolower($1)=="scatter" || tolower($1)=="scatterformatte") {if (n%2==0) print 1;else print 0;exit}' $ref.data` 
# Recupere le nombre de processeurs necessaire
if [ "$PARALLEL" = 1 ]
then
   NOM=`ls *_0000.Zones 2>/dev/null | head -1`
   [ "$NOM" = "" ] && echo "Le decoupage sur le cas $ref.data n'a pas ete fait." && exit -1 
   NB_PROCS=`ls ${NOM%0000.Zones}*.Zones | wc -l` 
else
   NB_PROCS=""
fi

if [ "`grep -i seuil_convergence_solveur $ref.data`" != "" ]
then
   #########################################
   # Cas on l'on change le solveur implicite
   #########################################
   resolution="solveur_implicite"
   if [ "`grep -i seuil_convergence_solveur $ref.data | wc -l`" -ne 1 ]
   then
      echo "Cas avec aucun ou plusieurs seuil_convergence_solveur dans $ref.data encore non prevu"
      exit -1
   fi
   if [ "`grep -i 'solveur ' $ref.data | grep '{' | grep -i seuil`" != "" ]
   then
      echo "Le cas ou le solver implicite est explicitement defini dans $ref.data n'est pas encore prevu."
      exit -1
   fi
   solveur_initial=`grep -i seuil_convergence_solveur $ref.data`
   keyword=$solveur_initial" solveur"
   # Recherche du seuil
   seuil=`grep -i seuil_convergence_solveur $ref.data | awk '{print $2}'`
else
   ###########################################
   # Cas on l'on change le solveur en pression
   ###########################################
   resolution="solveur_pression"
   nb_solveur_pression=`grep -i solveur_pression $ref.data | wc -l`
   if [ "$nb_solveur_pression" = 0 ]
   then
      echo "Cas avec aucun solveur_pression dans $ref.data encore non prevu"
      exit -1
   fi
   # On prend le dernier solveur en pression trouve dans le jeu de donnees
   # supposa traitant le plus gros maillage...
   keyword="solveur_pression"
   # Recherche du seuil (un solveur defini sur plusieurs lignes fonctionne)
   seuil="`$TRUST_Awk -v nb_solveur_pression=$nb_solveur_pression 'BEGIN {IGNORECASE=1} \
   		/solveur_pression/ {nb++} \
		/solveur_pression/ && (nb==nb_solveur_pression) {\
			while(index($0,"seuil")==0 && getline) {};\
			while($i!="seuil" && i++<NF);print $(i+1);exit}\
		' $ref.data`"
fi
# Seuil non trouve (Cholesky) on prend une valeur quelconque
[ "$seuil" = "" ] && seuil=1.e-6

# Chargement d'une liste de solveurs de la version
# Peut etre eventuellement surchargee si un fichier est en local
let i=0
liste=Check_solver.list
if [ -f $liste ]
then
   . ./$liste
else
   . $TRUST_ROOT/bin/KSH/$liste
fi

# Pour initialiser
rm -f rank
[ $lml = 1 ] && trust $ref $NB_PROCS 1>/dev/null 2>&1

# Titre
titre="Calcul "`awk 'BEGIN {IGNORECASE=1} /VDF / {print "VDF";exit} /VEFPREP1B / {print "VEF";exit}' $ref.data`
if [ "$NB_PROCS" = "" ]
then
   titre=$titre" sequentiel sur 1 proc" 
else
   titre=$titre" parallele sur $NB_PROCS procs"
fi
titre=$titre" de $HOST"

let i=1
exit=0

# Cas ou l'on utilise le mot cle TRUST test_solveur pour tester N solveurs en 1 calcul
if [ $test_solveur = 1 ]
then
   # On cree la liste des solveurs a tester (ceux ayant un rank)
   while [ "${solver[$i]}" != "" ]
   do
      if [ "$liste_solveurs" = "" ] || [ "`echo ' '$liste_solveurs' ' | grep ' '$i' '`" != "" ]
      then
         solver=${solver[$i]}      
	 evalue_solveur
         [ $run = 1 ] && echo ${rank[$i]}" solveur "${solver[$i]}
      fi
      let i=$i+1
   done | sort -n | cut -c3- > liste_solveur
   # On cree un nouveau fichier en ajoutant la liste des solveurs en fin de fichier (on tient compte du fin en mettant apres Resoudre)
   $TRUST_Awk 'BEGIN {IGNORECASE=1} // {print $0} /Resoudre / {print "Test_solveur { fichier_solveur liste_solveur }"}' $ref.data > test_solveur_$ref.data
   # On introduit le mot cle save_matrice (faire mieux?) dans le solveur
   if [ $resolution = solveur_pression ]
   then        
      # Fonctionne s'il est defini sur plusieurs lignes
      tmp_data=`mktemp_`
      $TRUST_Awk -v nb_solveur_pression=$nb_solveur_pression 'BEGIN {IGNORECASE=1} \
		/solveur_pression/ {nb++} \
           	/solveur_pression/ && (nb==nb_solveur_pression) {sub("{","{ save_matrice",$0);print $0} \
	   	!/solveur_pression/ || (nb!=nb_solveur_pression) {print $0} 
         	' test_solveur_$ref.data > $tmp_data
      mv -f $tmp_data test_solveur_$ref.data
      # echo -e "/solveur_pression/ s?{[ \t]?{ save_matrice ?\nw" | ed test_solveur_$ref.data 1>/dev/null 2>&1
   fi
   if [ $resolution = solveur_implicite ]
   then
      echo -e "1,$ s?seuil_convergence_solveur.*? solveur gmres { save_matrice seuil $seuil controle_residu 1 diag impr }?g\nw" | ed test_solveur_$ref.data 1>/dev/null 2>&1
   fi
   # On fait 1 seul pas de temps pour creer la matrice et le second membre
   echo -e "1,$ s?nb_pas_dt_max .*?nb_pas_dt_max 1?g\nw" | ed test_solveur_$ref.data 1>/dev/null 2>&1
   # On lance le calcul (qui fera un pas de temps puis qui testera les solveurs apres la resolution)
   output=test_solveur_$ref.out_err
   trust test_solveur_$ref $NB_PROCS 1>out 2>err
   run_err=$? 
   cat out err > $output ; rm -f out err
   OK $run_err $output
   echo $ECHO_OPTS $titre $size
   print_init
   cat $output | $TRUST_Awk 'BEGIN {print "CPU Solveur"} /Essai [0-1]/ || /Try [0-1]/ {try=$2;if (try) {solv=$NF;while ($NF!="}") {getline;solv=solv" "$0}}} /CPU=/ {cpu[try]=$2;if (try) print cpu[1]" \t"cpu[0]" "solv}' > perf
   cat perf | awk '/{/ {gsub("Solv_","\tnc \tnc \tnc \tOK \t["NR-1"] \t",$0);print $0}' | sort -n
   # [ "$mail" = 1 ] && cat perf | mail_ -s\"[Check_solver.sh] $HOST: Tests solveurs sur $ref.data et $NB_PROCS processeurs\" $TRUST_MAIL
   echo "$HOST: Tests solveurs sur $ref.data et $NB_PROCS processeurs:"
   cat perf
   exit $exit
fi

# Cas ou l'on boucle sur les N solveurs en lancant N calculs
echo $ECHO_OPTS $titre | tee -a rank
print_init
while [ "${solver[$i]}" != "" ]
do
   if [ "$liste_solveurs" = "" ] || [ "`echo ' '$liste_solveurs' ' | grep ' '$i' '`" != "" ]
   then
      err=""
      solver=${solver[$i]}
      jdd=$ref$i
      if [ "$keyword" = solveur_pression ]
      then
         # Changement du solveur de pression dans le jeu de donnees (Fonctionne s'il est defini sur plusieurs lignes)
         $TRUST_Awk -v nb_solveur_pression=$nb_solveur_pression -v solver="$solver" 'BEGIN {IGNORECASE=1} \
	 	/solveur_pression/ {nb++} \
           	/solveur_pression/ && (nb==nb_solveur_pression) {print "solveur_pression "solver; getline;while (($1=="{")+index($0,"precond")+index($0,"omega")+index($0,"impr")+index($0,"seuil")+($1=="}")) getline}
	   	!/solveur_pression/ || (nb!=nb_solveur_pression) {print $0} 
         	' $ref.data > $jdd.data
      else
         # Changement du solveur implicite dans le jeu de donnees (Fonctionne que si c'est sur une seule ligne)
         echo $ECHO_OPTS "1,$ s?$solveur_initial?$keyword $solver?g\nw $jdd.data" | ed $ref.data 1>/dev/null 2>&1
      fi
      # Changement de dt_impr pour imprimer a chaque pas de temps
      dt_impr=`grep -i dt_impr $jdd.data`
      echo $ECHO_OPTS "1,$ s?$dt_impr?dt_impr 1.e-10?g\nw" | ed $jdd.data 1>/dev/null 2>&1

      # On regarde si le solveur est operationnel
      evalue_solveur
      
      output=$jdd.out_err
      if [ $run = 1 ]
      then
         rm -f gmon.out
	 # trust $jdd $NB_PROCS 1>$output 2>&1
	 # Bug vu sur trust sur castor ou CCRT, il faut rediriger dans 2 fichiers distincts:
	 trust $jdd $NB_PROCS 1>out 2>err
	 run_err=$?
	 cat out err > $output
	 rm -f out err
	 OK $run_err $output
	 if [ $run_err = 0 ] && [ $lml = 1 ]
	 then
	    if [ -f $ref.lml ]
	    then
	       format=lml
	    elif [ -f $ref.lata ]
	    then
	       format=lata
	    else
	       echo "No $ref.lml or $ref.lata found !" 
	       exit -1
	    fi
	    compare_lata $ref.$format $jdd.$format 1>>$output 2>&1
	    OK $? $output
	 fi
	 [ -f gmon.out ] && gprof $exec 1>$jdd.cpu
	 print $output
	 # Menage
	 mkdir tmp
	 mv -f $output $jdd.data $jdd.TU $jdd.dt_ev tmp/. 2>/dev/null && [ -f $jdd.cpu ] && mv $jdd.cpu tmp/.
	 rm -f $jdd.* $jdd"_"*
	 mv tmp/* .
	 rmdir tmp	 
      elif [ $nc = 1 ]
      then
	 echo $ECHO_OPTS "\t\t\t\t\tNonCode\t[$i]    $solver" | tee -a rank
      fi
   fi
   let i=$i+1
done
echo "********"
echo "RANKING:"
echo "********"
echo $ECHO_OPTS "1,$ s?$HOST?$HOST $size?g\nw" | ed rank 1>/dev/null 2>&1
head -2 rank | tee ranking.$$
# tail +3 pas portable
awk '(NR>2) && ($6=="OK")' rank | sort -n | tee -a ranking.$$
echo "Saved in ranking.$$ file"
# [ "$mail" = 1 ] && cat ranking.$$ | mail_ -s\"[Check_solver.sh] NP sur $HOST\" $TRUST_MAIL
echo "NP sur $HOST:"
cat ranking.$$
exit $exit
