file(GLOB srcs [A-Z|a-z]*.cpp [A-Z|a-z]*.c [A-Z|a-z]*.f)
<<<<<<< HEAD
include_directories(${CMAKE_CURRENT_SOURCE_DIR}  ${TRUST_ROOT}/include/backward  ${TRUST_ROOT}/src/Kernel/Champs  ${TRUST_ROOT}/src/Kernel/Champs/Champs_Don  ${TRUST_ROOT}/src/Kernel/Champs_dis  ${TRUST_ROOT}/src/Kernel/Cond_Lim  ${TRUST_ROOT}/src/Kernel/Framework  ${TRUST_ROOT}/src/Kernel/Geometrie  ${TRUST_ROOT}/src/Kernel/Math  ${TRUST_ROOT}/src/Kernel/Math/Matrices  ${TRUST_ROOT}/src/Kernel/Math/SolvSys  ${TRUST_ROOT}/src/Kernel/Operateurs  ${TRUST_ROOT}/src/Kernel/Utilitaires  ${TRUST_ROOT}/src/Kernel/VF/Zones  ${TRUST_ROOT}/src/ThHyd/Incompressible/Champs  ${TRUST_ROOT}/src/ThHyd/Incompressible/Cond_Lim  ${TRUST_ROOT}/src/ThHyd/Incompressible/Equations  ${TRUST_ROOT}/src/ThHyd/Incompressible/Milieu  ${TRUST_ROOT}/src/ThHyd/Incompressible/Problems  ${TRUST_ROOT}/src/ThHyd/Incompressible/Sources  ${TRUST_ROOT}/src/ThHyd/Incompressible/Traitement_particulier  ${TRUST_ROOT}/src/ThHyd/Incompressible  ${TRUST_ROOT}/src/ThHyd  ${TRUST_ROOT}/src/ThSol  ${TRUST_ROOT}/src/VEF/Champs  ${TRUST_ROOT}/src/VEF/Zones )
=======
include_directories(${CMAKE_CURRENT_SOURCE_DIR}  ${TRUST_ROOT}/include/backward  ${TRUST_ROOT}/src/Kernel/Champs  ${TRUST_ROOT}/src/Kernel/Champs_dis  ${TRUST_ROOT}/src/Kernel/Cond_Lim  ${TRUST_ROOT}/src/Kernel/Framework  ${TRUST_ROOT}/src/Kernel/Geometrie  ${TRUST_ROOT}/src/Kernel/Math/Matrices  ${TRUST_ROOT}/src/Kernel/Math/SolvSys  ${TRUST_ROOT}/src/Kernel/Math  ${TRUST_ROOT}/src/Kernel/Operateurs  ${TRUST_ROOT}/src/Kernel/Utilitaires  ${TRUST_ROOT}/src/Kernel/VF/Zones  ${TRUST_ROOT}/src/ThHyd/Incompressible/Cond_Lim  ${TRUST_ROOT}/src/ThHyd/Incompressible/Equations  ${TRUST_ROOT}/src/ThHyd/Incompressible/Milieu  ${TRUST_ROOT}/src/ThHyd/Incompressible/Traitement_particulier  ${TRUST_ROOT}/src/ThSol  ${TRUST_ROOT}/src/VEF/Champs  ${TRUST_ROOT}/src/VEF/Zones )
>>>>>>> [Headers] Nettoyage des make.include (merci Gauthier) avec Update_make.include.sh (attention à garder les includes des librairies tierces)
add_library(obj_vefcl OBJECT  ${srcs} )
set(listlibs ${listlibs} vefcl PARENT_SCOPE    )
add_custom_target(check_sources_vefcl   COMMAND check_sources.sh ${CMAKE_CURRENT_SOURCE_DIR} ) #COMMENT  "checking code validity vefcl" )
add_DEPENDENCIES(obj_vefcl check_sources_vefcl)
if(COMPIL_DYN)
  add_library(vefcl SHARED   $<TARGET_OBJECTS:obj_vefcl> )
  install(TARGETS vefcl  DESTINATION lib)
endif(COMPIL_DYN)
