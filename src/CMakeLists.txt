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
 string(STRIP ${pathcc} CMAKE_CXX_COMPILER)
 set (CMAKE_C_COMPILER $ENV{TRUST_cc})
 set (CMAKE_Fortran_COMPILER $ENV{TRUST_F77})
ENDIF(NOT VISUAL)

project(TRUST)



if(EXISTS ${CMAKE_SOURCE_DIR}/cmake.deps)
   include(cmake.deps)
endif()


set(listdir src/CoviMAC src/CoviMAC/Champs src/CoviMAC/Cond_Lim src/CoviMAC/Operateurs src/CoviMAC/Solveurs src/CoviMAC/Sources src/CoviMAC/Zones src/EF/Champs src/EF/fortran src/EF/Operateurs src/EF/Sources src/EF/Zones src/Kernel/Champs src/Kernel/Champs_dis src/Kernel/Cond_Lim src/Kernel/Framework src/Kernel/Geometrie src/Kernel/Geometrie/Decoupeur src/Kernel/Geometrie/Interpolation_IBM src/Kernel/ICoCo src/Kernel/Math src/Kernel/Math/Matrices src/Kernel/Math/SolvSys src/Kernel/MEDimpl src/Kernel/Operateurs src/Kernel/Schemas_Temps src/Kernel/Solveurs src/Kernel/Statistiques_temps src/Kernel/Utilitaires src/Kernel/VF/Champs src/Kernel/VF/Zones src/P1NCP1B src/P1NCP1B/Champs src/P1NCP1B/Cond_Lim src/P1NCP1B/Operateurs src/P1NCP1B/Solveurs src/P1NCP1B/Zones src/PolyMAC/Champs src/PolyMAC/Cond_Lim src/PolyMAC/Operateurs src/PolyMAC/Operateurs/Conv_iterateur src/PolyMAC/Sources src/PolyMAC/Zones src/ThHyd/Chimie src/ThHyd/Dilatable/Common src/ThHyd/Dilatable/Common/Cond_Lim src/ThHyd/Dilatable/Common/Equations src/ThHyd/Dilatable/Common/Loi_Etat src/ThHyd/Dilatable/Common/Milieu src/ThHyd/Dilatable/Common/Problems src/ThHyd/Dilatable/Common/Sources src/ThHyd/Dilatable/Common/VDF src/ThHyd/Dilatable/Common/VEF src/ThHyd/Dilatable/Quasi_Compressible src/ThHyd/Dilatable/Quasi_Compressible/Cond_Lim src/ThHyd/Dilatable/Quasi_Compressible/EF src/ThHyd/Dilatable/Quasi_Compressible/Equations src/ThHyd/Dilatable/Quasi_Compressible/Loi_Etat src/ThHyd/Dilatable/Quasi_Compressible/Milieu src/ThHyd/Dilatable/Quasi_Compressible/Problems src/ThHyd/Dilatable/Quasi_Compressible/Sources src/ThHyd/Dilatable/Quasi_Compressible/VDF src/ThHyd/Dilatable/Quasi_Compressible/VEF src/ThHyd/Dilatable/Weakly_Compressible/Equations src/ThHyd/Dilatable/Weakly_Compressible/Loi_Etat src/ThHyd/Dilatable/Weakly_Compressible/Milieu src/ThHyd/Dilatable/Weakly_Compressible/Problems src/ThHyd/Dilatable/Weakly_Compressible/Sources src/ThHyd/Dilatable/Weakly_Compressible/VDF src/ThHyd/Dilatable/Weakly_Compressible/VEF src/ThHyd/Incompressible src/ThHyd/Incompressible/Champs src/ThHyd/Incompressible/Cond_Lim src/ThHyd/Incompressible/Equations src/ThHyd/Incompressible/Milieu src/ThHyd/Incompressible/Problems src/ThHyd/Incompressible/Sources src/ThHyd/Incompressible/Traitement_particulier src/ThHyd/Modele_turbulence_base src/ThHyd/Multiphase src/ThHyd/Multiphase/Correlations src/ThHyd/Multiphase/CoviMAC src/ThHyd/Multiphase/Equations src/ThHyd/Multiphase/Milieu src/ThHyd/Multiphase/Operateurs src/ThHyd/Multiphase/Schemas_Temps src/ThHyd/Schemas_Temps src/ThHyd/Turbulence src/ThSol src/VDF src/VDF/Axi/Sources src/VDF/Champs src/VDF/Cond_Lim src/VDF/Elements src/VDF/Operateurs/Eval_Conv src/VDF/Operateurs/Eval_Diff_Dift src/VDF/Operateurs/Eval_Divers src/VDF/Operateurs/Iterateurs src/VDF/Operateurs/Op_Conv src/VDF/Operateurs/Op_Diff_Dift src/VDF/Operateurs/Op_Diff_Dift/Op_Diff_Dift_base src/VDF/Operateurs/Op_Divers src/VDF/Solveurs src/VDF/Sources src/VDF/Sources/Evaluateurs src/VDF/Sources/Iterateurs src/VDF/Zones src/VEF src/VEF/Champs src/VEF/Cond_Lim src/VEF/Operateurs src/VEF/Solveurs src/VEF/Sources src/VEF/Sources/Evaluateurs src/VEF/Sources/Iterateurs src/VEF/Zones src/MAIN)



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

CMAKE_MINIMUM_REQUIRED(VERSION 3.0 FATAL_ERROR)




IF(NOT VISUAL)

enable_language(Fortran OPTIONAL)

SET (MPI_INCLUDE $ENV{MPI_INCLUDE})
SET (CUDA_INC_PATH $ENV{CUDA_INC_PATH})
SET (TRUST_ARCH $ENV{TRUST_ARCH})
SET (TRUST_ROOT $ENV{TRUST_ROOT})
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
elseif (CMAKE_BUILD_TYPE STREQUAL "custom")
    set(OPT "_custom")
    include (${TRUST_ROOT}/env/Cmake.custom)
else(CMAKE_BUILD_TYPE STREQUAL "Release")
   message(FATAL_ERROR  "unknown build_type ${CMAKE_BUILD_TYPE} !, use -DCMAKE_BUILD_TYPE=Release,Debug,Profil,Coverage,semi_opt,Release_avx,custom")
endif(CMAKE_BUILD_TYPE STREQUAL "Release")
message("Mode: ${OPT}")


SET(COMM $ENV{COMM})
SET (PETSC_ROOT $ENV{PETSC_ROOT} )
SET (METIS_ROOT $ENV{METIS_ROOT} )
SET (TRUST_LATAFILTER $ENV{TRUST_LATAFILTER})
SET (TRUST_ICOCOAPI $ENV{TRUST_ICOCOAPI})
SET (TRUST_MED_ROOT $ENV{TRUST_MED_ROOT})
SET (TRUST_MEDCOUPLING_ROOT $ENV{TRUST_MEDCOUPLING_ROOT})



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

include_directories(${METIS_ROOT}/include ${TRUST_MED_ROOT}/include ${TRUST_MEDCOUPLING_ROOT}/include ${MPI_INCLUDE} ${TRUST_ROOT}/lib/src/LIBAMGX/AmgXWrapper/include ${TRUST_ROOT}/lib/src/LIBAMGX/AmgX/include ${CUDA_INC_PATH} ${PETSC_ROOT}/${TRUST_ARCH}${OPT}/include ${TRUST_LATAFILTER}/include ${TRUST_ICOCOAPI}/include ${TRUST_ROOT}/lib/src/LIBOSQP/include ${TRUST_ROOT}/lib/src/LIBVC/include )
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


   ENABLE_TESTING()
   SET (TRUST_TESTS $ENV{TRUST_TESTS})
   #file(GLOB_RECURSE LML  RELATIVE ${TRUST_TESTS} FOLLOW_SYMLINKS ${TRUST_TESTS}/[A-Z|a-z]*.lml.gz )
   file(GLOB_RECURSE LML   FOLLOW_SYMLINKS ${TRUST_TESTS}/[A-Z|a-z|0-9]*.lml.gz )
#message("${ajout} iiiii ${LML} oooo")
LIST (LENGTH  LML NN)
message("go ${NN} tests")
foreach(f ${LML})
string(REPLACE .lml.gz "" l2 ${f} )
string(FIND ${f} /  ii  REVERSE )
string(SUBSTRING ${l2} ${ii} -1 l3 )
string(SUBSTRING ${l3} 1 -1 l3 )
ADD_TEST(NAME ${l3} COMMAND trust -exe $<TARGET_FILE:${trio}> -check ${l3}  )
endforeach(f )

 ENDIF()


ELSE(NOT ATELIER)
FOREACH(dir ${listdir})
   include_directories(${TRUST_ROOT}/${dir})
 ENDFOREACH(dir)
ENDIF(NOT ATELIER)


