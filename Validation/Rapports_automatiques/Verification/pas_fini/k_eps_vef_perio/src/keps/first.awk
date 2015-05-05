#!/bin/bash
#Postraitement d'un canal plan : champs de vitesse et thermique (T0-Q)
tps_init_moyen=0
h=`grep "Longueurs" $1.data| awk '{print 0.5*$3}'| head -1`

[ "$2" != "" ] && u=$2
  [ "$2" = "p" ] && iut2=`tail -1 $1"_Pressure_Gradient_pb_perio" | awk -v h=$h '{print $2*(h)}'`&&u=`echo "sqrt("$iut2")" | bc -l `

mu=`grep "mu " $1.data| awk '{print $4}' | head -1`
rho=`grep "rho" $1.data| awk '{print $4}' | head -1`
lambda=`grep "lambda" $1.data| awk '{print $4}' | head -1`
Cp=`grep "Cp" $1.data| awk '{print $4}' | head -1`

echo "mu " $mu    " rho" $rho  " lambda" $lambda " Cp" $Cp

Ucent=`grep "Ucent" $1.data| awk '{print $6}'`
#h=`grep "Longueurs" $1.data| awk '{print 0.5*$3}'`
Lx=`grep "Longueurs" $1.data| awk '{print $2}'`
Lz=`grep "Longueurs" $1.data| awk '{print $4}'`
Lz=1
Q=`grep "Puissance_Thermique_Uniforme" $1.data| awk '{print $4}'`
Ucent=10
Ucent=80
echo "Ucent " $Ucent "  h " $h "  Lx " $Lx"  Lz " $Lz
echo "Q = " $Q
#Nombre de Reynolds centerline
Re=`awk -v mu=$mu -v rho=$rho -v Ucent=$Ucent -v h=$h 'END {print rho*Ucent*h/mu}' $1.data`
echo "Re = " $Re
#Nombre de Reynolds selon la relation de Dean :
rto=`awk -v Re=$Re 'END {print 0.123*Re**(7/8)}' $0`
echo "Reynolds_tau(correlation de Dean) = " $rto
u_tau_correl=`awk -v rto=$rto -v mu=$mu -v rho=$rho -v h=$h 'END{print rto*mu/(rho*h)}' $0`
echo "u_tau_correl = " $u_tau_correl
u_tau=$u_tau_correl
[ "$2" != "" ] && u_tau=$u
echo "u_tau (calcul) = " $u_tau 
#u_tau=2.39
#u_tau=2.58
#Erreur sur le frottement
E_u_tau=`awk -v u_tau=$u_tau -v u_tau_correl=$u_tau_correl 'END{print (u_tau_correl-u_tau)/u_tau_correl}' $0`
echo "E_u_tau = " $E_u_tau

[ "$2" = "p" ] && awk -v u_tau=$u_tau_correl -v uc=$u_tau -v e_u_tau=$E_u_tau 'END {printf ("%.4g %.4g %.2g\n",u_tau,uc,e_u_tau*100)}' $0 > $1.erreur_u_tau
echo $u_tau_correl >$1.u_tau_th
[ "$2" = "p" ] && echo $u_tau >$1.u_tau_calc

vitu=`ls -rtC | grep Moyennes_spatiales_vitesse_rho_mu | tail -1`
echo $vitu
awk -v u_tau=$u_tau -v h=$h -v mu=$mu -v rho=$rho 'BEGIN {m=0} {if ($1=="#") { m=m+1;}; if (($1<=h)&&($1!=0) && (FNR>m+1)) {x=$1*u_tau/mu*rho; if (x>-10) {print ($1*u_tau/mu*rho,$2/u_tau,u_tau)}}}' $vitu > Umoy.dat


if [ -f $1_SONDE_VITMIL.son ]  
then
 extrait_coupe $1 SONDE_VITMIL
awk -v mu=$mu -v rho=$rho -v u_tau=$u_tau  '{if (FNR>1) {x=$1*u_tau/mu*rho; if (x>-10) {print ($1*u_tau/mu*rho,$2/u_tau,u_tau)}}}' $1_SONDE_VITMIL.coupe > $1_SONDE_VITMIL.adim.dat
else
cp Umoy.dat $1_SONDE_VITMIL.adim.dat 
fi
awk -v rto=$rto '($1<=rto) {print $1 " " 2.41*log($1)+5.32}' $1_SONDE_VITMIL.adim.dat > log.dat
#xmgr -log x $1_SONDE_VITMIL.adim.dat log.dat -noask
exit
