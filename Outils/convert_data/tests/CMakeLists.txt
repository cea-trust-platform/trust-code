cmake_minimum_required(VERSION 3.4)
project(ConvertScripts)

enable_testing()

set(SCR_DIR "$ENV{TRUST_ROOT}/Outils/convert_data")

set(MED_SCR "${SCR_DIR}/convert_liremed.py")
set(FT_SCR  "${SCR_DIR}/convert_interf.py")
set(MIL_SCR "${SCR_DIR}/convert_milieu.py")
set(EXEC_AND_DIFF "${SCR_DIR}/tests/exec_and_diff.sh")

file(GLOB med_jdds "med/*.data")
file(GLOB mil_jdds "milieu/*.data")
file(GLOB ft_jdds "ft/*.data")

# Create directories
file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/med_out)
file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/mil_out)
file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/FT_out)

# Add conversion tests for Milieu
foreach(ze_file ${mil_jdds})
   get_filename_component(base_name ${ze_file} NAME)
   set(data_out "${base_name}.out")
   set(data_ref "${CMAKE_CURRENT_SOURCE_DIR}/milieu_ref/${base_name}")
   add_test(NAME "MILIEU_${base_name}" 
            COMMAND ${EXEC_AND_DIFF} ${MIL_SCR} ${ze_file} ${data_out} ${data_ref}
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
endforeach()

# Add conversion tests for FT
foreach(ze_file ${ft_jdds})
   get_filename_component(base_name ${ze_file} NAME)
   set(data_out "${base_name}.out")
   set(data_ref "${CMAKE_CURRENT_SOURCE_DIR}/ft_ref/${base_name}")
   add_test(NAME "FT_${base_name}" 
            COMMAND ${EXEC_AND_DIFF} ${FT_SCR} ${ze_file} ${data_out} ${data_ref}
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
endforeach()

# Add conversion tests for Lire_MED
foreach(ze_file ${med_jdds})
   get_filename_component(base_name ${ze_file} NAME)
   set(data_out "${base_name}.out")
   set(data_ref "${CMAKE_CURRENT_SOURCE_DIR}/med_ref/${base_name}")
   add_test(NAME "MED_${base_name}" 
            COMMAND ${EXEC_AND_DIFF} ${MED_SCR} ${ze_file} ${data_out} ${data_ref}
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
endforeach()

