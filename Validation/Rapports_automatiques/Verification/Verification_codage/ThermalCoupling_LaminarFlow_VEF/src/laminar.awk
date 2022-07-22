#!/bin/bash
export LC_NUMERIC="C"

# maximal velocity
Umax=30.


##########################################################

jdd=$1


# fluid dimensions
X=`grep "LONGUEURS" ./"$jdd".data| awk '{print $2}' | head -1`
Y=`grep "LONGUEURS" ./"$jdd".data| awk '{print $3}' | head -1`
Zf=`grep "LONGUEURS" ./"$jdd".data| awk '{print $4}' | head -1`

# solid dimensions
Zs=`grep "longueurs" ./"$jdd".data| awk '{print $4}' | head -1`

# fluid physical properties 
rho=`grep "RHO Champ_Uniforme" $jdd.data| awk '{print $4}' | head -1`
mu=`grep "MU Champ_Uniforme" $jdd.data| awk '{print $4}' | head -1`
lambda_f=`grep "LAMBDA Champ_Uniforme" $jdd.data| awk '{print $4}' | head -1`
cp_f=`grep "CP Champ_Uniforme" $jdd.data| awk '{print $4}' | head -1`

echo $rho > rho_f.dat
echo $mu > mu_f.dat
echo $lambda_f > lambda_f.dat
echo $cp_f > cp_f.dat

Pr=`awk "BEGIN { print $mu*$cp_f/$lambda_f }"`
echo $Pr > Prandtl.dat

#solid properties
rho_s=`grep "rho champ_uniforme" $jdd.data| awk '{print $4}' | head -1`
cp_s=`grep "cp champ_uniforme" $jdd.data| awk '{print $4}' | head -1`
lambda_s=`grep "lambda champ_uniforme" $jdd.data| awk '{print $4}' | head -1`

echo $rho_s > rho_s.dat
echo $lambda_s > lambda_s.dat
echo $cp_s > cp_s.dat

# thermal conditions
q=`grep "puissance_thermique" $jdd.data| awk '{print $6}' | head -1`
T0=`grep "paroi_temperature_imposee" $jdd.data| awk '{print $5}' | head -1`

##########################################################

h=$Zs
H=`awk "BEGIN { print $Zs+$Zf }"`

S=`awk "BEGIN { print $X*$Y }"`

echo $X $Y $Zs h=$h > solide_dim.dat
echo $X $Y $Zf H=$H > fluide_dim.dat


# calculation results
Tmax_calc=`tail -1 ./"$jdd"_TEMP_MAX.son | awk '{print $2}' | head -1`
Tcs_calc=`tail -1 ./"$jdd"_TEMP_CONTACT_SOLIDE.son | awk '{print $2}' | head -1`
Tcf_calc=`tail -1 ./"$jdd"_TEMP_CONTACT_FLUIDE.son | awk '{print $2}' | head -1`

# delta T inside the solid
DeltaT=`awk "BEGIN { print $q*$h*$h/(2*$lambda_s) }"`
DeltaT_calc=`awk "BEGIN { print $Tmax_calc-$Tcs_calc }"`
diff_delta=`awk "BEGIN { print -100*($DeltaT-$DeltaT_calc)/$DeltaT }"`
echo $DeltaT $DeltaT_calc $diff_delta > delta.dat

# thermal flux
flux_theo=`awk "BEGIN { print $q*$h }"`

flux_haut=`tail -1 ./"$jdd"_pb1_Diffusion_chaleur.out | awk -v S=$S '{print $2/(S)}' | head -1`
flux_contact=`tail -1 ./"$jdd"_pb1_Diffusion_chaleur.out | awk -v S=$S '{print $7/(S)}' | head -1`
echo $flux_theo $flux_contact $flux_haut > flux.dat

# profils
echo 1 | awk -v Umax=$Umax -v q=$q -v H=$H -v h=$h -v lambda_s=$lambda_s -v lambda_f=$lambda_f -v T0=$T0 -v Tmax_calc=$Tmax_calc -v Tcs_calc=$Tcs_calc -v Tcf_calc=$Tcf_calc '
	
	## velocity profile
	function U(y)
		{
		toto=Umax*(1-(4/(H-h)^2)*(y-0.5*(H+h))^2);
		return toto;
		}	
	
	## temperature solide analytique
	function T_s(y)
		{
		toto=q*((0.5*h*h/lambda_s)+(h*H/lambda_f)+(-h*h/lambda_f)+(-0.5*y*y/lambda_s))+T0;
		return toto;
		}
	
	## temperature fluide analytique
	function T_f(y)
		{
		toto=q*h/lambda_f*(H-y)+T0;
		return toto;
		}	
		
	{
	for (z=h;z<(H+0.01);z=z+0.1) {printf "%.4f %.4f\n",z,U(z) > "profil_velocity.dat"} 
	
	for(z=0;z<(h+0.01);z=z+0.05) {printf "%.4f %.4f\n",z,T_s(z) > "profil_temp_solide.dat"}
	
	for(z=h;z<(H+0.01);z=z+0.1) {printf "%.4f %.4f\n",z,T_f(z) > "profil_temp_fluide.dat"}
	
	Tmax=T_s(0);
	Tcs=T_s(h);
	Tcf=T_f(h);
	Tf_H=T_f(H);
	
	diff_tmax=-100*(Tmax-Tmax_calc)/Tmax;
	diff_tcs=-100*(Tcs-Tcs_calc)/Tcs;
	diff_tcf=-100*(Tcf-Tcf_calc)/Tcf;
	}
	
	{
	{printf "Tmax %.4f %.4f %.4f \n", Tmax,Tmax_calc,diff_tmax > "Tmax.dat"}
	{printf "Tc %.4f %.4f %.4f \n", Tcs,Tcs_calc,diff_tcs > "Tcs.dat"}
	{printf "Tc %.4f %.4f %.4f \n", Tcs,Tcf_calc,diff_tcf > "Tcf.dat"}
	
	}' 
exit





