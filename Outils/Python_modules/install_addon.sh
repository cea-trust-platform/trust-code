#!/bin/bash
#ln -sf $TRUST_ROOT/externalpackages/Python_modules/* .
cible=$1 && [ "$1" = "" ] && exit -1
export OPT=""
./create_env $cible
. $cible/env.sh
for package in numpy pytz pyparsing distribute pip wheel six  cycler python-dateutil  matplotlib functools32 vcversioner jsonschema 
do
  . $cible/env.sh
  python -c "import $package"
  [ $? -eq 0 ] && echo $package ok && ok=1
  if [ $package = pip ] 
      then
      pip -h 2> /dev/null 1 >/dev/null
      [ $? -ne 0 ] && ok=0 
  fi
  [ $ok -eq 1 ] && continue
  pac=`ls $TRUST_ROOT/externalpackages/Python_modules/$package*.gz`
  pac=`basename $pac .tar.gz`
  rm -rf $pac
  tar zxvf $TRUST_ROOT/externalpackages/Python_modules/$pac.tar.gz
  cd $pac
  python setup.py build
  [ $? -ne 0 ] && echo "Pb building $package" && exit -1
  python setup.py install --prefix=$cible
  [ $? -ne 0 ] && echo "Pb installing $package" && exit -1
  cd ..
  rm -rf $pac
done
