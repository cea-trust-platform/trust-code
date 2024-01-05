#include <iostream>
#include <Kokkos_Core.hpp>


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
    
    }
    Kokkos::finalize();
    return 0;
}


