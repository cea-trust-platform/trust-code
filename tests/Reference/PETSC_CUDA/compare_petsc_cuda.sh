#!/bin/bash
# Script de comparaison de performance PETSC vs PETSC_GPU
# Analyse le -log_summary et compare les resultats du cas
compare_performance()
{
   echo "==============================="
   echo "$case test case ($cells cells):"
   echo "==============================="
   echo "                        CPU  vs GPU"
   echo "         ------------------------------------"
   echo "		        Time[s] Time[s] Ratio "
   awk '($1=="KSPSolve") {t[i++]=$4} END {gain=(t[1]!=0?t[0]/t[1]:0);printf("        KspSolve        %5.3f   %5.3f   *%2.1f\n",t[0],t[1],gain)}' $1 $2
   echo "         Routine %Time	MFlop/s	MFlop/s	Ratio "
   keys=`awk '(/Vec/ || /Mat/) && !/Assembly/ && ($3=="1.0") {print $1}' $1 $2 | sort -u`
   for key in $keys
   do
      grep "$key " $1 $2 | awk -v key=$key '// {if (NR==1) {pourcentage=$16;mflops1=$NF}; \
      						if (NR==2) {mflops2=$NF; gain=(mflops1!=0?mflops2/mflops1:0)}} \
						END {printf("%16s %d% \t%d \t%d \t*%2.1f\n",key,pourcentage,mflops1,mflops2,gain) }'
   done
}

#########
# BEGIN #
#########
run=1 && [ "$1" = -not_run ] && run=0
cas=$1 && [ "$1" = "" ] && exit -1
NPROCS=$2
[ ! -f $cas.data ] && exit -1
err=0
petsc_options="-log_summary -mat_no_inode -preload off -ksp_view"
line_solver=`grep -i solveur_pression $cas.data`
# List of solver
ksp="gcp"
pc="diag { }"
if [ $run = 1 ]
then
   for device in cpu gpu
   do
      seuil="seuil 1.e-7"
      [ "`grep "$seuil" $cas.data`" = "" ] && echo "Error in $0 !" && exit -1
      line="$ksp { precond $pc $seuil impr }"
      new_line_solver="solveur_pression petsc`[ $device = gpu ] && echo _gpu` $line"
      options=$options" `[ $device = gpu ] && echo -cusp_synchronize`" # cusp_synchronize is necessary to have correct timings with GPU on several routines, eg MatMult (KSPSolv seems fine though, but slower with -cusp_synchronize) 
      #core_per_node="-c 4" # To map one CPU on one GPU if 4CPU+1GPU on a socket
      echo $ECHO_OPTS "1,$ s?$line_solver?$new_line_solver?g\nw $device"_"$cas.data" | ed $cas.data 1>/dev/null 2>&1 || exit -1
      echo $ECHO_OPTS "Running test case on $device with $NPROCS core ($line): $device"_"$cas $options ... \c"
      trust $core_per_node $device"_"$cas $NPROCS $options 1>$device"_"$cas.out_err 2>&1 || exit -1
      if [ $? != 0 ]
      then
	 echo "Error, see `pwd`/$device"_"$cas.out_err"
	 exit -1
      else
	 awk '($1=="KSPSolve") {printf("OK KSPSolve in %5.3f s\n",$4)}' $device"_"$cas"_petsc.TU"
      fi      
   done   
   # Check the results: 
   compare_lata cpu_$cas.lata gpu_$cas.lata
   err=$?
fi
if [ "$err" = 0 ]
then
   case="TRUST"
   cells=`awk -F ", cols=" '/rows=/ {gsub("rows=","",$1);print $1;exit}' cpu_$cas.out_err`
   compare_performance cpu_$cas"_petsc.TU" gpu_$cas"_petsc.TU"
else
   echo "See gpu_$cas.out_err and cpu_$cas.out_err"
   exit -1
fi
exit 0


