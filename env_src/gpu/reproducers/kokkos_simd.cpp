#include <Kokkos_Core.hpp>
#include <Kokkos_SIMD.hpp>
#include <cmath>  // For Kokkos::exp
#include <iostream>
#include <Kokkos_MathematicalFunctions.hpp>

int main(int argc, char* argv[]) {
    Kokkos::initialize(argc, argv);
    {
        const int N = 10000000;  // Array size, ensure it's large enough for SIMD
        Kokkos::View<double*> data_simd("data_simd", N);
        Kokkos::View<double*> data_no_simd("data_no_simd", N);

        // SIMD Vectorized Loop using Kokkos SIMD types
        //using simd_type = Kokkos::Experimental::simd<double, Kokkos::Experimental::simd_abi::avx2>;
        using simd_type = Kokkos::Experimental::native_simd<double>;
        constexpr int simd_width = simd_type::size();

        Kokkos::Timer timer_simd;
        Kokkos::parallel_for("FillExpArray_SIMD", Kokkos::RangePolicy<>(0, N/simd_width), KOKKOS_LAMBDA(const int i) {
            simd_type idx = simd_type(i * simd_width);
            //simd_type result = Kokkos::exp(idx);  // Kokkos SIMD exp with g++ is not vectorized (only Intel compiler)
            simd_type result = Kokkos::cos(idx);
            for (int lane = 0; lane < simd_width; ++lane) {
                data_simd(i * simd_width + lane) = result[lane];
            }
        });
        Kokkos::fence();
        double simd_time = timer_simd.seconds();

        // Non-SIMD Loop using Kokkos::exp (sequential)
        Kokkos::Timer timer_no_simd;
        Kokkos::parallel_for("FillExpArray_No_SIMD", Kokkos::RangePolicy<>(0, N), KOKKOS_LAMBDA(const int i) {
            //data_no_simd(i) = Kokkos::exp(static_cast<double>(i));
            data_no_simd(i) = Kokkos::cos(static_cast<double>(i));
        });
        Kokkos::fence();
        double no_simd_time = timer_no_simd.seconds();

        // Print timing results
        std::cout << "SIMD (Kokkos SIMD types) loop time: " << simd_time << " seconds\n";
        std::cout << "Non-SIMD (Kokkos parallel_for) loop time: " << no_simd_time << " seconds\n";
        std::cout << "Speedup : x" << 0.01*int(100*no_simd_time/simd_time) << std::endl;
    }
    Kokkos::finalize();
    return 0;
}
