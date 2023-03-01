# Minimum version required
cmake_minimum_required (VERSION 3.18)

# Project name
project (latafiter)

# Lib name
set(lib latafilter)

# Some useful variables
set(TRUST_ROOT $ENV{TRUST_ROOT})
set(CMAKE_INSTALL_LIBDIR $ENV{TRUST_LATAFILTER})

# Compiler, flags and TRUST_DEFINES (NEEDED WHEN LINKING $exec)
set(CMAKE_CXX_COMPILER  $ENV{TRUST_CC})
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
set(TRUST_DEFINES ${TRUST_DEFINES})
add_definitions(${TRUST_DEFINES})


# List of source files
set(listdir ${TRUST_ROOT}/Outils/lata_tools/src/trust_commun/)

set(lib_src)
foreach(dir ${listdir})
 file(GLOB srcs ${dir}/*.cpp)
 list(APPEND lib_src ${srcs})
 include_directories(${dir})
endforeach()

add_library(${lib} STATIC ${lib_src})

target_include_directories(${lib} PRIVATE ${TRUST_ROOT}/src/Kernel/Utilitaires/
                           ${TRUST_ROOT}/src/Kernel/Math/
                           ${TRUST_ROOT}/src/Kernel/Geometrie/)


install(TARGETS  latafilter
        EXPORT   ${PROJECT_NAME}
        ARCHIVE  DESTINATION "${CMAKE_INSTALL_LIBDIR}/lib/")

install(DIRECTORY   ${listdir}
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/include/
        FILES_MATCHING REGEX "/*.(h)$")





