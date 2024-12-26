#include <iostream>
#include <chrono>
#include <cstdlib>
#include <algorithm> 
#include <Kokkos_Core.hpp>
#include <Kokkos_DualView.hpp>
using namespace Kokkos;

using layout = Kokkos::LayoutRight;
using trait = Kokkos::MemoryTraits<Kokkos::RandomAccess>;

int main() {
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

    Kokkos::initialize();
    {  
    
    Kokkos::View<double*> a_view("a_v", nb_elem);
    Kokkos::View<int*> ef_view("ef_v", 4*nb_elem);
    Kokkos::View<double*> f_view("f_v", nb_faces);
    Kokkos::View<double*> resu_view("resu_v", nb_faces);

    // Copy data to Kokkos views
    Kokkos::deep_copy(a_view, Kokkos::View<double*, Kokkos::HostSpace>(a, nb_elem));
    Kokkos::deep_copy(ef_view, Kokkos::View<int*, Kokkos::HostSpace>(ef, 4*nb_elem));
    Kokkos::deep_copy(f_view, Kokkos::View<double*, Kokkos::HostSpace>(f, nb_faces));
    Kokkos::deep_copy(resu_view, Kokkos::View<double*, Kokkos::HostSpace>(resu, nb_faces));
    
    int FLOPS=0;
    double SEC=0;
    for (int sample=0;sample<samples;sample++)
    {
        auto start = std::chrono::high_resolution_clock::now();
        for (int elem = 0; elem < nb_elem; elem++)
        {
            double pre = a[elem];
            for (int j=0; j<4; j++)
            {
                int face = ef[4*elem+j];
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
    std::cout << "[Kernel OpDiv] CPU    Mean time: " << 1000*SEC << " ms " << 0.1*int(0.01*0.001*0.001*FLOPS/SEC) << " GFLOPS\n";

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
    std::cout << "[Kernel OpDiv] Kokkos Mean time: " << 1000*SEC << " ms " << 0.1*int(0.01*0.001*0.001*FLOPS/SEC) << " GFLOPS\n";
    }
    Kokkos::finalize();
    return 0;
}

