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

# include file:
medcoupling_hxx=$DEST/include/medcoupling++.h
cp -af $medcoupling_hxx .

rm -rf build install $medcoupling

rm -f ICoCoMEDField.hxx
if [ "$TRUST_USE_EXTERNAL_MEDCOUPLING" = "1" ]
then
 echo $MEDCOUPLING_ROOT_DIR
 ln -sf $MEDCOUPLING_ROOT_DIR $DEST
 exit 0
fi
[ ! -f $archive ] && echo $archive no such file && exit 1
tar zxf $archive

echo patching MEDCouplingMemArray.i
echo "cp $org/MEDCouplingMemArray.i $medcoupling/src/MEDCoupling_Swig/MEDCouplingMemArray.i"
cp $org/MEDCouplingMemArray.i $medcoupling/src/MEDCoupling_Swig/MEDCouplingMemArray.i

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

echo patching MEDCouplingTimeLabel.hxx 
cp $org/MEDCouplingTimeLabel.hxx $(find $medcoupling -name  MEDCouplingTimeLabel.hxx )

mkdir build
cd build

#export HDF5_ROOT=$TRUST_MED_ROOT
#export MED3HOME=$TRUST_MED_ROOT


USE_MPI=ON
[ "$TRUST_DISABLE_MPI" -eq 1 ] && USE_MPI=OFF

# We use now python 2.7.16 and swig from conda so:
MED_COUPLING_PYTHON="ON"

OPTIONS="-DCMAKE_BUILD_TYPE=Release -DMEDCOUPLING_USE_MPI=$USE_MPI -DMPI_ROOT_DIR=$MPI_ROOT -DCMAKE_INSTALL_PREFIX=$DEST -DCMAKE_CXX_COMPILER=$TRUST_CC  -DHDF5_ROOT_DIR=$TRUST_MED_ROOT  -DMEDFILE_ROOT_DIR=$TRUST_MED_ROOT -DMEDCOUPLING_BUILD_DOC=OFF  -DMEDCOUPLING_PARTITIONER_METIS=OFF -DMEDCOUPLING_PARTITIONER_SCOTCH=OFF -DMEDCOUPLING_ENABLE_RENUMBER=OFF -DMEDCOUPLING_ENABLE_PARTITIONER=OFF -DMEDCOUPLING_BUILD_TESTS=OFF -DMEDCOUPLING_WITH_FILE_EXAMPLES=OFF -DCONFIGURATION_ROOT_DIR=../configuration-$mc_version -DSWIG_EXECUTABLE=$TRUST_ROOT/exec/python/bin/swig"
OPTIONS=$OPTIONS" -DMEDCOUPLING_MEDLOADER_USE_XDR=OFF" 
# NO_CXX1 pour cygwin
OPTIONS=$OPTIONS" -DMEDCOUPLING_BUILD_STATIC=ON -DNO_CXX11_SUPPORT=ON"
cmake ../$medcoupling $OPTIONS -DMEDCOUPLING_ENABLE_PYTHON=$MED_COUPLING_PYTHON


# The current CMake of MEDCoupling is badly written: dependencies on .pyc generation do not properly capture SWIG generated Python modules.
# So we need to do make twice ...
make -j $TRUST_NB_PROCS
# si make install fonctionne coorectement ce fichier sera ecrase
# echo "#define NO_MEDFIELD " > $DEST/include/ICoCoMEDField.hxx
make install
make install
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
    exit -1
  fi
fi

touch $DEST/include/*

[ ! -f $DEST/include/ICoCoMEDField.hxx ] && echo "#define NO_MEDFIELD " > $DEST/include/ICoCoMEDField.hxx
[ ! -f $medcoupling_hxx ] && echo "#define MEDCOUPLING_" > $medcoupling_hxx

exit $status
