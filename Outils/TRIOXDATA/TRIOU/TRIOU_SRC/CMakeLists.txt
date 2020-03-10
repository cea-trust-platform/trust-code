# --
# Copyright (C) CEA, EDF
# Author : Erwan ADAM (CEA)
# --

cmake_minimum_required(VERSION 2.4)

if(CMAKE_HOST_WIN32)
  set(WINDOWS 1)
else(CMAKE_HOST_WIN32)
  set(WINDOWS 0)
endif(CMAKE_HOST_WIN32)

if(WINDOWS)
  set(PYTHON_EXE $ENV{PYTHONBIN})
else(WINDOWS)
  set(PYTHON_EXE python)
endif(WINDOWS)

set(CMD "import xdata ; print (xdata.getInstallationDirectory())")
execute_process(
  COMMAND ${PYTHON_EXE} -c "${CMD}"
  OUTPUT_VARIABLE XDATA_INSTALLATION_DIR
  RESULT_VARIABLE status
  ERROR_QUIET
  OUTPUT_STRIP_TRAILING_WHITESPACE
  )
if(status)
  message(FATAL_ERROR "import xdata in python failed ! Cannot continue ...")
endif(status)

include(${XDATA_INSTALLATION_DIR}/share/xdata/salome/cmake_base.cmake)

include(${XDATA_INSTALLATION_DIR}/share/xdata/salome/cmake_begin.cmake)

add_subdirectory(src)
add_subdirectory(templates)

include(${XDATA_INSTALLATION_DIR}/share/xdata/salome/cmake_end.cmake)
