include_directories(.)

#
# Build lata_analyzer executable
#
set(exec lata_analyzer)
add_executable (${exec} lata_analyzer.cpp)
target_link_libraries(${exec} ${lib} ${ajout} )
install(TARGETS ${exec} DESTINATION bin)

#
# Build compare_lata executable
#
set(exec compare_lata)
add_executable (${exec} compare_lata.cpp )
target_link_libraries(${exec} ${lib} ${ajout})
install(TARGETS ${exec} DESTINATION bin)

#
# Build lataloader if requested
#
if (loadercpp)
  #
  # Library
  #
  set(tgt_ll LataLoader)
  add_library(${tgt_ll} SHARED LataLoader.cpp)
  if (CYGWIN)
    add_library(${tgt_ll}.st STATIC LataLoader.cpp)
  endif()
  target_link_libraries(${tgt_ll} ${lib}  ${ajout} )
  install(TARGETS ${tgt_ll} DESTINATION lib)
  #
  # Test executable
  #
  set(tgt_tst test_LataLoader)
  add_executable (${tgt_tst} test_LataLoader.cpp )
  install(TARGETS ${tgt_tst} DESTINATION bin)
  target_link_libraries(${tgt_tst} ${tgt_ll} ${ajout})

  #
  # Swig
  #
  if (loaderpython)
    find_package(PythonLibs REQUIRED)
    find_package(PythonInterp REQUIRED)
    find_package(SWIG REQUIRED)
    include(${SWIG_USE_FILE})

    set_source_files_properties(LataLoader.i PROPERTIES CPLUSPLUS ON)
    set_source_files_properties(LataLoader.i PROPERTIES SWIG_FLAGS "-shadow")
   
   IF(APPLE)
     SET_PROPERTY(SOURCE LataLoader.i PROPERTY COMPILE_DEFINITIONS WIN32)
   ENDIF()

    if(${CMAKE_VERSION} VERSION_LESS "3.8.0")
        swig_add_module(LataLoader python LataLoader.i)
    else()
        swig_add_library(LataLoader LANGUAGE python SOURCES LataLoader.i)
    endif()

    include_directories(${PYTHON_INCLUDE_DIRS})
    if (CYGWIN)
      swig_link_libraries(LataLoader ${PYTHON_LIBRARIES} ${tgt_ll}.st  ${lib}  ${ajout})
    else()
      swig_link_libraries(LataLoader ${tgt_ll})  # Do not add PYTHON_LIBRARIES as in MC
    endif()
    install(TARGETS _LataLoader  DESTINATION lib)
    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/LataLoader.py   DESTINATION lib)
  else()
    message ("LataLoader.py not compiled, env var TRUST_INT64=1, and not -Dloaderpython=ON ?")
  endif()

else()
  message ("LataLoader not compiled, MED_COUPLING_ROOT not set ?")
endif(loadercpp)
