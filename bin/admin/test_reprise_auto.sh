#!/bin/bash
log=test_reprise_auto.log
source $TRUST_ROOT/env_TRUST.sh 1>$log 2>&1
trust -copy Obstacle 1>$log 2>&1
cd Obstacle
trust -clean 1>$log 2>&1
make_PAR.data Obstacle 1>$log 2>&1
echo $ECHO_OPTS "1,$ s?tmax 5.0?tmax 100.0?g\nw" | ed PAR_Obstacle.data 1>>$log 2>&1
trust -create_sub_file PAR_Obstacle 2 1>>$log 2>&1
reprise_auto -test 1>>$log 2>&1
sleep 1800 # Attente de 30mn que le calcul/reprise se finisse
cd test_reprise_auto 1>>$log 2>&1
if [ "`grep 'final time reached' PAR_Obstacle.err 2>/dev/null`" = "" ]
then
   echo "Script reprise_auto KO"
   echo "final time reached no detecte in PAR_Obstacle.err:" >> $log
   cat PAR_Obstacle.err >> $log
   cat $log | mail_ -s\"[mise_a_jour_TRUST_arch] KO reprise_auto script on $HOST_BUILD\" $TRUST_MAIL
else
   echo "Script reprise_auto OK"
fi

