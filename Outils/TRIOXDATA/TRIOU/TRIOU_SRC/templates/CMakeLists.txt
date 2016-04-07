# --
# Copyright (C) CEA, EDF
# Author : Erwan ADAM (CEA)
# --


include(${XDATA_INSTALLATION_DIR}/share/xdata/salome/cmake_begin.cmake)
set(DEST templates)
install(
  DIRECTORY . 
  DESTINATION ${DEST}
  PATTERN CMakeLists.txt EXCLUDE
  PATTERN First.py EXCLUDE
)

include(${XDATA_INSTALLATION_DIR}/share/xdata/salome/cmake_end.cmake)
