#!/bin/bash
#Postraitement pour canal laminaire"
tps_init_moyen=0
LC_NUMERIC=C

#On recupere les donnees utiles
rho=`cat  ../rho.dat`
mu=`cat  ../mu.dat`
h=`cat  ../dim.dat | awk '{print $1}' | head -1`
prof=`cat  ../dim.dat | awk '{print $2}' | head -1`
Um=`cat  ../Um.dat | awk '{print $1}' | head -1`
dhy=`awk "BEGIN {print 4.*0.5*$h}" `
#
#Resultats concernant les pertes de charge
#---------------------------------------------
# Calcul des valeurs theoriques
Rehyd=`awk "BEGIN {print $Um*$rho*$dhy/($mu)}" `
uTau=`awk "BEGIN {print $Um*sqrt(12./($Rehyd))}" `
ReTau=`awk "BEGIN {print $uTau*$rho*$h/2./($mu)}" `
PG=`awk "BEGIN {print 2.*$uTau*$uTau/($h)}" `
echo $Um $Rehyd $ReTau > ../hydrau.dat
#Recuperation des resultats Trio_U
uTauTrio=`tail u_tau.dat | awk '{print $2}' | head -1`
ReTauTrio=`tail reynolds_tau.dat | awk '{print $2}' | head -1`
Pressure_Gradient=`ls *Pressure_Gradient_pb_periox`
Channel_Flow_Rate=`ls *Channel_Flow_Rate_pb_periox`
PGTrio=`tail $Pressure_Gradient | awk '{print $2}' | head -1`
debTrio=`tail $Channel_Flow_Rate | awk '{print $2}' | head -1`
UmTrio=`awk "BEGIN {print $debTrio/($h*$prof)}" `
ReTrio=`awk "BEGIN {print $UmTrio*$rho*$dhy/($mu)}" `
echo $UmTrio $ReTrio $ReTauTrio > hydrau.dat
#Calcul des ecarts
deltaPG=`awk "BEGIN {print ($PGTrio-$PG)*100./($PG)}" `
deltaUtau=`awk "BEGIN {print ($uTauTrio-$uTau)*100./($uTau)}" `
deltaRetau=`awk "BEGIN {print ($ReTauTrio-$ReTau)*100./($ReTau)}" `

awk -v UtTrio=$uTauTrio -v Ut=$uTau -v dUt=$deltaUtau -v PTrio=$PGTrio -v P=$PG -v dP=$deltaPG 'BEGIN {printf ("%.5f %.5f %.1f %.5f %.5f %.1f\n",UtTrio,Ut,dUt,PTrio,P,dP)}'  > press.dat

exit
