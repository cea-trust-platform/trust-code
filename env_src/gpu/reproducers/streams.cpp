#include <iostream>
#include <omp.h>
#include <chrono>
#include <cstdlib>
#include <algorithm> 
#include <Kokkos_Core.hpp>
#include <Kokkos_DualView.hpp>
using namespace Kokkos;

using layout = Kokkos::LayoutRight;
using trait = Kokkos::MemoryTraits<Kokkos::RandomAccess>;
template<typename T>
typename DualView<T*, layout, trait>::t_dev create_view(T* tab, int size)
{
    using t_host = typename DualView<T*, layout, trait>::t_host;
    using t_dev = typename DualView<T*, layout, trait>::t_dev;
    
    t_host host_view = t_host(tab, size);
    t_dev device_view; // Creation d'une vue avec les donnees du device:
    #pragma omp target data use_device_ptr(tab)
    {
       device_view = t_dev(tab, size);
    }  
    DualView<T*, layout, trait> dualView = DualView<T*, layout, trait>(device_view, host_view);
    dualView.modify_device();
    return dualView.view_device();
}

int main() {
    #pragma omp target
    {;}
    const int nb_elem = 20000;
    const int nb_faces = 2 * nb_elem;
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
        return 0;
    });
    double *resu1 = new double[nb_faces]();
    std::generate(resu1, resu1 + nb_faces, []() {
        return 0;
    });
    double *resu2 = new double[nb_faces]();
    std::generate(resu2, resu2 + nb_faces, []() {
        return 0;
    });
    int *ef = new int[4*nb_elem]();
    std::generate(ef, ef + 4*nb_elem, []() {
        return std::rand()%nb_faces;
    });    
    #pragma omp target enter data map(alloc:a[0:nb_elem])
    #pragma omp target enter data map(alloc:b[0:nb_elem])
    #pragma omp target enter data map(alloc:f[0:nb_faces])
    #pragma omp target enter data map(alloc:resu[0:nb_faces])
    #pragma omp target enter data map(alloc:resu1[0:nb_faces])
    #pragma omp target enter data map(alloc:resu2[0:nb_faces])
    #pragma omp target enter data map(alloc:ef[0:4*nb_elem])
    #pragma omp target update to(a[0:nb_elem])
    #pragma omp target update to(b[0:nb_elem])
    #pragma omp target update to(f[0:nb_faces])
    #pragma omp target update to(resu[0:nb_faces])
    #pragma omp target update to(resu1[0:nb_faces])
    #pragma omp target update to(resu2[0:nb_faces])
    #pragma omp target update to(ef[0:4*nb_elem])
    
    Kokkos::initialize();
    {  
    const auto a_v = create_view(a, nb_elem);
    const auto ef_v = create_view(ef, 4*nb_elem);
    const auto f_v = create_view(f, nb_faces);
    auto resu_v = create_view(resu, nb_faces);
    double SEC=0; 
    // Define kernel:
    /*
    auto L = KOKKOS_LAMBDA(const int elem) {
         double pre = a_v[elem];
         for (int j=0; j<4; j++)
         {
              int face = ef_v[4*elem+j];
              Kokkos::atomic_add(&resu_v[face], pre * f_v[face]);
         }
    }; */
    int M = 100; //10;
    Kokkos::View<double*> slow("A",nb_elem*M);
    Kokkos::View<double**> fast("A",nb_elem,M);
    // L'asynchronisme ne permet pas toujours de gagner du temps !!!
    // Depends la taille de la boucle, plus elle est petite mieux cela marche
    // Depend aussi du layout, si pas bon, cela degrade !!!
    auto L = KOKKOS_LAMBDA(const int i) {
          for(int j=0;j<M;j++) {
              // Why ?
              // slow(i*M+j)+=1.0; // 283-360 ms Layout !!!!
              fast(i,j)+=j; // 145-73  ms
       }
     };
    auto start = std::chrono::high_resolution_clock::now();
    Kokkos::parallel_for("Kokkos1", nb_elem, L);
    Kokkos::fence();
    Kokkos::parallel_for("Kokkos2", nb_elem, L);
    Kokkos::fence();
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> diff = end - start;
    SEC += diff.count();
    std::cout << "[Kernel OpDiv] 2 Kokkos Sync" << " Mean time: " << 1000*SEC << " ms\n";    

    using device = Kokkos::DefaultExecutionSpace;
    device device_stream1, device_stream2, device_stream3;
    // ToDo: more generic
#define TRUST_USE_CUDA
#ifdef TRUST_USE_CUDA  
    cudaStream_t stream1, stream2, stream3;
    cudaStreamCreate(&stream1);
    cudaStreamCreate(&stream2);
    cudaStreamCreate(&stream3);
#else
    hipStream_t stream1, stream2;
    hipStreamCreate(&stream1);
    hipStreamCreate(&stream2);
#endif
    device_stream1 = stream1;
    device_stream2 = stream2;

    auto resu_v1 = create_view(resu1, nb_faces);
    auto resu_v2 = create_view(resu2, nb_faces);
    // Define kernels:
    auto L1 = KOKKOS_LAMBDA(const int elem) {
       double pre = a_v[elem];
       for (int j=0; j<4; j++)
       {
          int face = ef_v[4*elem+j];
          Kokkos::atomic_add(&resu_v1[face], pre * f_v[face]);
       }
    };
    auto L2 = KOKKOS_LAMBDA(const int elem) {
       double pre = a_v[elem];
       for (int j=0; j<4; j++)
       {
          int face = ef_v[4*elem+j];
          Kokkos::atomic_add(&resu_v2[face], pre * f_v[face]);
       }
    };
    SEC=0; 
    start = std::chrono::high_resolution_clock::now();
    {
       // Emulate first operator 
       Kokkos::parallel_for("Kokkos1",RangePolicy<>(device_stream1, 0, nb_elem), L);
    }
    {       
       // Emulate second operator
       Kokkos::parallel_for("Kokkos2",RangePolicy<>(device_stream2, 0, nb_elem), L);
    }
    Kokkos::fence();
    end = std::chrono::high_resolution_clock::now();
    diff = end - start;
    SEC += diff.count();
    std::cout << "[Kernel OpDiv] 2 Kokkos Async" << " Mean time: " << 1000*SEC << " ms\n";    

    // Clean up CUDA streams
#ifdef TRUST_USE_CUDA
    cudaStreamDestroy(stream1);
    cudaStreamDestroy(stream2);
    cudaStreamDestroy(stream3);
#else
    hipStreamDestroy(stream1);
    hipStreamDestroy(stream2);        
#endif    
    }
    Kokkos::finalize();
    return 0;
} 

