#include <Kokkos_Core.hpp>
#include <Kokkos_SIMD.hpp>
#include <iostream>
#include <Kokkos_MathematicalFunctions.hpp>
// SIMD Vectorized Loop using Kokkos SIMD types

int main(int argc, char* argv[]) {
    Kokkos::initialize(argc, argv);
    {
        const int N = 1217;  // Array size which is not multiple of 32 (to test padding is correct)
        const int samples = 1000;
	
	using mask_type = Kokkos::Experimental::native_simd_mask<double>;
        using simd_type = Kokkos::Experimental::native_simd<double>;
        constexpr int simd_width = simd_type::size();
        std::cout << "Simd size=" << simd_width << std::endl;
       
        //int nb_simd = N/simd_width;
	int nb_simd = (N + simd_width - 1)/simd_width;
        Kokkos::View<simd_type*> data_simd("data_simd", nb_simd);
        Kokkos::View<double*>    data_scalar("data_scalar", N);

        // Simd loop:
        Kokkos::Timer timer_simd;
	for (int s=0;s<samples;s++)
	{
           Kokkos::parallel_for("SIMD", nb_simd, KOKKOS_LAMBDA(const int v) {
               data_simd(v) = Kokkos::cos(v);

               // padding (do a function?)
               if ((v+1)*simd_width>N)
	       {
 	          mask_type mask([&] (int lane) { return v*simd_width+lane>=N; });
	          where(mask, data_simd(v)) = 0.0;
	       }
           });
        }
	double simd_time = timer_simd.seconds()/samples;

        // Scalar loop
        Kokkos::Timer timer_scalar;
	for (int s=0;s<samples;s++)
	{
           for (int i=0;i<N;i++) {
              data_scalar(i) = Kokkos::cos(static_cast<double>(i/simd_width));
           };
	}
        double scalar_time = timer_scalar.seconds()/samples;
        
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
        if (sum_simd!=sum_scalar) std::cout << "Error different sum for Simd and Scalar !!!" << std::endl;

        // Print timing results
        std::cout << "Simd   sum=" << sum_simd   << " loop time: " << 0.001*int(1000000*simd_time) << " ms\n";
        std::cout << "Scalar sum=" << sum_scalar << " loop time: " << 0.001*int(1000000*scalar_time) << " ms\n";
        std::cout << "Speedup : x" << 0.01*int(100*scalar_time/simd_time) << std::endl;
    }
    Kokkos::finalize();
    return 0;
}
