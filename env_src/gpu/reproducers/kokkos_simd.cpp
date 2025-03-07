#include <Kokkos_Core.hpp>
#include <Kokkos_SIMD.hpp>
#include <cmath>  // For Kokkos::exp
#include <iostream>
#include <Kokkos_MathematicalFunctions.hpp>
// SIMD Vectorized Loop using Kokkos SIMD types

int main(int argc, char* argv[]) {
    Kokkos::initialize(argc, argv);
    {
        const int N = 32000;  // Array size, ensure it's large enough for SIMD

        using simd_type = Kokkos::Experimental::native_simd<double>;
        constexpr int simd_width = simd_type::size();
        std::cout << "SIMD size=" << simd_width << std::endl;

        int nb_simd = N/simd_width;
        Kokkos::View<simd_type*> data_simd("data_simd", nb_simd);
        Kokkos::View<double*>    data_scalar("data_scalar", N);

        Kokkos::Timer timer_simd;
        Kokkos::parallel_for("SIMD", Kokkos::RangePolicy<>(0, nb_simd), KOKKOS_LAMBDA(const int i) {
            data_simd(i) = Kokkos::cos(i);
        });
        double simd_time = timer_simd.seconds();

        // Non-SIMD Loop
        Kokkos::Timer timer_scalar;
        Kokkos::parallel_for("SCALAR", Kokkos::RangePolicy<>(0, N), KOKKOS_LAMBDA(const int i) {
            data_scalar(i) = Kokkos::cos(static_cast<double>(i/simd_width));
        });
        double scalar_time = timer_scalar.seconds();
        
        // Check
        double sum_simd=0;
        for (int i=0;i<nb_simd;i++)
        {
           simd_type result = data_simd(i);
           for (int lane = 0; lane < simd_width; ++lane)
           {
               sum_simd+=result[lane];
               //printf("Simd:  %d %f\n",lane,result[lane]);
           }
        }
        double sum_scalar=0;
        for (int i=0;i<N;i++)
        {
           sum_scalar+=data_scalar(i);
           //printf("Scalar: %d %f\n",i,data_scalar(i));
        }

        // Print timing results
        std::cout << "SIMD   " << sum_simd << " loop time: " << simd_time << " seconds\n";
        std::cout << "Scalar " << sum_scalar << " loop time: " << scalar_time << " seconds\n";
        std::cout << "Speedup : x" << 0.01*int(100*scalar_time/simd_time) << std::endl;
    }
    Kokkos::finalize();
    return 0;
}
