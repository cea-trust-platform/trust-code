CMAKE_MINIMUM_REQUIRED(VERSION 3.0 FATAL_ERROR)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_INSTALL_MESSAGE LAZY)

project(lata2dx)

set (loadercpp off)
option(loaderpython "build swig of lataloer" ON)
set (TRUST_INT64 $ENV{TRUST_INT64})

# if ("${TRUST_INT64}"  STREQUAL "1" )
# set (loaderpython off)
# endif ()

if ("${MED_COUPLING_ROOT}"  STREQUAL "" )
   set (MED_COUPLING_ROOT_ENV $ENV{MED_COUPLING_ROOT})
   if (NOT "${MED_COUPLING_ROOT_ENV}"  STREQUAL "" )
   set (paramedmem ${MED_COUPLING_ROOT_ENV}) 
   set (loadercpp on)
   endif()
else()
   set (paramedmem ${MED_COUPLING_ROOT}) 
   set (loadercpp on)
endif ()


if (loadercpp)
   add_definitions(-DWITH_MEDLOADER)
   find_library(libz z)
   if (${libz} STREQUAL "libz-NOTFOUND")
     set(libz "")
   endif()
   find_library(libdl dl)
   if (${libdl} STREQUAL "libdl-NOTFOUND")
     set(libdl "")
   endif()
 
   FOREACH (liba medloader medcouplingremapper medcoupling interpkernel med medC hdf5)
        set (staticlib lib${liba}.a )
        message("searching ${liba} in ${paramedmem}/lib/ $ENV{TRUST_MED_ROOT}/lib  ${MED_ROOT_DIR}/lib")
        find_library( lib${liba} NAMES ${staticlib} ${liba} PATHS  ${paramedmem}/lib/ $ENV{TRUST_MED_ROOT}/lib  ${MED_ROOT_DIR}/lib NO_DEFAULT_PATH  )
	if (${lib${liba}} STREQUAL lib${liba}-NOTFOUND)
	   message("${liba} librairie systeme ?")
	   find_library( lib${liba} NAMES ${liba} PATHS ${paramedmem}/lib/ $ENV{TRUST_MED_ROOT}/lib  ${MED_ROOT_DIR}/lib NO_DEFAULT_PATH )	
 	endif(${lib${liba}} STREQUAL lib${liba}-NOTFOUND)	    
	set (ajout ${ajout} ${lib${liba}})
   endforeach(liba)
   set (ajout  ${ajout}  ${libz} ${libdl})
   include_directories(${paramedmem}/include ${paramedmem}/include/salome )
   include_directories($ENV{TRUST_MED_ROOT}/include  ${MED_ROOT_DIR}/include)
   message ('ok')
endif(loadercpp)

# 
# Building common static library 'liblata.a'
#
set(listdir commun_triou triou_compat)
set(lib lata)
set(lib_src)
foreach(dir ${listdir})
 file(GLOB srcs ${dir}/*.cpp  )
 list(APPEND lib_src ${srcs})
 include_directories(${dir})
endforeach()
add_library(${lib} STATIC ${lib_src})


add_subdirectory(tools)
add_subdirectory(scripts)
