#!/bin/bash
cible=$1 && [ "$1" = "" ] && exit -1

python -c "import PySide"
[ $? -eq 0 ] && echo OK && exit 0

export OPT=""
package=PySide

pac=`ls $TRUST_ROOT/externalpackages/Python_modules/$package*.gz`
pac=`basename $pac .tar.gz`
rm -rf $pac
tar zxvf $TRUST_ROOT/externalpackages/Python_modules/$pac.tar.gz
cd $pac
python setup.py bdist_wheel --qmake=/usr/bin/qmake-qt4
[ $? -ne 0 ] && echo "Pb building $package" && exit -1
pip install dist/PySide-*.whl  --prefix=$cible
[ $? -ne 0 ] && echo "Pb installing $package" && exit -1
cd ..
rm -rf $pac
echo "export CURVEPLOT_FORCE_PYSIDE=ON" >> $TRUST_ROOT/env/machine.env
