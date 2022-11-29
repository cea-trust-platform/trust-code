#!/bin/bash
# Taskset pour affinite
# perf stat -r pour mesure avec intervalle de precision, cool
cmd="perf stat -r 5 taskset -c 0 $exec cpu_3D"
echo $cmd
eval $cmd 1>cpu_3D.out_err 2>&1
cat cpu_3D.out_err
