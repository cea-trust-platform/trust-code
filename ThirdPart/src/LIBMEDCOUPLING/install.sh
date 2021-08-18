#!/bin/bash

#
# Installation script for MEDCoupling
#

archive_mc=$1
archive_conf=$2
tool_dir=`readlink -f $(dirname $0)`
src_dir=`basename $archive_mc .tar.gz`
mc_version=`echo $src_dir | sed 's/[^0-9]*\([0-9].[0-9].[0-9]\)/\1/'`
build_root=$TRUST_ROOT/build/lib
build_dir=$build_root/medcoupling_build
install_dir=$TRUST_MEDCOUPLING_ROOT
org=`pwd`

mkdir -p $install_dir
mkdir -p $build_dir
cd $build_dir

# include file:
medcoupling_hxx=$install_dir/include/medcoupling++.h

# MEDCoupling uses DataArrayInt32 not DataArrayInt64, so we disable MEDCoupling when building a 64 bits version of TRUST
# if [ "$TRUST_INT64" = "1" ]
# then
#     echo "@@@@@@@@@@@@ INT64 specific stuff ..."
#     mkdir -p $install_dir/include
#     rm -rf $install_dir/lib
#     echo "MEDCOUPLING DISABLE for 64 bits"
#     echo "#define NO_MEDFIELD " > $install_dir/include/ICoCoMEDField.hxx
#     echo "#undef MEDCOUPLING_"  > $medcoupling_hxx
#     exit 0
# fi

rm -f ICoCoMEDField.hxx
if [ "$TRUST_USE_EXTERNAL_MEDCOUPLING" = "1" ]; then
  echo "@@@@@@@@@@@@ Using external MEDCoupling: '$MEDCOUPLING_ROOT_DIR'"
  echo $MEDCOUPLING_ROOT_DIR
  ln -sf $MEDCOUPLING_ROOT_DIR $install_dir
  exit 0
fi

echo "@@@@@@@@@@@@ Unpacking ..."

cd $build_root

[ ! -f $archive_mc ] && echo $archive_mc no such file && exit 1
[ ! -f $archive_conf ] && echo $archive_conf no such file && exit 1
tar zxf $archive_mc
tar zxf $archive_conf

echo "@@@@@@@@@@@@ Configuring, compiling and installing ..."
cd $build_dir

OPTIONS=""
USE_MPI=ON && [ "$TRUST_DISABLE_MPI" -eq 1 ] && USE_MPI=OFF
# NO_CXX1 pour cygwin
OPTIONS="$OPTIONS -DNO_CXX11_SUPPORT=OFF"
USE_PYTHON=ON # API Python
# Warnings selon compilateurs:
if [ "`basename $TRUST_CC_BASE`" = nvc++ ]
then
   # Pour nvc++, suppression de 2 flags non supportes:
   find $build_root -name CMakeLists.txt | xargs sed -i "1,$ s?-Wsign-compare -Wconversion??g"
   USE_PYTHON=OFF # Crash sur JeanZay lors du build de l'API Python
   OPTIONS="$OPTIONS -DCMAKE_CXX_FLAGS=-D__GCC_ATOMIC_TEST_AND_SET_TRUEVAL=1" # Sur JeanZay error sinon
else
   OPTIONS="$OPTIONS -DCMAKE_CXX_FLAGS=-Wno-narrowing" # Error->Warning pour ARM
fi
# We use now python and SWIG from conda so:
OPTIONS="$OPTIONS -DMEDCOUPLING_USE_MPI=$USE_MPI -DSALOME_USE_MPI=$USE_MPI -DMPI_ROOT_DIR=$MPI_ROOT -DCMAKE_CXX_COMPILER=$TRUST_CC -DCMAKE_C_COMPILER=$TRUST_cc "
OPTIONS="$OPTIONS -DHDF5_ROOT_DIR=$TRUST_MED_ROOT  -DMEDFILE_ROOT_DIR=$TRUST_MED_ROOT -DMEDCOUPLING_BUILD_DOC=OFF  -DMEDCOUPLING_PARTITIONER_METIS=OFF "
OPTIONS="$OPTIONS -DMEDCOUPLING_PARTITIONER_SCOTCH=OFF -DMEDCOUPLING_ENABLE_RENUMBER=OFF -DMEDCOUPLING_ENABLE_PARTITIONER=OFF -DMEDCOUPLING_BUILD_TESTS=OFF "
OPTIONS="$OPTIONS -DMEDCOUPLING_WITH_FILE_EXAMPLES=OFF -DCONFIGURATION_ROOT_DIR=../configuration-$mc_version "
OPTIONS="$OPTIONS -DMEDCOUPLING_MEDLOADER_USE_XDR=OFF -DMEDCOUPLING_BUILD_STATIC=ON -DMEDCOUPLING_ENABLE_PYTHON=$USE_PYTHON"
#INT64
if [ "$TRUST_INT64" = "1" ]
then
    OPTIONS="$OPTIONS -DMEDCOUPLING_USE_64BIT_IDS=ON"
else
    OPTIONS="$OPTIONS -DMEDCOUPLING_USE_64BIT_IDS=OFF"
fi

echo "About to execute CMake -- options are: $OPTIONS"
echo "Current directory is : `pwd`"
cmake ../$src_dir $OPTIONS -DCMAKE_INSTALL_PREFIX=$install_dir -DCMAKE_BUILD_TYPE=Release

# The current CMake of MEDCoupling is badly written: dependencies on .pyc generation do not properly capture SWIG generated Python modules.
# So we need to do make twice ...
$TRUST_MAKE
make install
make install
status=$?

if ! [ $status -eq 0 ]; then
  echo "@@@@@@@@@@@@@@@@@@@@@"
  echo "Error at compilation."
  exit -1
fi

# Creation of env file. Done in a temporary file, because the final env.sh is the main target of the Makefile
# but we need an env file for the test below ... 
echo "@@@@@@@@@@@@ Creating env file ..."
MC_ENV_FILE_tmp=$install_dir/env_tmp.sh
MC_ENV_FILE=$install_dir/env.sh
#echo "export MED_COUPLING_ROOT=$install_dir"> $MC_ENV_FILE_tmp
echo "export LD_LIBRARY_PATH=\$TRUST_MEDCOUPLING_ROOT/lib:\$TRUST_MED_ROOT/lib:\$TRUST_ROOT/exec/python/lib:\$LD_LIBRARY_PATH" > $MC_ENV_FILE_tmp
echo "export PYTHONPATH=\$TRUST_MEDCOUPLING_ROOT/bin:\$TRUST_MEDCOUPLING_ROOT/lib/`cd $TRUST_MEDCOUPLING_ROOT/lib;find * -name site-packages`:\$PYTHONPATH" >> $MC_ENV_FILE_tmp
echo "@@@@@@@@@@@@ Testing install ..."
if [ $status -eq 0 ]  # install was successful
then
  ##
  ## Test de fonctionnement de l'API Python
  ##
  if [ "$USE_PYTHON" = ON ]
  then
     source $MC_ENV_FILE_tmp
     python -c "import medcoupling"  # also provides MEDLoader functionalities
     if [ $? -eq 0 ]
     then
        echo "medcoupling library OK"
    # Clean build folder
    cd .. ; rm -rf configuration* medcoupling*
     else
        echo "medcoupling library KO"
        exit -1
     fi
  fi
fi

echo "@@@@@@@@@@@@ Updating TRUST include files ..."
touch $install_dir/include/*

if [ "$TRUST_INT64" = "1" ]
then
    [ ! -f $medcoupling_hxx ]  && printf "#define MEDCOUPLING_\n#define MEDCOUPLING_USE_64BIT_IDS" > $medcoupling_hxx
else
    [ ! -f $medcoupling_hxx ]  && echo "#define MEDCOUPLING_" > $medcoupling_hxx
fi

# Update env file:
mv $MC_ENV_FILE_tmp $MC_ENV_FILE

echo "All done!"


