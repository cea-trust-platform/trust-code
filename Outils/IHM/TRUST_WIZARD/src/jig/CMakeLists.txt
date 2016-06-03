
SET(_SCRIPTS
    __init__.py
    generator.py
    loader.py
    validator.py
    widgets.py
    views.py
)

SALOME_INSTALL_SCRIPTS("${_SCRIPTS}" ${SALOME_INSTALL_PYTHON}/jig)
