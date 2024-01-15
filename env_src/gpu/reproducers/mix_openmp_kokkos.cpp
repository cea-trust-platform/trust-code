#include <iostream>
#include <Kokkos_Core.hpp>
#include <Kokkos_DualView.hpp>
using namespace Kokkos;
int main() {
    Kokkos::initialize();
    {
       const int size = 1000;
       int data[size];
       data[1]=-1;

       // OpenMP target parallel loop
       #pragma omp target teams distribute parallel for map(tofrom: data[0:size])
       for (int i = 0; i < size; ++i) {
           data[i] = i;
       }
       std::cout << "After OpenMP (should be 1):" << data[1] << std::endl;

       Kokkos::View<int*> data_view(data, size);
       Kokkos::View<int*, Kokkos::CudaSpace> kokkosData("KokkosData", size);

       Kokkos::parallel_for(size, KOKKOS_LAMBDA(const int i) {
               kokkosData(i) = i * 2;
       });

       // Kokkos::deep_copy is used to bring data back to the host
       Kokkos::deep_copy(data_view, kokkosData);
       std::cout << "Kokkos View on ";

       if (std::is_same<Kokkos::DefaultExecutionSpace, Kokkos::Cuda>::value) {
           std::cout << "GPU\n";
       } else {
           std::cout << "Host\n";
       }
       std::cout << "After Kokkos (should be 2):" << data[1] << std::endl;
    
       // Now create a DualView:
       constexpr int arraySize = 10;
       double* tab_host = new double[arraySize];
       for(int i=0; i<arraySize; i++) tab_host[i]=1.0;
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
       std::cout << "Before loop (should be 1):" << h_data[0] << std::endl;

       // Loop on device
       auto d_data = dualView.view_device();
       Kokkos::parallel_for(arraySize, KOKKOS_LAMBDA(const int i) {
               d_data(i) *= 2;
               //printf("Kokkos: %e\n",d_data(i));
       });         
       // Back to host
       dualView.sync_host();
       std::cout << "Before loop (should be 2):" << h_data[0] << std::endl;
       
       // Destruction des donnees sur le device
       #pragma omp target exit data map(from:tab_host[0:arraySize])
    }     
    Kokkos::finalize();
    return 0;
}


