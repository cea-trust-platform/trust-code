file(GLOB srcs [A-Z|a-z]*.cpp [A-Z|a-z]*.c [A-Z|a-z]*.f)
<<<<<<< HEAD
include_directories(${CMAKE_CURRENT_SOURCE_DIR}  ${TRUST_ROOT}/include/backward  ${TRUST_ROOT}/src/Kernel/Champs  ${TRUST_ROOT}/src/Kernel/Champs/Champs_Don  ${TRUST_ROOT}/src/Kernel/Champs_dis  ${TRUST_ROOT}/src/Kernel/Cond_Lim  ${TRUST_ROOT}/src/Kernel/Framework  ${TRUST_ROOT}/src/Kernel/Geometrie  ${TRUST_ROOT}/src/Kernel/Math  ${TRUST_ROOT}/src/Kernel/Math/Matrices  ${TRUST_ROOT}/src/Kernel/Utilitaires  ${TRUST_ROOT}/src/Kernel/VF/Zones  ${TRUST_ROOT}/src/ThHyd  ${TRUST_ROOT}/src/ThHyd/Incompressible/Champs  ${TRUST_ROOT}/src/ThHyd/Incompressible/Cond_Lim  ${TRUST_ROOT}/src/ThHyd/Incompressible/Equations  ${TRUST_ROOT}/src/ThHyd/Incompressible/Milieu  ${TRUST_ROOT}/src/ThHyd/Incompressible/Problems  ${TRUST_ROOT}/src/ThHyd/Incompressible/Sources  ${TRUST_ROOT}/src/ThHyd/Incompressible/Traitement_particulier  ${TRUST_ROOT}/src/ThHyd/Incompressible  ${TRUST_ROOT}/src/ThHyd/Modele_turbulence_base  ${TRUST_ROOT}/src/ThSol  ${TRUST_ROOT}/src/VDF/Zones )
=======
include_directories(${CMAKE_CURRENT_SOURCE_DIR}  ${TRUST_ROOT}/include/backward  ${TRUST_ROOT}/src/Kernel/Champs  ${TRUST_ROOT}/src/Kernel/Champs_dis  ${TRUST_ROOT}/src/Kernel/Cond_Lim  ${TRUST_ROOT}/src/Kernel/Framework  ${TRUST_ROOT}/src/Kernel/Geometrie  ${TRUST_ROOT}/src/Kernel/Math/Matrices  ${TRUST_ROOT}/src/Kernel/Math  ${TRUST_ROOT}/src/Kernel/Utilitaires  ${TRUST_ROOT}/src/Kernel/VF/Zones  ${TRUST_ROOT}/src/ThHyd/Incompressible/Champs  ${TRUST_ROOT}/src/ThHyd/Incompressible/Cond_Lim  ${TRUST_ROOT}/src/ThHyd/Incompressible/Milieu  ${TRUST_ROOT}/src/ThHyd/Modele_turbulence_base  ${TRUST_ROOT}/src/VDF/Zones )
>>>>>>> [Headers] Nettoyage des make.include (merci Gauthier) avec Update_make.include.sh (attention à garder les includes des librairies tierces)
add_library(obj_chvdf OBJECT  ${srcs} )
set(listlibs ${listlibs} chvdf PARENT_SCOPE    )
add_custom_target(check_sources_chvdf   COMMAND check_sources.sh ${CMAKE_CURRENT_SOURCE_DIR} ) #COMMENT  "checking code validity chvdf" )
add_DEPENDENCIES(obj_chvdf check_sources_chvdf)
if(COMPIL_DYN)
  add_library(chvdf SHARED   $<TARGET_OBJECTS:obj_chvdf> )
  install(TARGETS chvdf  DESTINATION lib)
endif(COMPIL_DYN)
