# Minimum version required
cmake_minimum_required (VERSION 3.18)

# Project name
project (latafiter)

# Lib name
set(lib latafilter)

# Some useful variables
set(TRUST_ROOT $ENV{TRUST_ROOT})
set(TRUST_KOKKOS_ROOT $ENV{TRUST_KOKKOS_ROOT})
set(CMAKE_INSTALL_LIBDIR $ENV{TRUST_LATAFILTER})

# TRUST_DEFINES (NEEDED WHEN LINKING $exec)
add_definitions(${TRUST_DEFINES})

#set(with_kokkos $ENV{TRUST_USE_KOKKOS})
#if(with_kokkos)
#    list(APPEND CMAKE_PREFIX_PATH $ENV{TRUST_KOKKOS_ROOT})
#    find_package(Kokkos REQUIRED)
#    if(Kokkos_ENABLE_CUDA)
#        kokkos_check(OPTIONS CUDA_LAMBDA)
#    endif()
#endif()

# List of source files
set(sourcedir ${TRUST_ROOT}/Outils/lata_tools/src/trust_commun/)
file(GLOB cpp_sources ${sourcedir}/*.cpp)

add_library(${lib} STATIC ${cpp_sources})
if(with_kokkos)
  target_link_libraries(${lib} Kokkos::kokkos)
endif()

target_include_directories(${lib} PRIVATE
                           ${sourcedir}
                           ${TRUST_ROOT}/src/Kernel/Utilitaires/
                           ${TRUST_ROOT}/src/Kernel/Math/
                           ${TRUST_ROOT}/src/Kernel/Geometrie/
                           ${TRUST_KOKKOS_ROOT}/include/ )


install(TARGETS  ${lib}
        EXPORT   ${PROJECT_NAME}
        ARCHIVE  DESTINATION "${CMAKE_INSTALL_LIBDIR}/lib/")

install(DIRECTORY   ${sourcedir}
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/include
        FILES_MATCHING REGEX "/*.(h)$")





