cmake_minimum_required(VERSION 3.5)

set(ATELIER  ON)
include(../../build/src/CMakeLists.txt.trio)      # Needs to be put before "project()" (it tweaks compiler)
set(LIBRARY_OUTPUT_PATH  ${CMAKE_BUILD_DIR})   # Make sure we don't pollute TRUST/lib directory

project(ICoCo_SWIG CXX)
enable_testing()

set(TRUST_ROOT $ENV{TRUST_ROOT})
set(MEDCOUPLING_DIR $ENV{TRUST_MEDCOUPLING_ROOT})

add_definitions(-Wno-error)
add_definitions(-Wno-shadow)

set(CMAKE_CXX_COMPILER  $ENV{TRUST_CC})
set(CMAKE_SKIP_RULE_DEPENDENCY 0)

message("Utilisation de la librairie : ${libtrio_name} " )
find_library(libTrio NAMES ${libtrio_name}.a PATHS ${TRUST_ROOT}/lib NO_DEFAULT_PATH )

find_package(PythonInterp 3 REQUIRED)  # Make sure to find TRUST Python first (even if not technically needed) ...
find_package(PythonLibs 3 REQUIRED)    # ... so that TRUST Python libs are found too.

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
  ${PYTHON_INCLUDE_PATH})

set_source_files_properties(trusticoco.i PROPERTIES CPLUSPLUS ON)
set_source_files_properties(trusticoco.i PROPERTIES SWIG_FLAGS "-py3")
set_source_files_properties(trusticoco_V2.i PROPERTIES CPLUSPLUS ON)
set_source_files_properties(trusticoco_V2.i PROPERTIES SWIG_FLAGS "-py3")
if(NOT $ENV{TRUST_DISABLE_MEDCOUPLING})
  set_source_files_properties(trusticoco.i PROPERTIES COMPILE_DEFINITIONS "MEDCOUPLING")
  #set_source_files_properties(trusticoco.i PROPERTIES COMPILE_DEFINITIONS "OLD_MEDCOUPLING")
  set_source_files_properties(trusticoco_V2.i PROPERTIES COMPILE_DEFINITIONS "MEDCOUPLING")
  #set_source_files_properties(trusticoco_V2.i PROPERTIES COMPILE_DEFINITIONS "OLD_MEDCOUPLING")
endif()

if(${CMAKE_VERSION} VERSION_LESS "3.8.0") 
  swig_add_module(trusticoco python trusticoco.i)
  swig_add_module(trusticoco_V2 python trusticoco_V2.i)
else()
  swig_add_library(trusticoco LANGUAGE python SOURCES trusticoco.i)
  swig_add_library(trusticoco_V2 LANGUAGE python SOURCES trusticoco_V2.i)
endif()

swig_link_libraries(trusticoco ${libdeps} ${libTrio} ${syslib} ${PYTHON_LIBRARIES} )
swig_link_libraries(trusticoco_V2 ${libdeps} ${libTrio} ${syslib} ${PYTHON_LIBRARIES} )

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
           ${PYTHON_EXECUTABLE} test_trusticoco.py
         WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/test)

if(NOT $ENV{TRUST_DISABLE_MEDCOUPLING})
    add_test(NAME TRUST_swig_V2
             COMMAND ${CMAKE_COMMAND} -E env
                  PYTHONPATH=${CMAKE_BINARY_DIR}:$ENV{PYTHONPATH}
               ${PYTHON_EXECUTABLE} test_trusticoco_V2.py
             WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/test)
endif()

##
## Installation
##
install(TARGETS ${SWIG_MODULE_trusticoco_REAL_NAME} DESTINATION lib)
install(TARGETS ${SWIG_MODULE_trusticoco_V2_REAL_NAME} DESTINATION lib)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/trusticoco.py DESTINATION lib)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/trusticoco_V2.py DESTINATION lib)
