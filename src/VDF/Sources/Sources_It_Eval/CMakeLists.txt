file(GLOB srcs [A-Z|a-z]*.cpp [A-Z|a-z]*.c [A-Z|a-z]*.f)
<<<<<<< HEAD
include_directories(${CMAKE_CURRENT_SOURCE_DIR}  ${TRUST_ROOT}/include/backward  ${TRUST_ROOT}/src/Kernel/Champs  ${TRUST_ROOT}/src/Kernel/Champs/Champs_Don  ${TRUST_ROOT}/src/Kernel/Champs_dis  ${TRUST_ROOT}/src/Kernel/Cond_Lim  ${TRUST_ROOT}/src/Kernel/Framework  ${TRUST_ROOT}/src/Kernel/Geometrie  ${TRUST_ROOT}/src/Kernel/Math/Matrices  ${TRUST_ROOT}/src/Kernel/Math/SolvSys  ${TRUST_ROOT}/src/Kernel/Math  ${TRUST_ROOT}/src/Kernel/Operateurs  ${TRUST_ROOT}/src/Kernel/Utilitaires  ${TRUST_ROOT}/src/Kernel/VF/Zones  ${TRUST_ROOT}/src/ThHyd/Incompressible  ${TRUST_ROOT}/src/ThHyd/Incompressible/Sources  ${TRUST_ROOT}/src/ThHyd/Incompressible/Equations  ${TRUST_ROOT}/src/ThHyd/Incompressible/Milieu  ${TRUST_ROOT}/src/ThHyd/Incompressible/Traitement_particulier  ${TRUST_ROOT}/src/ThSol  ${TRUST_ROOT}/src/VDF/Champs  ${TRUST_ROOT}/src/VDF/Sources/Evaluateurs  ${TRUST_ROOT}/src/VDF/Sources/Iterateurs  ${TRUST_ROOT}/src/VDF/Zones)
=======
include_directories(${CMAKE_CURRENT_SOURCE_DIR}  ${TRUST_ROOT}/include/backward  ${TRUST_ROOT}/src/Kernel/Champs  ${TRUST_ROOT}/src/Kernel/Cond_Lim  ${TRUST_ROOT}/src/Kernel/Framework  ${TRUST_ROOT}/src/Kernel/Geometrie  ${TRUST_ROOT}/src/Kernel/Math/Matrices  ${TRUST_ROOT}/src/Kernel/Math/SolvSys  ${TRUST_ROOT}/src/Kernel/Math  ${TRUST_ROOT}/src/Kernel/Operateurs  ${TRUST_ROOT}/src/Kernel/Utilitaires  ${TRUST_ROOT}/src/Kernel/VF/Zones  ${TRUST_ROOT}/src/ThHyd/Incompressible/Equations  ${TRUST_ROOT}/src/ThHyd/Incompressible/Milieu  ${TRUST_ROOT}/src/ThHyd/Incompressible/Sources  ${TRUST_ROOT}/src/ThHyd/Incompressible/Traitement_particulier  ${TRUST_ROOT}/src/ThHyd/Incompressible  ${TRUST_ROOT}/src/ThSol  ${TRUST_ROOT}/src/VDF/Sources/Evaluateurs  ${TRUST_ROOT}/src/VDF/Sources/Iterateurs  ${TRUST_ROOT}/src/VDF/Zones )
>>>>>>> [Headers] Nettoyage des make.include (merci Gauthier) avec Update_make.include.sh (attention à garder les includes des librairies tierces)
add_library(obj_sovdf_using_it_eval OBJECT  ${srcs} )
set(listlibs ${listlibs} sovdf_using_it_eval PARENT_SCOPE    )
add_custom_target(check_sources_sovdf_using_it_eval   COMMAND check_sources.sh ${CMAKE_CURRENT_SOURCE_DIR} ) #COMMENT  "checking code validity sovdf_using_it_eval" )
add_DEPENDENCIES(obj_sovdf_using_it_eval check_sources_sovdf_using_it_eval)
if(COMPIL_DYN)
  add_library(sovdf_using_it_eval SHARED   $<TARGET_OBJECTS:obj_sovdf_using_it_eval> )
  install(TARGETS sovdf_using_it_eval  DESTINATION lib)
endif(COMPIL_DYN)
