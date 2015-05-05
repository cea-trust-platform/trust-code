#!/bin/bash
# Run Gmsh if installed (take the more recent one)
Gmsh=`ls -rt $TRUST_ROOT/Outils/Gmsh/gmsh $TRUST_ROOT/Outils/Gmsh/gmsh*/gmsh $TRUST_ROOT/Outils/Gmsh/gmsh*/bin/gmsh 2>/dev/null | tail -1`
if [ ${#Gmsh} = 0 ]
then
   error="Gmsh, a tool used to build unstructured meshes, is not installed. Please download a version at http://www.geuz.org/gmsh and install it (you may need also OpenGL libraries) under $TRUST_ROOT/Outils/Gmsh"
   if [ ${#1} != 0 ]
   then
      echo $ECHO_OPTS $error
   else
      Erreur.ihm $error
   fi
elif [ ${#1} = 0 ]
then
   $Xterm $HautDroit_geometrie -title "Unstructured mesh with Gmsh" -e Run_Gmsh $Gmsh 
fi
