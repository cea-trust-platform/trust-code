#!/bin/bash
#Check_solver.sh -all -not_lml -no_ranking $1
Check_solver.sh -all -not_lml $1
[ -f rank.ref_$HOST ] && sdiff rank.ref_$HOST rank
