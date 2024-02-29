#include <Kokkos_SIMD.hpp>
#include <cstdio>

int main(int argc, char* argv[]) {
  Kokkos::initialize(argc,argv);
  {
  using simd_type = Kokkos::Experimental::native_simd<double>;
  simd_type a([] (std::size_t i) { return 0.1 * i; });
  simd_type b(2.0);
  simd_type c = Kokkos::sqrt(a * a + b * b);
  for (std::size_t i = 0; i < simd_type::size(); ++i) {
    printf("[%zu] = %g\n", i, c[i]);
  }
  }
  Kokkos::finalize();
}
