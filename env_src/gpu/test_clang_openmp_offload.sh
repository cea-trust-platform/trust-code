#!/bin/bash
echo "
#include <cstdio>

int main(){
  int N = 1e7;
  double *x = new double[N];
  double *y = new double[N];

  #pragma omp target teams distribute parallel for map(to: x[0:N]) map(from: y[0:N])
  for(int i = 0; i < N; i++){
    for(int j = 0; j < 10000; j++){
      y[i] += 3*x[i];
    }
  }

  printf(\"%g\n\", y[5]);

  delete [] x;
  delete [] y;
}" > dp.cpp

#clang++ -o run_gpu -fopenmp -fopenmp-targets=nvptx64 /export/home/catA/pl254994/clang/lib/libomptarget.so -O3 dp.cpp || exit -1
clang++ -o run_gpu -fopenmp -fopenmp-targets=nvptx64 -O3 dp.cpp || exit -1
echo "GPU DP:" && time ./run_gpu || exit -1
clang++ -o run_gpu -fopenmp                          -O3 dp.cpp || exit -1
echo "CPU DP:" && time ./run_gpu || exit -1
exit 0

