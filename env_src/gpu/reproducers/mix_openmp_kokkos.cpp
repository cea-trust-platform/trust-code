#include <iostream>
#include <chrono>
#include <Kokkos_Core.hpp>
#include <Kokkos_DualView.hpp>
using namespace Kokkos;
int main() {
    Kokkos::initialize();
    {    
       #pragma omp target
       {;}
       const int size = 1260000;
       int data[size];
       std::cout << "Loops on " << size << " items:" << std::endl;
       // Kernel on HOST:
       auto start = std::chrono::high_resolution_clock::now();
       for (int i = 0; i < size; ++i)
           data[i] = i;
       auto end = std::chrono::high_resolution_clock::now();
       std::chrono::duration<double> diff = end - start;double SEC = diff.count();
       std::cout << "[Host a(i)=i] cpu: " << 1000*SEC << " ms \n";
       if (data[1]!=1)
       {
          std::cerr << "After Host (should be 1):" << data[1] << std::endl;
          exit(1);
       }
       
       // OpenMP target parallel loop
       #pragma omp target enter data map(alloc:data[0:size])
       #pragma omp target update to(data[0:size])
       start = std::chrono::high_resolution_clock::now();
       #pragma omp target teams distribute parallel for
       for (int i = 0; i < size; ++i)
           data[i] = i;
       end = std::chrono::high_resolution_clock::now();
       diff = end - start;SEC = diff.count();
       std::cout << "[OpenMP a(i)=i] gpu: " << 1000*SEC << " ms \n";
       #pragma omp target update from(data[0:size])
       if (data[1]!=1)
       {
          std::cout << "After OpenMP (should be 1):" << data[1] << std::endl;
          exit(1);
       }

       // Vue sur le device:
       Kokkos::View<int*> kokkosData("KokkosData", size);
       start = std::chrono::high_resolution_clock::now();
       Kokkos::parallel_for(size, KOKKOS_LAMBDA(const int i) {
          kokkosData(i) = i;
       });
       Kokkos::fence();
       end = std::chrono::high_resolution_clock::now();
       diff = end - start;SEC = diff.count();
       std::cout << "[Kokkos a(i)=i] gpu: " << 1000*SEC << " ms \n";
       Kokkos::View<int*>::HostMirror data_view = Kokkos::create_mirror_view(kokkosData);
       // Kokkos::deep_copy is used to bring data back to the host
       Kokkos::deep_copy(data_view, kokkosData);
       if (data_view[1]!=1)
       {
          std::cout << "After Kokkos (should be 1):" << data_view[1] << std::endl;
          exit(1);
       }
       
       // Now create a DualView:
       constexpr int arraySize = 10;
       double* tab_host = new double[arraySize];
       for(int i=0; i<arraySize; i++) 
           tab_host[i]=1.0;
       // Copie des donnees sur le device par OpenMP:
       #pragma omp target enter data map(to:tab_host[0:arraySize])
       
       DualView<double*>::t_host host_view = DualView<double*>::t_host(tab_host, arraySize);
       DualView<double*>::t_dev device_view; // Creation d'une vue avec les donnees du device:
       #pragma omp target data use_device_ptr(tab_host)
       {
          device_view = DualView<double*>::t_dev(tab_host, arraySize);
       }  
       // Creation de la DualView
       DualView<double*> dualView = DualView<double*>(device_view, host_view);
       // La vue est a jour sur le device:
       dualView.modify_device();
       
       auto h_data = dualView.view_host();
       if (h_data[0]!=1)
       {
          std::cout << "Before loop (should be 1):" << h_data[0] << std::endl;
          exit(1);
       }
       // Loop on device
       auto d_data = dualView.view_device();
       Kokkos::parallel_for(arraySize, KOKKOS_LAMBDA(const int i) {
               d_data(i) *= 2;
               //printf("Kokkos: %e\n",d_data(i));
       });         
       // Back to host
       dualView.sync_host();
       if (h_data[0]!=2)
       {
          std::cout << "Before loop (should be 2):" << h_data[0] << std::endl;
          exit(1);
       }   
       // Destruction des donnees sur le device
       #pragma omp target exit data map(from:tab_host[0:arraySize])
    }     
    Kokkos::finalize();
    return 0;
}


