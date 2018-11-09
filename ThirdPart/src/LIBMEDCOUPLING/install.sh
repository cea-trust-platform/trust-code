#!/bin/bash

archive=$1
medcoupling=`basename $archive .tar.gz`
mc_version=`echo $medcoupling | sed 's/[^0-9]*\([0-9].[0-9].[0-9]\)/\1/'`
org=`pwd`

DEST=$TRUST_MEDCOUPLING_ROOT
mkdir -p $DEST 
cd $DEST/.. 

dest=$DEST/include/ICoCoMEDField.hxx
cp -af $dest .

rm -rf build install $medcoupling

# New include file:
medcoupling_hxx=$DEST/include/medcoupling++.h
if [ "$TRUST_DISABLE_MED" = "1" ] || [ "$TRUST_DISABLE_MEDCOUPLING" = "1" ] 
then
    mkdir -p $DEST/include
    rm -rf $DEST/lib
    echo "MED or MEDCOUPLING DISABLE"
    echo "#define NO_MEDFIELD " >  prov.h

    if [ "`diff ICoCoMEDField.hxx prov.h 2>&1`" != "" ]
	then
	cp prov.h $dest
    else
	cp -a ICoCoMEDField.hxx $dest
    fi
    echo "#undef MEDCOUPLING_" > $medcoupling_hxx
    rm -f prov.h ICoCoMEDField.hxx
    exit 0
fi
rm -f ICoCoMEDField.hxx
if [ "$TRUST_USE_EXTERNAL_MEDCOUPLING" = "1" ]
then
 echo $MEDCOUPLING_ROOT_DIR
 ln -sf $MEDCOUPLING_ROOT_DIR $DEST
 exit 0
fi
[ ! -f $archive ] && echo $archive no such file && exit 1
tar zxf $archive

echo patching MEDCouplingFieldDouble
cp $org/MEDCouplingFieldDouble.hxx $(find $medcoupling -name  MEDCouplingFieldDouble.hxx )

echo patching DisjointDEC
cp $org/DisjointDEC.hxx $(find $medcoupling -name  DisjointDEC.hxx )
cp $org/DisjointDEC.cxx $(find $medcoupling -name  DisjointDEC.cxx )

echo patching Interpolation1D0D.txx 
cp $org/Interpolation1D0D.txx $(find $medcoupling -name Interpolation1D0D.txx )

echo patching MEDCouplingSkyLineArray.cxx 
cp $org/MEDCouplingSkyLineArray.cxx $(find $medcoupling -name  MEDCouplingSkyLineArray.cxx )

# for swig_import_helper error with icpc 17
echo patching MEDCouplingMemArray.cxx 
cp $org/MEDCouplingMemArray.cxx $(find $medcoupling -name  MEDCouplingMemArray.cxx )

mkdir build
cd build

#export HDF5_ROOT=$TRUST_MED_ROOT
#export MED3HOME=$TRUST_MED_ROOT


USE_MPI=ON
[ "$TRUST_DISABLE_MPI" -eq 1 ] && USE_MPI=OFF

# Do not build Python MEDCoupling if Python < 2.7 -- will be fixed directly in MEDCoupling from version 8.4
PY_VERSION_MAJOR=`python -c 'import sys; print(sys.version_info[0])'`
PY_VERSION_MINOR=`python -c 'import sys; print(sys.version_info[1])'`
if [[ $PY_VERSION_MAJOR == 2 && $PY_VERSION_MINOR < 7 ]]
then 
	echo "Detected Python version is too old. Not building MEDCoupling Python bindings"
	MED_COUPLING_PYTHON="OFF"
else
	MED_COUPLING_PYTHON="ON"
fi

OPTIONS="-DCMAKE_BUILD_TYPE=Release -DMEDCOUPLING_USE_MPI=$USE_MPI -DMPI_ROOT_DIR=$MPI_ROOT -DCMAKE_INSTALL_PREFIX=$DEST -DCMAKE_CXX_COMPILER=$TRUST_CC  -DHDF5_ROOT_DIR=$TRUST_MED_ROOT  -DMEDFILE_ROOT_DIR=$TRUST_MED_ROOT -DMEDCOUPLING_BUILD_DOC=OFF  -DMEDCOUPLING_PARTITIONER_METIS=OFF -DMEDCOUPLING_PARTITIONER_SCOTCH=OFF -DMEDCOUPLING_ENABLE_RENUMBER=OFF -DMEDCOUPLING_ENABLE_PARTITIONER=OFF -DMEDCOUPLING_BUILD_TESTS=OFF -DMEDCOUPLING_WITH_FILE_EXAMPLES=OFF -DCONFIGURATION_ROOT_DIR=../configuration-$mc_version"
OPTIONS=$OPTIONS" -DMEDCOUPLING_MEDLOADER_USE_XDR=OFF" 
# NO_CXX1 pour cygwin
OPTIONS=$OPTIONS" -DMEDCOUPLING_BUILD_STATIC=ON -DNO_CXX11_SUPPORT=ON"
cmake ../$medcoupling $OPTIONS -DMEDCOUPLING_ENABLE_PYTHON=$MED_COUPLING_PYTHON

status=$?

if [ $status -ne 0 ] && [ "$MED_COUPLING_PYTHON" = "ON" ]
then
  echo "we tried to compile without python"
  cmake ../$medcoupling $OPTIONS -DMEDCOUPLING_ENABLE_PYTHON=OFF
  MED_COUPLING_PYTHON="OFF"
fi


make -j $TRUST_NB_PROCS
# si make install fonctionne coorectement ce fichier sera ecrase
# echo "#define NO_MEDFIELD " > $DEST/include/ICoCoMEDField.hxx
make install
make install
status=$?
if [ $status -ne 0 ]
then
  echo "we tried to compile without python"
  cmake ../$medcoupling $OPTIONS -DMEDCOUPLING_ENABLE_PYTHON=OFF
  MED_COUPLING_PYTHON="OFF"
  make install
fi
status=$?

#ar cru $DEST/lib/libParaMEDMEM.a  `find src -name '*'.o`

MC_ENV_FILE=$DEST/env.sh
version=`python  -c "import sys; print sys.version[:3]"`
echo "export MED_COUPLING_ROOT=$DEST"> $MC_ENV_FILE
echo "export LD_LIBRARY_PATH=$DEST/lib/:$TRUST_MED_ROOT/lib:\${LD_LIBRARY_PATH}" >> $MC_ENV_FILE
echo "export PYTHONPATH=$DEST/bin/:$DEST/lib/python$version/site-packages/:\$PYTHONPATH" >> $MC_ENV_FILE
echo "export MED_COUPLING_PYTHON=$MED_COUPLING_PYTHON" >> $MC_ENV_FILE

if [ $status -eq 0 ]  # install was successful
then
  #cd ..
  #rm -rf build $medcoupling
  ##
  ## Test de fonctionnement
  ##
  source $MC_ENV_FILE
  python -c "import MEDCoupling"
  if [ $? -eq 0 ]
  then
    echo "MEDCoupling library OK"
  else
    echo "MEDCoupling library KO"
    #exit -1
  fi
fi

touch $DEST/include/*

[ ! -f $DEST/include/ICoCoMEDField.hxx ] && echo "#define NO_MEDFIELD " > $DEST/include/ICoCoMEDField.hxx
[ ! -f $medcoupling_hxx ] && echo "#define MEDCOUPLING_" > $medcoupling_hxx

exit $status
