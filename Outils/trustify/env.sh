#!/bin/bash
# Source environment for trustify

root_dir=$TRUST_ROOT/Outils/trustify/install

# Python version:
py_vers=`python -c "import sys; print('%d.%d' % (sys.version_info.major, sys.version_info.minor))"` 

export TRUSTIFY_ROOT_DIR=${root_dir}/lib/python${py_vers}/site-packages
export PYTHONPATH=${PYTHONPATH}:${TRUSTIFY_ROOT_DIR}

# https://groups.google.com/g/qtcontribs/c/cWQgcrbKUCM useful to avoid QXcbConnection: XCB error: 3
# export QT_LOGGING_RULES="*.debug=false"  

