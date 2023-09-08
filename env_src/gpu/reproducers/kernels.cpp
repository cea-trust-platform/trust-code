#include <iostream>
#include <omp.h>
#include <chrono>
#include <cuda_runtime.h>
#include <cstdlib>
#include <algorithm> 

int main() {
    #pragma omp target
    {;}
    const int nb_elem = 2600000;
    const int nb_faces = 2 * nb_elem;
    const int samples = 10;
    // Initialize data on the host
    double *a = new double[nb_elem]();
    for (int i = 0; i < nb_elem; i++)
        a[i] = 0;
    double *b = new double[nb_elem]();
    std::generate(b, b + nb_elem, []() {
        return std::rand();
    });
    double *f = new double[nb_faces]();
    std::generate(f, f + nb_faces, []() {
        return std::rand();
    });
    double *resu = new double[nb_faces]();
    std::generate(resu, resu + nb_faces, []() {
        return std::rand();
    });
    int *ef = new int[4*nb_elem]();
    std::generate(ef, ef + 4*nb_elem, []() {
        return std::rand()%nb_faces;
    });    
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
          FLOPS += 2 * nb_elem;	  
          auto end = std::chrono::high_resolution_clock::now();
          std::chrono::duration<double> diff = end - start;
          SEC += diff.count();
       }
       SEC/=samples;
       FLOPS/=samples;
       std::cout << "[Kernel  a+=b] gpu: " << gpu << " Mean time: " << 1000*SEC << " ms " << 0.1*int(0.01*0.001*0.001*FLOPS/SEC) << " GFLOPS\n";
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
          FLOPS += 8 * nb_elem;
          auto end = std::chrono::high_resolution_clock::now();
          std::chrono::duration<double> diff = end - start;
          SEC += diff.count();
       }
       SEC/=samples;
       FLOPS/=samples;
       std::cout << "[Kernel OpDiv] gpu: " << gpu << " Mean time: " << 1000*SEC << " ms " << 0.1*int(0.01*0.001*0.001*FLOPS/SEC) << " GFLOPS\n";
    }
    return 0;
}

