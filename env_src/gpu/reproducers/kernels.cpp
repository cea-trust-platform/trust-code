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
    
    Kokkos::initialize();
    {  
    const auto a_v = create_view(a, nb_elem);
    const auto ef_v = create_view(ef, 4*nb_elem);
    const auto f_v = create_view(f, nb_faces);
    auto resu_v = create_view(resu, nb_faces);
    int FLOPS=0;   
    double SEC=0; 
    for (int sample=0;sample<samples;sample++)
    {
          auto start = std::chrono::high_resolution_clock::now();
          Kokkos::parallel_for(nb_elem, KOKKOS_LAMBDA(const int elem) {
              double pre = a_v[elem];
              for (int j=0; j<4; j++)
              {
                  int face = ef_v[4*elem+j];
                  Kokkos::atomic_add(&resu_v[face], pre * f_v[face]);
              }
          });
          Kokkos::fence();
          auto end = std::chrono::high_resolution_clock::now();
          std::chrono::duration<double> diff = end - start;
          FLOPS += 8 * nb_elem;
          SEC += diff.count();
    }
    FLOPS/=samples;
    SEC/=samples;
    std::cout << "[Kernel OpDiv] Kokkos" << " Mean time: " << 1000*SEC << " ms " << 0.1*int(0.01*0.001*0.001*FLOPS/SEC) << " GFLOPS\n";
    
    Kokkos::View<double*> a_view("a_v", nb_elem);
    Kokkos::View<int*> ef_view("ef_v", 4*nb_elem);
    Kokkos::View<double*> f_view("f_v", nb_faces);
    Kokkos::View<double*> resu_view("resu_v", nb_faces);

    // Copy data to Kokkos views
    Kokkos::deep_copy(a_view, Kokkos::View<double*, Kokkos::HostSpace>(a, nb_elem));
    Kokkos::deep_copy(ef_view, Kokkos::View<int*, Kokkos::HostSpace>(ef, 4*nb_elem));
    Kokkos::deep_copy(f_view, Kokkos::View<double*, Kokkos::HostSpace>(f, nb_faces));
    Kokkos::deep_copy(resu_view, Kokkos::View<double*, Kokkos::HostSpace>(resu, nb_faces));
    
    FLOPS=0;   
    SEC=0; 
    for (int sample=0;sample<samples;sample++)
    {
          auto start = std::chrono::high_resolution_clock::now();
          Kokkos::parallel_for(nb_elem, KOKKOS_LAMBDA(const int elem) {
              double pre = a_view(elem);
              for (int j=0; j<4; j++)
              {
                  int face = ef_view(4*elem+j);
                  Kokkos::atomic_add(&resu_view(face), pre * f_view(face));
              }
          });
          Kokkos::fence();
          auto end = std::chrono::high_resolution_clock::now();
          std::chrono::duration<double> diff = end - start;
          FLOPS += 8 * nb_elem;
          SEC += diff.count();
    }
    FLOPS/=samples;
    SEC/=samples;
    std::cout << "[Kernel OpDiv] Kokkos2" << " Mean time: " << 1000*SEC << " ms " << 0.1*int(0.01*0.001*0.001*FLOPS/SEC) << " GFLOPS\n";
    }
    Kokkos::finalize();
    return 0;
}

