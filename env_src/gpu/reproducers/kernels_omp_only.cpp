#include <iostream>
#include <omp.h>
#include <chrono>
#include <cstdlib>
#include <algorithm>

template <typename _TYPE_>
_TYPE_* addrOnDevice(_TYPE_* tab)
{
  _TYPE_ *device_ptr = nullptr;
  _TYPE_ *ptr = tab;
  #pragma omp target data use_device_ptr(ptr)
  {
    device_ptr = ptr;
  }
  return device_ptr;
}
template int* addrOnDevice<int>(int*);
template double* addrOnDevice<double>(double*);

void compute_omp(double* a, double* b, double* f, double* resu, int* ef, int nb_elem, int nb_faces, int samples) {
    #pragma omp target
    {;}
   
    #pragma omp target enter data map(alloc:a[0:nb_elem])
    #pragma omp target enter data map(alloc:b[0:nb_elem])
    #pragma omp target enter data map(alloc:f[0:nb_faces])
    #pragma omp target enter data map(alloc:resu[0:nb_faces])
    #pragma omp target enter data map(alloc:ef[0:4*nb_elem])
    #pragma omp target update to(a[0:nb_elem])
    #pragma omp target update to(b[0:nb_elem])
    #pragma omp target update to(f[0:nb_faces])
    #pragma omp target update to(resu[0:nb_faces])
    #pragma omp target update to(ef[0:4*nb_elem])

    // Kernel simple type TRUSTArray::operator+=(const TRUSTArray& arr);
    for (int gpu=0;gpu<2;gpu++)
    {
       int FLOPS=0;   
       double SEC=0; 
       for (int sample=0;sample<samples;sample++)
       {
          auto start = std::chrono::high_resolution_clock::now();
          #pragma omp target teams distribute parallel for if (gpu)
          for (int elem = 0; elem < nb_elem; elem++)
             a[elem] += b[elem];
          auto end = std::chrono::high_resolution_clock::now();
          std::chrono::duration<double> diff = end - start;
          FLOPS += 2 * nb_elem;	  
          SEC += diff.count();
       }
       FLOPS/=samples;
       SEC/=samples;
       std::cout << "[Kernel  a+=b] " << (gpu?"OMPT  ":"CPU   ") << " Mean time: " << 1000*SEC << " ms " << 0.1*int(0.01*0.001*0.001*FLOPS/SEC) << " GFLOPS\n";
    }
    // Kernel simple type Op_Div avec tableaux face/elem
    for (int gpu=0;gpu<2;gpu++)
    {
       int FLOPS=0;   
       double SEC=0; 
       for (int sample=0;sample<samples;sample++)
       {
          auto start = std::chrono::high_resolution_clock::now();
          #pragma omp target teams distribute parallel for if (gpu)
          for (int elem = 0; elem < nb_elem; elem++)
          {
             double pre = a[elem];
             for (int j=0; j<4; j++)
             {
               int face = ef[4*elem+j];
               #pragma omp atomic
               resu[face] += pre * f[face];
             }
          }
          auto end = std::chrono::high_resolution_clock::now();
          std::chrono::duration<double> diff = end - start;
          FLOPS += 8 * nb_elem;
          SEC += diff.count();
       }
       FLOPS/=samples;
       SEC/=samples;
       std::cout << "[Kernel OpDiv] " << (gpu?"OMPT  ":"CPU   ") << " Mean time: " << 1000*SEC << " ms " << 0.1*int(0.01*0.001*0.001*FLOPS/SEC) << " GFLOPS\n";
    }
}

