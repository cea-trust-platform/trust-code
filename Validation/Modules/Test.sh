#!/bin/bash


test_module()
{
module=$1
cas=$2
lancete=$PWD/$module/lance_test_modif
cd $module
sh ../fait_lance_test $PWD 
[ -f ./fait_lance_test ] && sh ./fait_lance_test $PWD 
echo $cas | $lancete -nomail -efface $exec 1>out 2>err
status=$?

if [ $status -eq 0 ]
then
 echo $module ready to run
 rm -f err out 
else
   echo $module NOT ready to run
fi
cd ..
return $status
}
export PAR_F=0
status_tot=0
test_module QC docond
[ $? -ne 0 ] && status_tot=1

test_module Mu_var Bilan_chaleur_vef_jdd1
[ $? -ne 0 ] && status_tot=1

test_module Matrice_Implicite Constituants_VEF_impl
[ $? -ne 0 ] && status_tot=1

test_module Petsc_Cholesky Cholesky
[ $? -ne 0 ] && status_tot=1

test_module Petsc_GCP Cavite_paroi_defilante_hexa
[ $? -ne 0 ] && status_tot=1

test_module Poreux Bilan_chaleur_vef_jdd1
[ $? -ne 0 ] && status_tot=1

test_module Rayo_transp Bilan_chaleur_vef_jdd1
[ $? -ne 0 ] && status_tot=1

test_module Reprise upwind
[ $? -ne 0 ] && status_tot=1

test_module Rotation docond
[ $? -ne 0 ] && status_tot=1


exit $status_tot
