cmake_minimum_required(VERSION 3.18)

set(ATELIER  ON)
include(../../build/src/CMakeLists.txt.trust)  # Needs to be put before "project()" (it tweaks compiler)
set(LIBRARY_OUTPUT_PATH  ${CMAKE_BUILD_DIR})   # Make sure we don't pollute TRUST/lib directory
#SET(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)   # Added when we had a dependency on libpython*.so

project(ICoCo_SWIG CXX)

enable_testing()

set(TRUST_ROOT $ENV{TRUST_ROOT})
set(MEDCOUPLING_DIR "$ENV{TRUST_MEDCOUPLING_ROOT}/$ENV{TRUST_ARCH}${OPT}")

if(NOT $ENV{TRUST_ARCH_CC} STREQUAL "linux_nvc++")
add_definitions(-Wno-error)
add_definitions(-Wno-suggest-override)
add_definitions(-Wno-shadow)
else()
# Disable some warnings for nvc++
add_definitions(--diag_suppress1349 --diag_suppress550)
endif()

set(CMAKE_CXX_COMPILER  $ENV{TRUST_CC})
set(CMAKE_SKIP_RULE_DEPENDENCY 0)

message("Utilisation de la librairie : ${libtrio_name} " )
find_library(libTrio NAMES ${libtrio_name}.a PATHS ${TRUST_ROOT}/lib NO_DEFAULT_PATH )

##
## libbaltik_name and libBaltik for TRUST baltiks (such as triocfd)
##
#set(libbaltik_name _TRUSTModule${ajout}${OPT})
#SET (project_directory $ENV{project_directory})

#message("Utilisation de la librairie : ${libbaltik_name} " )
#find_library(libBaltik NAMES ${libbaltik_name}.so PATHS ${project_directory}/build/src/exec${ajout}${OPT} NO_DEFAULT_PATH )

#if (NOT libBaltik)
#    message(FATAL_ERROR "Could not find BALTIK library: ${libbaltik_name}.so - did you run: make module_optim in your baltik (potentially in debug)??")
#endif()

if (APPLE)
  # Necessary for MAC, otherwise system python detected when finding package Python3
  find_package(PythonInterp 3 REQUIRED)  # Make sure to find TRUST Python first (even if not technically needed) ...
  find_package(PythonLibs 3 REQUIRED)    # ... so that TRUST Python libs are found too.
endif()

find_package(Python3 COMPONENTS Interpreter Development.Module  REQUIRED)  # Make sure to find TRUST Python first (even if not technically needed) ...
find_package(SWIG REQUIRED)
include(${SWIG_USE_FILE})

##
## Python module build
##
include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${TRUST_ROOT}/src/Kernel/ICoCo
  ${TRUST_ROOT}/lib/src/LIBICOCOAPI/include
  ${MEDCOUPLING_DIR}/include
  ${Python3_INCLUDE_DIRS})

message("@@@@ Python3_INCLUDE_DIRS : ${Python3_INCLUDE_DIRS} ")
message("@@@@ Python3_EXECUTABLE : ${Python3_EXECUTABLE} ")

# Configuring .i file to put TRUST version into it
set(TRUST_VERSION $ENV{TRUST_VERSION})
set(swig_file ${CMAKE_CURRENT_BINARY_DIR}/trusticoco.i)
configure_file(trusticoco.i.in ${swig_file} @ONLY)

set_source_files_properties(${swig_file} PROPERTIES CPLUSPLUS ON)
set(comp_def_)
if(NOT $ENV{TRUST_DISABLE_MEDCOUPLING})
    list(APPEND comp_def_ "MEDCOUPLING")
endif()

# mpi4py should be installed by Conda if compiled with MPI:
if(NOT "$ENV{TRUST_DISABLE_MPI}" STREQUAL "1")
  message(STATUS "Compiling with MPI support!")
  include_directories("$ENV{MPI_INCLUDE}")
  list(APPEND comp_def_ "WITH_MPI")
endif()
set_source_files_properties(${swig_file} PROPERTIES COMPILE_DEFINITIONS "${comp_def_}")

if(${CMAKE_VERSION} VERSION_LESS "3.8.0") 
  swig_add_module(trusticoco python ${swig_file})
else()
  swig_add_library(trusticoco LANGUAGE python SOURCES ${swig_file})
endif()

if (APPLE)
 swig_link_libraries(trusticoco ${libdeps} ${libTrio} ${syslib} ${PYTHON_LIBRARIES} )
else()
  swig_link_libraries(trusticoco ${libdeps} ${libTrio} ${syslib})   # ${PYTHON_LIBRARIES} removed as in MC
  #swig_link_libraries(trusticoco ${libdeps} ${libBaltik} ${libTrio} ${syslib} ${PYTHON_LIBRARIES} )
endif()

##
## Tests
##
# Copy files to binary since a trust run generates a lot of crap ... 
file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/test)
file(GLOB tst_files "${CMAKE_CURRENT_SOURCE_DIR}/test/*")
file(COPY ${tst_files} DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/test)
add_test(NAME TRUST_swig
         COMMAND ${CMAKE_COMMAND} -E env
              PYTHONPATH=${CMAKE_BINARY_DIR}:$ENV{PYTHONPATH}
           ${Python3_EXECUTABLE} test_trusticoco.py
         WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/test)

# If MPI, test setting communicator
if(NOT "$ENV{TRUST_DISABLE_MPI}" STREQUAL "1")
  add_test(NAME TRUST_swig_mpi
         COMMAND ${CMAKE_COMMAND} -E env
              PYTHONPATH=${CMAKE_BINARY_DIR}:$ENV{PYTHONPATH}
           mpirun.sh -np 2 ${Python3_EXECUTABLE} test_icoco_comm.py
         WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/test)
endif()


##
## Installation
##
install(TARGETS ${SWIG_MODULE_trusticoco_REAL_NAME} DESTINATION lib)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/trusticoco.py DESTINATION lib)
