CMAKE_MINIMUM_REQUIRED(VERSION 3.0 FATAL_ERROR)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_INSTALL_MESSAGE LAZY)

project(lata_tools)

set (loadercpp off)
option(loaderpython "Build swig of lataloader" ON)
set (TRUST_INT64 $ENV{TRUST_INT64})

# if ("${TRUST_INT64}"  STREQUAL "1" )
# set (loaderpython off)
# endif ()

if ("${MEDCOUPLING_ROOT}"  STREQUAL "" )
   set (MEDCOUPLING_ROOT_ENV $ENV{TRUST_MEDCOUPLING_ROOT})
   if (NOT "${MEDCOUPLING_ROOT_ENV}"  STREQUAL "" )
     set (med_dir "${MEDCOUPLING_ROOT_ENV}/$ENV{TRUST_ARCH}_opt") 
     set (loadercpp on)
   endif()
else()
   set (med_dir ${MEDCOUPLING_ROOT}) 
   set (loadercpp on)
endif ()


if (loadercpp)
   add_definitions(-DWITH_MEDLOADER)
   find_library(libz z)
   if (NOT libz) # Not found
     set(libz "")
   endif()
   find_library(libdl dl)
   if (NOT libdl) # Not found
     set(libdl "")
   endif()
 
   set(lib_paths 
      ${med_dir}/lib 
      $ENV{TRUST_MED_ROOT}/lib  
      ${MED_ROOT_DIR}/lib
   )
 
   set (libs
    medloader medcouplingremapper medcoupling interpkernel 
    med medC hdf5
   )

   foreach(liba IN LISTS libs)
        set (staticlib lib${liba}.a )
        message(STATUS "Searching ${liba} ...")
        find_library( lib${liba} NAMES ${staticlib} ${liba} PATHS ${lib_paths} NO_DEFAULT_PATH  )
        if (NOT ${lib${liba}})  # Not found
          message(STATUS "${liba} system library ?")
          find_library( lib${liba} NAMES ${liba} PATHS ${lib_paths} NO_DEFAULT_PATH REQUIRED)
          message(STATUS "Found at ${lib${liba}}")
        endif()
        set (ajout ${ajout} ${lib${liba}})
   endforeach()

   set (ajout  ${ajout}  ${libz} ${libdl})
   include_directories(${med_dir}/include ${med_dir}/include/salome )
   include_directories($ENV{TRUST_MED_ROOT}/include  
        ${MED_ROOT_DIR}/include
   )
endif(loadercpp)

# 
# Building common static library 'liblata.a'
#
set(listdir trust_commun)
set(lib lata)
set(lib_src)
foreach(dir ${listdir})
   file(GLOB srcs ${dir}/*.cpp  )
   list(APPEND lib_src ${srcs})
   include_directories(${dir})
endforeach()

#
# Compile definitions. Notably lata_tools is compiled in 64b, whatever the choice
# indicated at configure time.
#
add_definitions("-DLATATOOLS")
add_definitions("-DINT_is_64_=2")   # For example will make sure all template instanciation include trustIdType signatures
add_definitions("$ENV{TRUST_DEFINES}")

#
# Extra sources from TRUST :
# The extra sources coming from TRUST kernel - see env variable defined in root makefile:
#
set(trust_kern "$ENV{TRUST_ROOT}/src/Kernel")
set(extra_src)
string (REPLACE " " ";" extra_src $ENV{EXTRA_SRCS})

include_directories(
    ${trust_kern}/Utilitaires
    ${trust_kern}/Math
    ${trust_kern}/Geometrie
)

add_library(${lib} STATIC ${lib_src} ${extra_src})

add_subdirectory(tools)
add_subdirectory(scripts)
