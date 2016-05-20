
CMAKE_MINIMUM_REQUIRED(VERSION 2.4.7 FATAL_ERROR)
    
project(lata2dx)
add_definitions(-fPIC)


set (MED_COUPLING_PYTHON $ENV{MED_COUPLING_PYTHON})

if ("${MED_COUPLING_PYTHON}"  STREQUAL "ON" )
   set (loader on)
else()
   set (loader off)
endif ()


set (MED_COUPLING_ROOT $ENV{MED_COUPLING_ROOT})

if ("${MED_COUPLING_PYTHON}"  STREQUAL "" )
   set (loadercpp off)
   else()
      set (loadercpp on)
	  endif ()


if (loadercpp)
   add_definitions(-DWITH_MEDLOADER)
   set (paramedmem $ENV{MED_COUPLING_ROOT}) 
   find_library(libz z)
   if (${libz} STREQUAL "libz-NOTFOUND")
   set(libz "")
   endif()
   find_library(libdl dl)
   if (${libdl} STREQUAL "libdl-NOTFOUND")
   set(libdl "")
   endif()
 set (ajout  ${paramedmem}/lib/libmedloader.a    ${paramedmem}/lib/libmedcouplingremapper.a ${paramedmem}/lib/libmedcoupling.a ${paramedmem}/lib/libparamedmem.a ${paramedmem}/lib/libparamedloader.a ${paramedmem}/lib/libinterpkernel.a  $ENV{TRUST_MED_ROOT}/lib/libmed.a  $ENV{TRUST_MED_ROOT}/lib/libhdf5.a ${libz} ${libdl})
include_directories(${paramedmem}/include)
 include_directories($ENV{TRUST_MED_ROOT}/include)
message ('ok')
endif(loadercpp)
set(listdir commun_triou triou_compat)
set(lib lata)
FOREACH(dir ${listdir})
 file(GLOB srcs ${dir}/*.cpp  )
 set(Monosrc ${Monosrc} ${srcs} )
 include_directories(${dir})
ENDFOREACH(dir)

add_library(${lib} STATIC ${Monosrc})

set(essai lata2dx)
add_executable (${essai} lata2dx.cpp lata2dx.h)
target_link_libraries(${essai} ${lib} )
target_link_libraries(${essai} ${ajout} )

install(TARGETS ${essai} DESTINATION exec)


set(essai compare_lata)
add_executable (${essai} compare_lata.cpp )
target_link_libraries(${essai} ${lib} ${ajout})
install(TARGETS ${essai} DESTINATION bin)

#install(TARGETS ${lib} DESTINATION lib)

if (loader)
   set(essai LataLoader)
    add_library(${essai} SHARED LataLoader.cpp LataLoader.h)
include_directories(.)
target_link_libraries(${essai} ${lib}  ${ajout} )
install(TARGETS ${essai} DESTINATION lib)

set(essai2 test_LataLoader)
add_executable (${essai2} test_LataLoader.cpp )
install(TARGETS ${essai2} DESTINATION bin)
message("la ${paramedmem}")
target_link_libraries(${essai2} ${essai})
target_link_libraries(${essai2} ${ajout} )


# swig
FIND_PACKAGE(PythonLibs)
FIND_PACKAGE(SWIG REQUIRED)
INCLUDE(${SWIG_USE_FILE})
SET_SOURCE_FILES_PROPERTIES(LataLoader.i PROPERTIES CPLUSPLUS ON)
SET_SOURCE_FILES_PROPERTIES(LataLoader.i PROPERTIES SWIG_FLAGS "-shadow")
SWIG_ADD_MODULE(LataLoader python LataLoader.i)
include_directories(${PYTHON_INCLUDE_DIRS})
SWIG_LINK_LIBRARIES(LataLoader ${PYTHON_LIBRARIES} ${essai})
install(TARGETS _LataLoader  DESTINATION lib)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/LataLoader.py   DESTINATION lib)

configure_file(env.sh.in env.sh @ONLY)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/env.sh DESTINATION . )

else(loader)
	message ("LataLoader not compiled, MED_COUPLING_PYTHON not set ?")





endif(loader)
