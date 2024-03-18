# This file is NOT automatically generated - it is not 
# compiled as part of the standard TRUST kernel.

include_directories(
  ${CMAKE_SOURCE_DIR}/Kernel/Utilitaires
  ${CMAKE_SOURCE_DIR}/Kernel/Math
)

set(src_unit_arr 
     unit_array.cpp
     ${CMAKE_SOURCE_DIR}/Kernel/Math/TRUSTArray.cpp
     ${CMAKE_SOURCE_DIR}/Kernel/Math/TRUSTTravPool.cpp
)
add_executable(unit_array${OPT} ${src_unit_arr})
# Here libtrio will be the micro kernel:
target_link_libraries(unit_array${OPT} ${libtrio} ${syslib})
install (TARGETS unit_array${OPT} DESTINATION exec)
