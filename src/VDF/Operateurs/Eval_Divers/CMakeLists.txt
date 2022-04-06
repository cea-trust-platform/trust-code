file(GLOB srcs [A-Z|a-z]*.cpp [A-Z|a-z]*.c [A-Z|a-z]*.f)
<<<<<<< HEAD
include_directories(${CMAKE_CURRENT_SOURCE_DIR}  ${TRUST_ROOT}/include/backward  ${TRUST_ROOT}/src/Kernel/Champs  ${TRUST_ROOT}/src/Kernel/Champs/Champs_Don  ${TRUST_ROOT}/src/Kernel/Champs_dis  ${TRUST_ROOT}/src/Kernel/Cond_Lim  ${TRUST_ROOT}/src/Kernel/Framework  ${TRUST_ROOT}/src/Kernel/Geometrie  ${TRUST_ROOT}/src/Kernel/Math  ${TRUST_ROOT}/src/Kernel/Utilitaires  ${TRUST_ROOT}/src/Kernel/VF/Zones  ${TRUST_ROOT}/src/VDF/Champs  ${TRUST_ROOT}/src/VDF/Operateurs/Eval_Conv  ${TRUST_ROOT}/src/VDF/Zones)
=======
include_directories(${CMAKE_CURRENT_SOURCE_DIR}  ${TRUST_ROOT}/include/backward  ${TRUST_ROOT}/src/Kernel/Framework  ${TRUST_ROOT}/src/Kernel/Geometrie  ${TRUST_ROOT}/src/Kernel/Math  ${TRUST_ROOT}/src/Kernel/Utilitaires  ${TRUST_ROOT}/src/Kernel/VF/Zones  ${TRUST_ROOT}/src/VDF/Zones )
>>>>>>> [Headers] Nettoyage des make.include (merci Gauthier) avec Update_make.include.sh (attention à garder les includes des librairies tierces)
add_library(obj_evalvdf_divers OBJECT  ${srcs} )
set(listlibs ${listlibs} evalvdf_divers PARENT_SCOPE    )
add_custom_target(check_sources_evalvdf_divers   COMMAND check_sources.sh ${CMAKE_CURRENT_SOURCE_DIR} ) #COMMENT  "checking code validity evalvdf_divers" )
add_DEPENDENCIES(obj_evalvdf_divers check_sources_evalvdf_divers)
if(COMPIL_DYN)
  add_library(evalvdf_divers SHARED   $<TARGET_OBJECTS:obj_evalvdf_divers> )
  install(TARGETS evalvdf_divers  DESTINATION lib)
endif(COMPIL_DYN)
