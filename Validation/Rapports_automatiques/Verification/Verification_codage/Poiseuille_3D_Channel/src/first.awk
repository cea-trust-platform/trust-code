#!/bin/bash
#Postraitement pour canal laminaire"
tps_init_moyen=0
LC_NUMERIC=C

#Recuperation des donnees necessaires dans jdd
#---------------------------------------------
mu=`grep "mu" $1.data| awk '{print $4}' | head -1`
rho=`grep "rho" $1.data| awk '{print $4}' | head -1`
h=`grep "Longueurs" $1.data| awk '{print $3}' | head -1`
prof=`grep "Longueurs" $1.data| awk '{print $4}' | head -1`
Um=`grep "vitesse Champ_Uniforme" $1.data| awk '{print $4}' | head -1`
echo $mu > mu.dat
echo $rho > rho.dat
echo $h $prof > dim.dat
echo $Um  > Um.dat

