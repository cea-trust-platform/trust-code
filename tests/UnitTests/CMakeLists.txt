# This file is NOT automatically generated - it is not 
# compiled as part of the standard TRUST kernel.

include_directories(
  ${CMAKE_SOURCE_DIR}/Kernel/Utilitaires
  ${CMAKE_SOURCE_DIR}/Kernel/Math
  ${CMAKE_SOURCE_DIR}/MAIN
  $ENV{TRUST_GTEST_ROOT}/include  # For GTest
)

find_library(gtestlib NAMES gtest HINTS $ENV{TRUST_GTEST_ROOT}/lib REQUIRED)
message(STATUS "Found GTest library at: ${gtestlib}")

#find_library(kern_num_lib NAMES TRUST_mpi_numeric_kernel.a PATHS $ENV{TRUST_ROOT}/lib REQUIRED)
#message(STATUS "Found TRUST_mpi_numeric_kernel library at: ${kern_num_lib}")

#
#  Sequential
#
set(src_unit
     main_gtest.cpp
     unit_array.cpp
     unit_vect_tools.cpp
     unit_device.cpp
     unit_array_kokkos.cpp
#     ${CMAKE_SOURCE_DIR}/Kernel/Math/TRUSTArray.cpp
#     ${CMAKE_SOURCE_DIR}/Kernel/Math/TRUSTVect_tools.cpp
#     ${CMAKE_SOURCE_DIR}/Kernel/Math/TRUSTTravPool.cpp
)

set(exe_name "unit_gtest")

add_executable(${exe_name} ${src_unit})
target_compile_options(${exe_name} PRIVATE -Wno-unused-variable)
# Here libtrio will be the micro kernel:
target_link_libraries(${exe_name} ${gtestlib} ${libtrio} ${syslib})
#target_link_libraries(${exe_name} ${gtestlib})
install (TARGETS ${exe_name} DESTINATION exec)

#
# Parallel
#

set(src_unit_mpi
     main_gtest_mpi.cpp
     unit_mpi.cpp
     unit_tab_tools.cpp
#     ${CMAKE_SOURCE_DIR}/Kernel/Math/TRUSTArray.cpp
#     ${CMAKE_SOURCE_DIR}/Kernel/Math/TRUSTTravPool.cpp
#     ${CMAKE_SOURCE_DIR}/Kernel/Math/MD_Vector_base.cpp
)


set(exe_name_mpi "unit_gtest_mpi")

add_executable(${exe_name_mpi} ${src_unit_mpi})
target_compile_options(${exe_name_mpi} PRIVATE -Wno-unused-variable)
## Here libtrio will be the micro kernel:
target_link_libraries(${exe_name_mpi} ${gtestlib} ${libtrio} ${syslib})
install (TARGETS ${exe_name_mpi} DESTINATION exec)
