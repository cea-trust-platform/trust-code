# Estimation du nombre de boucles
dis=$1
src=$TRUST_ROOT/src/$dis

som=" grep -Ri for $src | grep -e nb_som   | grep -v face | grep -v elem | grep -iv kokkos"
elem="grep -Ri for $src | grep -e nb_elem | grep -iv kokkos"
face="grep -Ri for $src | grep -e nb_faces | grep -v elem | grep -v fr | grep -v bord | grep -iv kokkos"

nb_elem=`eval $elem | tee for_elem | wc -l`
nb_face=`eval $face | tee for_face | wc -l`
nb_som=`eval $som   | tee for_som  | wc -l`

nb=`echo "$nb_elem+$nb_face+$nb_som" | bc -l`
kokkos=`grep -Ri KOKKOS_LAMBDA $src | tee for_kokkos | wc -l`
ompt=`grep -Ri "omp target" $src | tee for_ompt | wc -l`

echo "TRUST $dis: (Kernel=boucle sur un item du maillage: elem, face ou som)"
echo "------------------"
echo "Kernels CPU    : "$nb dont $nb_elem elem $nb_face face $nb_som som 
echo "Kernels Kokkos : "$kokkos
echo "Kernels OMPT   : "$ompt
taux=`echo "scale=1;100*($kokkos+$ompt)/($nb+$kokkos+$ompt)" | bc -l`
echo "------------------"
echo "Portage GPU    : "$taux"%"
echo "------------------"
echo "Voir detail des boucles for dans les fichiers: for_elem for_face for_som for_kokkos for_ompt"

