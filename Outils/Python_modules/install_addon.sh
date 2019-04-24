#!/bin/bash
#ln -sf $TRUST_ROOT/externalpackages/Python_modules/* .




cible=$1 && [ "$1" = "" ] && exit -1

if [ -f  $cible/env.sh ]
    then
    . $cible/env.sh
    python -c "import matplotlib;import jsonschema"
    [ $? -eq 0 ] && [ "$FORCE_BUILD" != 1 ] && echo OK && exit 0
fi

export OPT=""
./create_env $cible
. $cible/env.sh
for package in numpy pytz pyparsing distribute Distribute2 pip wheel six  cycler python-dateutil  matplotlib functools32 vcversioner jsonschema 
do
  ok=0
  . $cible/env.sh
  python -c "import $package"
  [ $? -eq 0 ] && echo $package ok && ok=1
  if [ $package = pip ] 
      then
      pip -h 2> /dev/null 1 >/dev/null
      [ $? -ne 0 ] && ok=0 
  fi

  if [ $package = "distribute_N" ]  || [ $package = "Distribute2_N" ]
   then
       tes=`python -c 'import setuptools;import sys;print  (setuptools.__version__>="0.7")'`
     [ $tes = "True" ] && ok=1
fi

  [ $ok -eq 1 ] && [ "$FORCE_BUILD" != 1 ] &&continue
  if [ $package = "Distribute2" ]
then
   pac=distribute-0.7.3
   rm -rf $pac
   unzip $TRUST_ROOT/externalpackages/Python_modules/$pac.zip
else
  pac=`ls $TRUST_ROOT/externalpackages/Python_modules/$package*.gz`
  pac=`basename $pac .tar.gz`
  rm -rf $pac
  echo Untar $pac
  tar zxf $TRUST_ROOT/externalpackages/Python_modules/$pac.tar.gz
fi
  cd $pac
  python setup.py build
  [ $? -ne 0 ] && echo "Pb building $package" 
  python setup.py install --prefix=$cible
  [ $? -ne 0 ] && echo "Pb installing $package"
  cd ..
  if [ $package != distribute ] && [ $package != Distribute2 ] 
  then
   rm -rf $pac
  fi
done
python -c "import matplotlib;import jsonschema"
