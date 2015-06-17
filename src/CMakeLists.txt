# Tell the CMake makefile generator to not have rules depend on
# themselves.  This causes extra rebuilds when the include path
# changes from turning a kit on or off.
SET(CMAKE_SKIP_RULE_DEPENDENCY 0 )

#OPTION(kernelonly "kernel only" OFF)
#OPTION(microkernel "micro kernel only" OFF)
#OPTION(microAL "micro kernel + AL" OFF)
set(kernel "full" CACHE STRING "full,standard,numeric,micro" )

OPTION(VISUAL "pour MSVC" OFF)

IF(NOT VISUAL)
 # on prend les compilos choisis par configure
 # pour retirer des blancs inutils
 set (pathcc $ENV{TRUST_CC})
 string(STRIP ${pathcc}  CMAKE_GENERATOR_CXX)
 set (CMAKE_GENERATOR_CC $ENV{TRUST_cc})
 set (CMAKE_GENERATOR_Fortran $ENV{TRUST_F77})
ENDIF(NOT VISUAL)

project(Trio)



if(EXISTS ${CMAKE_SOURCE_DIR}/cmake.deps)
   include(cmake.deps)
endif()


set(listdir src/EF/Champs src/EF/fortran src/EF/Operateurs src/EF/Sources src/EF/Zones src/Kernel/Champs src/Kernel/Champs_dis src/Kernel/Cond_Lim src/Kernel/Framework src/Kernel/Geometrie src/Kernel/Geometrie/Decoupeur src/Kernel/ICoCo src/Kernel/Math src/Kernel/Math/Matrices src/Kernel/Math/SolvSys src/Kernel/MEDimpl src/Kernel/Operateurs src/Kernel/Schemas_Temps src/Kernel/Solveurs src/Kernel/Statistiques_temps src/Kernel/Utilitaires src/Kernel/VF/Champs src/Kernel/VF/Zones src/P1NCP1B src/P1NCP1B/Champs src/P1NCP1B/Cond_Lim src/P1NCP1B/Operateurs src/P1NCP1B/Solveurs src/P1NCP1B/Turbulence src/P1NCP1B/Zones src/ThHyd src/ThHyd/Chimie src/ThHyd/Modele_turbulence_base src/ThHyd/Quasi_Compressible src/ThHyd/Quasi_Compressible/Turbulence src/ThHyd/Quasi_Compressible/VDF src/ThHyd/Quasi_Compressible/VEF src/ThHyd/Schemas_Temps src/ThHyd/Turbulence src/ThHyd/Turbulence/Spectres src/ThSol src/VDF src/VDF/Axi/Operateurs src/VDF/Axi/Operateurs/Evaluateurs src/VDF/Axi/Sources src/VDF/Axi/Turbulence src/VDF/Champs src/VDF/Cond_Lim src/VDF/Elements src/VDF/Operateurs src/VDF/Operateurs/Evaluateurs src/VDF/Operateurs/Iterateurs src/VDF/Solveurs src/VDF/Sources src/VDF/Sources/Evaluateurs src/VDF/Sources/Iterateurs src/VDF/Turbulence src/VDF/Zones src/VEF src/VEF/Champs src/VEF/Cond_Lim src/VEF/Operateurs src/VEF/Solveurs src/VEF/Sources src/VEF/Sources/Evaluateurs src/VEF/Sources/Iterateurs src/VEF/Turbulence src/VEF/Zones src/MAIN)



set(listdir_standard src/Kernel/Champs src/Kernel/Champs_dis src/Kernel/Cond_Lim src/Kernel/Framework src/Kernel/Geometrie src/Kernel/Geometrie/Decoupeur src/Kernel/ICoCo src/Kernel/MEDimpl src/Kernel/Math src/Kernel/Math/Matrices src/Kernel/Math/SolvSys src/Kernel/Operateurs src/Kernel/Schemas_Temps src/Kernel/Solveurs src/Kernel/Statistiques_temps src/Kernel/Utilitaires src/Kernel/VF/Champs src/Kernel/VF/Zones  src/MAIN)

set(listdir_micro src/Kernel/Math src/Kernel/Utilitaires  src/MAIN)
set(listdir_numeric src/Kernel/Math src/Kernel/Math/Matrices  src/Kernel/Math/SolvSys src/Kernel/Utilitaires src/MAIN  )


if (NOT ${kernel} STREQUAL "full")
  MESSAGE("Kernel ${kernel}")
  set (listdirmod ${listdir_${kernel}})
  set(ajout _${kernel}_kernel)
  set(special_srcs ${CMAKE_CURRENT_BINARY_DIR}/appel_c_mod_${kernel}.cpp)
  message("special srs ${special_srcs}")
  set (oo "")
  foreach(d ${listdir})
    LIST(FIND  listdirmod ${d} trouve)
    if (${trouve} EQUAL -1)
      STRING(REPLACE "/" "_" d2 ${d})
      set(oo "${oo} void instancie_${d2}() { } ")
    endif() 
  endforeach(d)
  set(oo "${oo} 
")
  if (EXISTS  ${special_srcs})
    FILE(READ  ${special_srcs} org)
    string(COMPARE NOTEQUAL ${org} ${oo} update_src)
  else()
    set(update_src 1)
  endif()
  if (update_src)  
    message( "update ${special_srcs}")
    FILE(WRITE ${special_srcs} "${oo}")
  endif(update_src)
  
  set(listdir ${listdirmod})
endif()

CMAKE_MINIMUM_REQUIRED(VERSION 2.6 FATAL_ERROR)
    



IF(NOT VISUAL)

enable_language(Fortran OPTIONAL)

SET (MPI_INCLUDE $ENV{MPI_INCLUDE})
SET (TRUST_ARCH $ENV{TRUST_ARCH})
# OPT sert a petsc pour l instant
#SET(OPT $ENV{OPT})
if (CMAKE_BUILD_TYPE STREQUAL "Release")
    set(OPT "_opt")
elseif (CMAKE_BUILD_TYPE STREQUAL "Release_avx")
    set(OPT "_opt_avx")
elseif (CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(OPT "")
elseif (CMAKE_BUILD_TYPE STREQUAL "Profil")
    set(OPT "_opt_pg")
elseif (CMAKE_BUILD_TYPE STREQUAL "Coverage")
    set(OPT "_opt_gcov")
elseif (CMAKE_BUILD_TYPE STREQUAL "semi_opt")
    set(OPT "_semi_opt")
else(CMAKE_BUILD_TYPE STREQUAL "Release") 
   message(FATAL_ERROR  "unknown build_type ${CMAKE_BUILD_TYPE} !, use -DCMAKE_BUILD_TYPE=Release,Debug,Profil,Coverage,semi_opt")
endif(CMAKE_BUILD_TYPE STREQUAL "Release")
message("Mode: ${OPT}")


SET(COMM $ENV{COMM})
SET (PETSC_ROOT $ENV{PETSC_ROOT} )
SET (METIS_ROOT $ENV{METIS_ROOT} )
SET (TRUST_LATAFILTER $ENV{TRUST_LATAFILTER})
SET (TRUST_ICOCOAPI $ENV{TRUST_ICOCOAPI})
SET (TRUST_MED_ROOT $ENV{TRUST_MED_ROOT})
SET (TRUST_MEDCOUPLING_ROOT $ENV{TRUST_MEDCOUPLING_ROOT})
SET (TRUST_ROOT $ENV{TRUST_ROOT})



#set(COMPIL_DYN FALSE)

set(LIBRARY_OUTPUT_PATH ${TRUST_ROOT}/lib)
set(EXECUTABLE_OUTPUT_PATH ${TRUST_ROOT}/exec)


include (${TRUST_ROOT}/env/Cmake.env)
include (${TRUST_ROOT}/env/Cmake.libs)

FOREACH (liba ${list_libs})
        set (staticlib lib${liba}.a )
         find_library( lib${liba} NAMES ${staticlib} ${liba} PATHS ${list_path_libs} NO_DEFAULT_PATH )
	if (${lib${liba}} STREQUAL lib${liba}-NOTFOUND)
	   message("${liba} librairie systeme ?")
	   find_library( lib${liba} NAMES ${liba} PATHS ${list_path_sys} )
 	endif(${lib${liba}} STREQUAL lib${liba}-NOTFOUND)
        # pour supermuc on cherche ligfortran.so.3  en dur
        if (${liba} STREQUAL gfortran)
          if (${lib${liba}} STREQUAL libgfortran-NOTFOUND)
           find_library( lib${liba} NAMES  libgfortran.so.3 PATHS ${list_path_sys} )
          endif()
        endif()
        MARK_AS_ADVANCED( lib${liba})
        SET(libs ${libs} ${lib${liba}})
ENDFOREACH (liba )


ELSE(NOT VISUAL)
  include(windows/CMake.win)
ENDIF(NOT VISUAL)




STRING( TOUPPER ${CMAKE_BUILD_TYPE} BUILD_CONFIG)
string(STRIP ${CMAKE_EXE_LINKER_FLAGS_${BUILD_CONFIG}} linker_flag )
SET(syslib ${libs} ${linker_flag} )

include_directories(${METIS_ROOT}/include  ${TRUST_MED_ROOT}/include  ${TRUST_MEDCOUPLING_ROOT}/include  ${MPI_INCLUDE} ${PETSC_ROOT}/${TRUST_ARCH}${OPT}/include ${TRUST_LATAFILTER}/include ${TRUST_ICOCOAPI}/include   )
add_definitions(${ADD_CPPFLAGS})


 
 set(trio TRUST${COMM}${OPT})
 set(libtrio_name TRUST${COMM}${ajout}${OPT})
 set(libtrio lib${libtrio_name})

IF(NOT ATELIER)
FOREACH(dir ${listdir})
   add_subdirectory(../${dir})
 ENDFOREACH(dir)

if(NOT COMPIL_DYN)
  set(my_listobj)
  foreach(_obj IN LISTS listlibs)
    LIST (APPEND my_listobj  $<TARGET_OBJECTS:obj_${_obj}>)
  endforeach()
  
 add_library(${libtrio} STATIC  ${special_srcs} ${my_listobj} )
 set_target_properties(${libtrio} PROPERTIES OUTPUT_NAME ${libtrio_name} PREFIX "" )
 install(TARGETS ${libtrio} DESTINATION lib)
else(NOT COMPIL_DYN)
 set(libtrio ${listlibs})
  
endif(NOT COMPIL_DYN)
 
 # on ne produit pas d executable en mode partiel
 IF((  "${ajout}" STREQUAL "" ) OR ($ENV{FORCE_LINK}))
   add_executable (${trio} MAIN/the_main.cpp MAIN/mon_main.cpp ${special_srcs}  )
   include_directories(Kernel/Utilitaires MAIN Kernel/Math)
   target_link_libraries(${trio} ${libtrio} ${syslib})
   install (TARGETS ${trio} DESTINATION exec)
 ENDIF()
 
 
ELSE(NOT ATELIER)
FOREACH(dir ${listdir})
   include_directories(${TRUST_ROOT}/${dir})
 ENDFOREACH(dir)
ENDIF(NOT ATELIER)


