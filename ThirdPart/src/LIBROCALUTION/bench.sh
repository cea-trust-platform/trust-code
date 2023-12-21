gunzip -c VDF_10x10x10.mtx.gz > VDF_10x10x10.mtx
cas=VDF_10x10x10.mtx && liste="cg_mpi cg-uaamg_mpi cg-amg_mpi cg-rsamg_mpi cg-saamg_mpi"
mtx=../../../../../MATRIX_MARKET/trust_P0_VEF/aij_2592000.mtx
if [ -f $mtx ]
then
   cas=$mtx  && liste="cg_mpi cg-uaamg_mpi            cg-rsamg_mpi cg-saamg_mpi " # cg-amg_mpi encore bugge
fi
# Gros progres sur 3.1.0: il semble que que UA, SA, RS soient enfin paralleles!
# Donc il va falloir modifier le codage de solv_rocALUTION pour en beneficier... Et ne plus faire du Block Jacobi
# Le plus rapide semble etre SA
# Verifier avant que cela build sur ROCm 5.2.3 et que ROCm 6.0.0 pas necessaire
for exec in $liste
do
   echo "======================="
   for np in 1 2
   do
      echo mpirun -np $np $exec $cas
      mpirun -np $np $TRUST_ROOT/lib/src/LIBROCALUTION/clients/staging/$exec $cas 2>&1 | grep -e reached -e sec
   done   
done | tee bench.log  
