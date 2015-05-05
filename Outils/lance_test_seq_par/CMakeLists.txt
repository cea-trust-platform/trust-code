CMAKE_MINIMUM_REQUIRED(VERSION 2.4.7 FATAL_ERROR)
# Pour avoir lance_test_seq_par et compare_sonde sous visual


project(lance_test_seq_par)
if(VISUAL)
add_definitions(-DMICROSOFT)
endif(VISUAL)
add_executable(lance_test_seq_par  lance_test_seq_par.cpp )
add_executable(compare_sonde  compare_sonde.cpp )
install(TARGETS lance_test_seq_par DESTINATION exec)
