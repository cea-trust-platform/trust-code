#!/bin/bash
#

##################################
# Verification des classes Matrice
##################################
files=`ls Matrice_*.h | sed '/Matrice_Sym.h/d' | sed '/Matrice_SuperMorse.h/d' | sed '/Matrice_Base.h/d' | $TRUST_Awk -F. '{print $1}'`
for file in $files
do
   for ext in h
   do
      # Recherche des methodes qui doivent absolument etre code dans le ".$ext"
      methode_code="ajouter_multvect_ ajouter_multvectT_ ajouter_multTab_"
      for methode in $methode_code
      do
         code=`grep -i "Double.*&.*$methode(" $file.$ext`
         if [ "$code" == "" ]
         then
            echo "La methode \"$methode\" n'est pas trouvee or elle doit absolument etre code dans $file.$ext"
	    echo ""
	    exit 1
         fi
      done

      # Recherche des methodes qui ne doivent absolument pas etre code dans le ".$ext"
      methode_code="ajouter_multvect ajouter_multvectT ajouter_multTab multvect_ multvect multvectT_ multvectT multTab multTab_"
      for methode in $methode_code
      do
         code=`grep -i -n "[^_]$methode(" $file.$ext`
         if [ "$code" != "" ]
         then
            echo "$file.$ext" ; echo "$code"
	    echo "La methode \"$methode\" ne doit absolument pas etre code dans $file.$ext mais dans Matrice_base.$ext"
	    echo ""
	    exit 1
         fi
      done

      # Recherche des echanges d'espaces virtuels qui ne doivent pas etre dans les ".cpp"
      methode_code="echange_espace_virtuel"
      for methode in $methode_code
      do
         code=`grep -i -n "$methode(" $file.cpp`
         if [ "$code" != "" ]
         then
            echo "$file.$ext" ; echo "$code"
	    echo "La methode \"$methode\" ne doit absolument pas apparaitre dans $file.cpp"
	    echo ""
	    exit 1
	 fi
      done
   done
done
