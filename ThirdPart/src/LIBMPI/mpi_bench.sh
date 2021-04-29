#!/bin/bash
# Utilisation des micro benchs OSU
echo "Usage: $0 nb_proc"
nb_proc=$1 && [ "$nb_proc" = "" ] && exit 0
if [ ! -f osu-micro-benchmarks-5.7/mpi/collective/osu_allreduce ]
then
   wget http://mvapich.cse.ohio-state.edu/download/mvapich/osu-micro-benchmarks-5.7.tar.gz
   tar xfz osu-micro-benchmarks-5.7.tar.gz
   cd osu-micro-benchmarks-5.7
   ./configure CC=`which $TRUST_cc` CXX=`which $TRUST_CC` || exit -1
   $TRUST_MAKE || exit -1
else
   cd osu-micro-benchmarks-5.7/mpi/collective
fi
exec="`pwd`/osu_allreduce -m 8:8 -i 100 -f"
# Bench de mp_sum(double):
n=2
while [ $n -lt $nb_proc ]
do
   trust dummy $n
   n=`echo 2*$n | bc -l`
done
