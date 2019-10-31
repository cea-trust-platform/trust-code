#!/bin/bash

archive_mc=$1
archive_conf=$2
medcoupling=`basename $archive_mc .tar.gz`
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

# MEDCoupling uses DataArrayInt32 not DataArrayInt64, so we disable MEDCoupling when building a 64 bits version of TRUST
if [ "$TRUST_INT64" = "1" ]
then
    mkdir -p $DEST/include
    rm -rf $DEST/lib
    echo "MEDCOUPLING DISABLE for 64 bits"
    echo "#define NO_MEDFIELD " >  prov.h

    if [ "`diff ICoCoMEDField.hxx prov.h 2>&1`" != "" ]
    then
	cp prov.h $dest
    else
	cp -a ICoCoMEDField.hxx $dest
    fi
    echo "#undef MEDCOUPLING_" > prov2.h
    if [ "`diff medcoupling++.h prov2.h 2>&1`" != "" ] 
    then
       cp prov2.h $medcoupling_hxx
    else
       cp -a medcoupling++.h $medcoupling_hxx
    fi
    rm -f prov.h prov2.h ICoCoMEDField.hxx medcoupling++.h
    exit 0
fi

rm -f ICoCoMEDField.hxx
if [ "$TRUST_USE_EXTERNAL_MEDCOUPLING" = "1" ]
then
 echo $MEDCOUPLING_ROOT_DIR
 ln -sf $MEDCOUPLING_ROOT_DIR $DEST
 exit 0
fi
[ ! -f $archive_mc ] && echo $archive_mc no such file && exit 1
[ ! -f $archive_conf ] && echo $archive_conf no such file && exit 1
tar zxf $archive_mc
tar zxf $archive_conf

echo patching DisjointDEC
sed -i 's/throw(INTERP_KERNEL::Exception)//' $(find $medcoupling -name  DisjointDEC.hxx )
sed -i 's/throw(INTERP_KERNEL::Exception)//' $(find $medcoupling -name  DisjointDEC.cxx )

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
version=`python  -c "import sys; print (sys.version[:3])"`
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
  python -c "import medcoupling"
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
