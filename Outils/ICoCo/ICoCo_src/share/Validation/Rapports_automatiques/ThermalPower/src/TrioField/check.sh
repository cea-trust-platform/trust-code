#!/bin/bash
puissance=`awk '{print $5}' "test_pb_Diffusion_chaleur.out" | tail -n1`
valeurref=-100
ok=`awk '{print $1-$2}' <<<"$puissance $valeurref"`
err=0
[ $ok != 0 ] && echo $ECHO_OPTS "Error ($puissance!=$valeurref) when checking thermal power imposed" && err=1
exit $err
