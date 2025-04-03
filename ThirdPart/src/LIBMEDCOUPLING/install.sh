#!/bin/bash
#
# Installation script for MEDCoupling
#

# Dev note: switch this to 1 if you want to build in debug mode and
# preserve the source and build directory.
# For MEDCoupling you may also set the TRUST_ENABLE_MC_DEBUG env variable.
debug_mode=0
[ "$TRUST_ENABLE_MC_DEBUG" != "0" ] && debug_mode=1


archive_mc=$1
archive_conf=$2
tool_dir=`readlink -f $(dirname $0)`
src_dir_rel=`basename $archive_mc .tar.gz`
mc_version=`echo $src_dir_rel | sed 's/[^0-9]*\([0-9]\+.[0-9]\+.[0-9]\+\)/\1/'`
build_root=$TRUST_ROOT/build/lib
build_dir_root=$build_root/medcoupling_build
install_dir_root=$TRUST_MEDCOUPLING_ROOT
org=`pwd`

with_python=ON    # API Python

###################################################
# Build, install and test a variant of MEDCoupling. 
# Following variables must be set
#   rel_type : either 'Release' or 'Debug'
#   use_python : either ON or OFF
###################################################
build_and_test_mc()
{
    mkdir -p $install_dir
    mkdir -p $build_dir
    cd $build_dir

    echo "@@@@@@@@@@@@ Unpacking ..."

    cd $build_root

    [ ! -f $archive_mc ] && echo $archive_mc no such file && exit 1
    [ ! -f $archive_conf ] && echo $archive_conf no such file && exit 1
    tar zxf $archive_mc
    tar zxf $archive_conf
    
    echo "@@@@@@@@@@@@ Configuring, compiling and installing ..."
    cd $build_dir
    src_dir=../../$src_dir_rel
    
    OPTIONS=""
    USE_MPI=ON && [ "$TRUST_DISABLE_MPI" -eq 1 ] && USE_MPI=OFF
    CXX=$TRUST_CC
    CC=$TRUST_cc
    if [ "$TRUST_CC_BASE_EXTP" != "" ] && [ "`basename $TRUST_CC_BASE`" != crayCC ] # Sur adastra GNU ici ne marche pas...
    then
       OMPI_CXX=$TRUST_CC_BASE_EXTP
       OMPI_CC=$TRUST_cc_BASE_EXTP
       MPICH_CXX=$OMPI_CXX
       MPICH_CC=$OMPI_CC
    fi
    # -Wno-sign-conversion for intel oneAPI 2023, otherwise medcoupling build fails
    if [ `uname -s` = Darwin ]
    then
        export CXXFLAGS="-w -Wno-narrowing -Wno-sign-conversion" # deactivate warnings
    else
        export CXXFLAGS="-Wno-narrowing -Wno-sign-conversion"
    fi

    if [[ $use_python == ON && $(uname -s) == "Darwin" ]]
    then
       #PYTHON_VERSION=`python -c "import sys; print(str(sys.version_info.major)+'.'+str(sys.version_info.minor))"`
       #export CXXFLAGS="$CXXFLAGS -I${TRUST_ROOT}/exec/python/include/python$PYTHON_VERSION"
       export LDFLAGS="$LDFLAGS -Wl,-undefined,dynamic_lookup"
    fi

    echo "Applying patch for configuration to use static HDF5 ..."
    (cd ../../configuration* ; sed -i "s/FIND_PACKAGE(HDF5)/set(HDF5_USE_STATIC_LIBRARIES ON)\nFIND_PACKAGE(HDF5)/g" cmake/FindSalomeHDF5.cmake ) || exit -1

    echo "Applying patch for iterators ..."
    (cd $src_dir; patch -p1 -f < $TRUST_ROOT/ThirdPart/src/LIBMEDCOUPLING/iterator.patch ) || exit -1
    echo "Applying patch for Apple ..."
    (cd $src_dir; patch -p1 -f < $TRUST_ROOT/ThirdPart/src/LIBMEDCOUPLING/apple.patch ) || exit -1


    # Better detection of SWIG on Ubuntu 16
    SWIG_EXECUTABLE=`type -p swig`
    # We use now python from conda so:
    OPTIONS="$OPTIONS -DMEDCOUPLING_USE_MPI=$USE_MPI -DSALOME_USE_MPI=$USE_MPI -DMPI_ROOT_DIR=$MPI_ROOT -DCMAKE_CXX_COMPILER=$CXX -DCMAKE_C_COMPILER=$CC "
    OPTIONS="$OPTIONS -DHDF5_ROOT_DIR=$TRUST_HDF5_ROOT -DMEDFILE_ROOT_DIR=$TRUST_MED_ROOT -DMEDCOUPLING_BUILD_DOC=OFF -DMEDCOUPLING_PARTITIONER_METIS=OFF "
    OPTIONS="$OPTIONS -DMEDCOUPLING_PARTITIONER_SCOTCH=OFF -DMEDCOUPLING_ENABLE_RENUMBER=OFF -DMEDCOUPLING_ENABLE_PARTITIONER=OFF -DMEDCOUPLING_BUILD_TESTS=OFF "
    OPTIONS="$OPTIONS -DMEDCOUPLING_WITH_FILE_EXAMPLES=OFF -DCONFIGURATION_ROOT_DIR=../../configuration-$mc_version -DSWIG_EXECUTABLE=$SWIG_EXECUTABLE"
    OPTIONS="$OPTIONS -DMEDCOUPLING_MEDLOADER_USE_XDR=OFF -DMEDCOUPLING_BUILD_STATIC=ON -DMEDCOUPLING_ENABLE_PYTHON=$use_python -DPYTHON_ROOT_DIR=${TRUST_ROOT}/exec/python"
    #INT64 management:
    if [ "$TRUST_INT64" = "1" ]
    then
        OPTIONS="$OPTIONS -DMEDCOUPLING_USE_64BIT_IDS=ON"
    else
        OPTIONS="$OPTIONS -DMEDCOUPLING_USE_64BIT_IDS=OFF"
    fi

    echo "About to build MEDCoupling in mode: $rel_type"
    echo "About to execute CMake -- options are: $OPTIONS"
    echo "Current directory is : `pwd`"
    
    cmake $src_dir $OPTIONS -DCMAKE_INSTALL_PREFIX=$install_dir -DCMAKE_BUILD_TYPE=$rel_type || exit -1
    
    # The current CMake of MEDCoupling is badly written: dependencies on .pyc generation do not properly capture SWIG generated Python modules.
    # So we need to do make twice ...
    $TRUST_MAKE
    make install || exit -1
    make install

    echo "@@@@@@@@@@@@ Updating TRUST include files ..."
    touch $install_dir/include/*
    
    #####
    # Generate include file for TRUST:
    #####
    medcoupling_hxx=$install_dir/include/medcoupling++.h
    if [ "$TRUST_INT64" = "1" ]
    then
        [ ! -f $medcoupling_hxx ]  && printf "#define MEDCOUPLING_\n#define MEDCOUPLING_USE_64BIT_IDS" > $medcoupling_hxx
    else
        [ ! -f $medcoupling_hxx ]  && echo "#define MEDCOUPLING_" > $medcoupling_hxx
    fi
    
    # patch MEDCouplingConfig.cmake to not have absolute paths for MED and HDF5 inside it
    sed -i "s@$TRUST_ROOT@\${PACKAGE_PREFIX_DIR}/../../../..@g" $install_dir/cmake_files/MEDCouplingConfig.cmake
}


###################################################
##
## Main
##
###################################################
rm -f ICoCoMEDField.hxx
if [ "$TRUST_USE_EXTERNAL_MEDCOUPLING" = "1" ]; then
  echo "@@@@@@@@@@@@ Using external MEDCoupling: '$MEDCOUPLING_ROOT_DIR'"
  echo $MEDCOUPLING_ROOT_DIR
  ln -sf $MEDCOUPLING_ROOT_DIR $install_dir
  exit 0
fi

#####
# Build debug version first if requested:
#####
if [ "$debug_mode" != "0" ] || [ $HOST = $TRUST_HOST_ADMIN ] ; then
  rel_type="Debug"
  build_dir=$build_dir_root/${TRUST_ARCH}
  install_dir=$install_dir_root/${TRUST_ARCH}
  use_python=$with_python
  build_and_test_mc
else
  # Clean previous Debug version if $debug_mode set to 0:
  rm -rf $install_dir_root/${TRUST_ARCH}
fi

#####
# Build and install Release version whatever the case:
#####
rel_type="Release"
build_dir=$build_dir_root/${TRUST_ARCH}_opt
install_dir=$install_dir_root/${TRUST_ARCH}_opt
use_python=$with_python
build_and_test_mc

#####
# Update sym links
#####
for mkf in `cd $TRUST_ENV;\ls make.$TRUST_ARCH_CC*`; do
    TRUST_ARCH_OPT=$TRUST_ARCH${mkf#make.$TRUST_ARCH_CC}
    # Point to optim version if not there
    if [ ! -d $TRUST_MEDCOUPLING_ROOT/$TRUST_ARCH_OPT/lib ]; then
        (cd $TRUST_MEDCOUPLING_ROOT; ln -nsf ${TRUST_ARCH}_opt $TRUST_ARCH_OPT)
    fi
done

# Update include link - always pointing to opt includes:
(cd $TRUST_MEDCOUPLING_ROOT; ln -nsf ${TRUST_ARCH}_opt/include include)
# Creation of env file. Done in a temporary file, because the final env.sh is the main target of the Makefile
# but we need an env file for the test below ... 
echo "@@@@@@@@@@@@ Creating env file ..."
MC_ENV_FILE_tmp=$install_dir/env_tmp.sh  # install_dir is the Release path
MC_ENV_FILE=$install_dir/../env.sh
MC_DEBUG_ENV_FILE=$install_dir/../env_debug.sh && rm -f $MC_DEBUG_ENV_FILE
#echo "export MED_COUPLING_ROOT=$install_dir"> $MC_ENV_FILE_tmp
echo "export LD_LIBRARY_PATH=\$TRUST_MEDCOUPLING_ROOT/\${TRUST_ARCH}_opt/lib:\$LD_LIBRARY_PATH" > $MC_ENV_FILE_tmp
site_pkg=`cd $TRUST_MEDCOUPLING_ROOT/\${TRUST_ARCH}\${OPT}/lib;find * -name site-packages`
echo "export PYTHONPATH=\$TRUST_MEDCOUPLING_ROOT/\${TRUST_ARCH}_opt/bin:\$TRUST_MEDCOUPLING_ROOT/\${TRUST_ARCH}_opt/lib/${site_pkg}:\$PYTHONPATH" >> $MC_ENV_FILE_tmp
echo "export PATH=\$TRUST_HDF5_ROOT/hdf5_install/bin:\$TRUST_MED_ROOT/med_install/bin:\$PATH" >> $MC_ENV_FILE_tmp

##
## Test de fonctionnement de l'API Python (en optim seulement)
##
if [ "$with_python" = ON ]
then
   echo "@@@@@@@@@@@@ Testing install ..."
   source $MC_ENV_FILE_tmp
   python -c "import medcoupling"  # also provides MEDLoader functionalities
   if [ $? -eq 0 ]
   then
      echo "medcoupling library OK"
      # Clean build folder
      (cd $build_root ; rm -rf configuration* medcoupling*)
   else
      echo "medcoupling library KO"
      exit -1
   fi
fi


# Update env file:
mv $MC_ENV_FILE_tmp $MC_ENV_FILE
if [ -L $TRUST_MEDCOUPLING_ROOT/${TRUST_ARCH} ]
then
   echo "echo \"@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@\"" > $MC_DEBUG_ENV_FILE
   echo "echo \"Caution: MEDCoupling not compiled in Debug mode\"" >> $MC_DEBUG_ENV_FILE
   echo "echo \"@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@\"" >> $MC_DEBUG_ENV_FILE
fi
cat $MC_ENV_FILE | sed "s?\${TRUST_ARCH}_opt?\${TRUST_ARCH}?g" >> $MC_DEBUG_ENV_FILE

echo "All done!"

