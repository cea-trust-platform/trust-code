SET(SGPYQT_RES_DIR "${SALOME_INSTALL_RES}")
SALOME_CONFIGURE_FILE(SalomePyQt_MockUp.py.in mockup/SalomePyQt_MockUp.py)
SALOME_CONFIGURE_FILE(utils.py.in ${CMAKE_CURRENT_BINARY_DIR}/utils.py)

SET(_lib_SCRIPTS
    __init__.py
    FileManager.py
    DynamicDesktop.py
    DirScanWidget.py
    MenuTRUSTWidget.py
    TailFileWidget.py
    TrustFileWatcher.py
    ${CMAKE_CURRENT_BINARY_DIR}/mockup/SalomePyQt_MockUp.py
    ${CMAKE_CURRENT_BINARY_DIR}/utils.py
)

SET(_pyuic_files
  DirScanWidget.ui
  MenuTRUSTWidget.ui
  TailFileWidget.ui
)

SALOME_INSTALL_SCRIPTS("${_lib_SCRIPTS}" ${SALOME_INSTALL_PYTHON}/trust_plot2d)
INSTALL(FILES ${_pyuic_files} DESTINATION ${SALOME_INSTALL_PYTHON}/trust_plot2d)
