#include <fstream>
#include <iostream>
int main()
{
  bool kernelOnDevice = false;
  int sum=0;
  #pragma omp target teams distribute parallel for if (kernelOnDevice) reduction(+:sum)
  for (int num_face=0; num_face<10000; num_face++)
      sum += 1;
  std::cerr << sum << std::endl;    
  return 0;
}
