#!/bin/bash
puissance=`awk '{print $5}' "test_pb_Diffusion_chaleur.out" | tail -n1`
valeurref=-100
ok=`echo $puissance $valeurref | $TRUST_Awk '{diff=$1-$2 ; if (diff<-0.011 || diff>0.011) print 1;else print 0}'`
err=0
[ $ok != 0 ] && echo $ECHO_OPTS "Error ($puissance!=$valeurref) when checking thermal power imposed" && err=1
exit $err
