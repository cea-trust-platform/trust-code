#!/bin/bash
echo usage archive.tar [ -extension_log ext ] path_to_run user@host
[ $# -lt  3 ] && echo "wrong number arguments " && exit 1


set -u

ext=""
if [ "$1" = "-extension" ]
then
    shift
    ext=$1
    shift
fi


archive=$1
path_to_run=$2
machine_cible=$3



    
base=`basename $archive .tar`
# machine 	accesbile

log_time=${base}_%_${machine_cible}${ext}_%_time.log
echo Info_global date_debut `date '+%d/%m %H:%M'` >${log_time}


ssh -o "StrictHostKeyChecking no" ${machine_cible} 'echo `hostname`' 
[ $? !=  0 ] && echo ${machine_cible} inaccessible ? >> ${log_time} && echo Info_global prepare KO >> ${log_time} && exit 1

# path_to_run existe
ssh -o "StrictHostKeyChecking no" ${machine_cible} "[ ! -d ${path_to_run} ] && mkdir -p  ${path_to_run} ;[ ! -d ${path_to_run} ] && exit 2; exit 0"
      
[ $? !=  0 ] && echo ${path_to_run} sur ${machine_cible} inaccessible ? >> ${log_time} &&  echo Info_global prepare KO >> ${log_time} && exit 2

# la date sera reecite apres l attente, mais on voit ainsi les process en attente....
#echo Info_global date_debut `date` >${base}_${machine_cible}${ext}_time.log

#

#Encours=`echo En_cours_${base}_${machine_cible}_${path_to_run}| sed "s?\/?_%_?g"`
#while [ -f $Encours ] 
#  do 
#  echo Waiting `cat $Encours`
#  sleep 10
#  [ -f stop ] && exit 4
# done


# echo $* >  $Encours

scp `dirname $0`/list_pid_et_fils.sh ${archive} ${machine_cible}:${path_to_run}/

for phase in prepare configure make make_check make_install
  do
  log_phase=${base}_%_${machine_cible}${ext}_%_$phase.log
  echo $base $phase on $machine_cible see ${log_phase}
  	
  if [ $phase = prepare ]
      then
      detar="tar xf $base.tar;"    
  else
      detar=""
  fi

  option=""
  if [[ $machine_cible = *"jean-zay"* ]] && [ $phase !=  make_check ]
  then
      option="srun -p compil -A fej@cpu -t 10:00:00 -c 10 --hint=nomultithread "
  fi
  
  #ssh -n ${machine_cible} "cd ${path_to_run};$detar chmod +x $phase.sh;${path_to_run}/englobe.sh ./$phase.sh $* 2>&1  " > ${log_phase}
  ssh -o "StrictHostKeyChecking no" -n ${machine_cible} "cd ${path_to_run};$detar chmod +x $phase.sh; $option ./$phase.sh $* 2>&1  " > ${log_phase}
  status=$? && [ "`grep "./$phase.sh: No such file or directory" ${log_phase}`" != "" ] && status=-1
  echo Info date_fin_$phase `date '+%d/%m %H:%M'` >>${log_time}
  [ $status -ne 0 ] && echo Info $phase KO on $machine_cible  && tail  ${log_phase} && echo Info_global $phase KO >>  ${log_phase} && echo "Info_global date_fin `date '+%d/%m %H:%M'`" >> ${log_time} && exit 4
  echo Info_global $phase OK >> ${log_phase}	
done
echo "Info_global date_fin `date '+%d/%m %H:%M'`" >> ${log_time}
echo $base OK sur $machine_cible
# rm -f $Encours
exit 0

