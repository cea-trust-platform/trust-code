#!/bin/bash
# Demo_Gmsh.ksh: 
# Cet utilitaire permet de tester l'enchainement Gmsh -> TRUST
# Principe: 
#    a partir de 2 geometries:
#              2D (Gmsh2D.geo=carre) et 3D (Gmsh23D.geo=cube),
#   - lancement en batch de Gmsh, on obtient: Gmsh2D.msh et Gmsh23D.msh
#   - lancement en batch de TRUST jusqu'a la phase de discretisation
#       (jusqu'a la creation des faces)
source $TRUST_ROOT/env/env_TRUST.sh 1>/dev/null 2>&1
# Test Gmsh if installed
[ -f ~/.gmshrc ] && mv ~/.gmshrc ~/.gmshrc.bak
gmsh --version 1>/dev/null 2>&1
err=$?
[ -f ~/.gmshrc.bak ] && mv -f ~/.gmshrc.bak ~/.gmshrc
[ $err != 0 ] && echo "Sorry, can't run the demo, Gmsh is not installed." && exit 1
j=0
for i in `ls Gmsh*.geo`
do
    echo $ECHO_OPTS "\nRunning Gmsh on test case $i"
    pref=`basename $i .geo`
    let j=$j+1
    NOM=$i  
    case $pref in
       "Gmsh2D") 
          gmsh -2 $NOM -format med 1>$pref.gmsh.log 2>&1
          ;;
       "Gmsh3D")
          gmsh -3 $NOM -format med 1>$pref.gmsh.log 2>&1
          ;;
       "Gmsh3D_2")
          gmsh -3 $NOM -format med 1>$pref.gmsh.log 2>&1
          ;;
       *);;
    esac
    if [ -f $pref.med ]
    then
       echo $ECHO_OPTS "File $pref.med is ready for use"
    else
       echo $ECHO_OPTS "Error with Gmsh !"
       cat $pref.gmsh.log
       exit -1
    fi

# Lancement en batch de TRUST 

    echo $ECHO_OPTS "Running TRUST ....."
    trust $pref 1>$pref.log 2>&1
    if [ $? != 0 ]
    then
        tail -20 $pref.log
        echo $ECHO_OPTS "Error with your mesh"
        exit -1
    else
        echo $ECHO_OPTS "Test Gmsh->converter->TRUST ok" 
    fi
    sleep 1

# Nettoyage repertoire: les fichiers  *geo *msh *geom et *data doivent etre conserves
    rm -f *err *out *dump trace *param *.TU
    if [ $j -lt 1 ]
    then
       echo $ECHO_OPTS "No file .geo exist " 
       exit -1
    fi
    echo $ECHO_OPTS "        "
done
# Clean
rm -f *.med ssz*.geo *.file convert_jdd
exit 0

