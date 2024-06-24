#include <iostream>
#include <chrono>
#include <cstdlib>
#include <algorithm> 
#include <Kokkos_Core.hpp>
#include <Kokkos_DualView.hpp>

template <typename _TYPE_>
extern _TYPE_* addrOnDevice(_TYPE_* tab);
extern void compute_omp(double* a, double* b, double* f, double* resu, int* ef, int nb_elem, int nb_faces, int samples);

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
    device_view = t_dev(addrOnDevice(tab), size);
    DualView<T*, layout, trait> dualView = DualView<T*, layout, trait>(device_view, host_view);
    dualView.modify_device();
    return dualView.view_device();
}

int main() {
    
    Kokkos::initialize();
    {  
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

    // Calcul OpenMP
    compute_omp(a, b, f, resu, ef, nb_elem, nb_faces, samples);

    // Calcul Kokkos
    const auto a_v = create_view(a, nb_elem);
    const auto ef_v = create_view(ef, 4*nb_elem);
    const auto f_v = create_view(f, nb_faces);
    auto resu_v = create_view(resu, nb_faces);
/*
    Kokkos::View<double*> a_v("a_v", nb_elem);
    Kokkos::View<int*> ef_v("ef_v", 4*nb_elem);
    Kokkos::View<double*> f_v("f_v", nb_faces);
    Kokkos::View<double*> resu_v("resu_v", nb_faces);

    // Copy data to Kokkos views
    Kokkos::deep_copy(a_v, Kokkos::View<double*, Kokkos::HostSpace>(a, nb_elem));
    Kokkos::deep_copy(ef_v, Kokkos::View<int*, Kokkos::HostSpace>(ef, 4*nb_elem));
    Kokkos::deep_copy(f_v, Kokkos::View<double*, Kokkos::HostSpace>(f, nb_faces));
    Kokkos::deep_copy(resu_v, Kokkos::View<double*, Kokkos::HostSpace>(resu, nb_faces)); */

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
  
    }
    Kokkos::finalize();
    return 0;
}


