

SALOME_CONFIGURE_FILE(utils.py.in ${CMAKE_CURRENT_BINARY_DIR}/utils.py)
SALOME_CONFIGURE_FILE(SalomePyQt_MockUp.py.in ${CMAKE_CURRENT_BINARY_DIR}/SalomePyQt_MockUp.py)

# --- resources ---

# uic files 
SET(_pyuic_files
  MainWizWidget.ui
  GroupChooser.ui
)

# --- scripts ---
SET(_all_lib_SCRIPTS
  __init__.py
  MainWizWidget.py
  GroupChooser.py
  TrustWizDesktop.py
  extra_validation.py  
  ${CMAKE_CURRENT_BINARY_DIR}/utils.py
  ${CMAKE_CURRENT_BINARY_DIR}/SalomePyQt_MockUp.py
)

# --- rules ---
SALOME_INSTALL_SCRIPTS("${_all_lib_SCRIPTS}" ${SALOME_INSTALL_PYTHON}/trust_wiz)
INSTALL(FILES ${_pyuic_files} DESTINATION ${SALOME_INSTALL_PYTHON}/trust_wiz)
