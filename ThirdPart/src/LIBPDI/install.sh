#!/bin/bash
#
# PDI installation procedure
#

# Dev note: switch this to 1 if you want to build in debug mode and
# preserve the source and build directory.
debug_mode=0


if [ "x$TRUST_ROOT" = "x" ]; then
  echo TRUST_ROOT not defined!
  exit -1
fi

ARCHIVE=$1
[ ! -f $ARCHIVE ] && echo "$ARCHIVE not found! Update your makefile." && exit -1

# Path to library build
build_dir=$TRUST_ROOT/build/lib/LIBPDI
if [ "$debug_mode" = "0" ]; then
    rm -fr $build_dir
    mkdir -p $build_dir
fi
curr_dir=`dirname -- $( readlink -f -- "$0"; )`

# Path to library install
install_dir=$TRUST_PDI_ROOT
plugin_install_dir=$install_dir/lib64
rm -rf $install_dir
mkdir -p $install_dir

# Building library
cd $build_dir
if [ "$debug_mode" = "0" ]; then
    tar -xzvf $ARCHIVE
fi
pdi=`basename $ARCHIVE`
pdi_src=${pdi%.tar.gz}
cd $pdi_src

echo "@@@ Patching to accept correct build type ..."
sed -i 's/if(NOT "\${CMAKE_BUILD_TYPE}")/if(CMAKE_BUILD_TYPE STREQUAL "")/' CMakeLists.txt
echo "@@@ Patching to compile with static HDF5 from TRUST ..."
sed -i 's/set(HDF5_USE_STATIC_LIBRARIES OFF)/set(HDF5_USE_STATIC_LIBRARIES ON)/' plugins/decl_hdf5/CMakeLists.txt
echo "@@@ Patching to dynamically swicth to correct API version ..."
patch -p1 < $curr_dir/hdf5_err_handler.patch  || exit -1

mkdir -p build
cd build

if [ "$TRUST_CC_BASE_EXTP" != "" ]
then
   CC=$TRUST_cc_BASE_EXTP
   FC=$TRUST_F77_BASE_EXTP
else
   FC=$TRUST_F77
   CC=$TRUST_cc
fi

# configuration (we use the hdf5 of TRUST)
options="-DBUILD_BENCHMARKING=OFF -DBUILD_FORTRAN=OFF -DBUILD_DECL_NETCDF_PLUGIN=OFF -DBUILD_NETCDF_PARALLEL=OFF -DBUILD_TEST_PLUGIN=OFF -DBUILD_TESTING=OFF -DUSE_yaml=EMBEDDED"

[ "$TRUST_DISABLE_MPI" = 1 ] &&  options="$options -DBUILD_MPI_PLUGIN=OFF -DBUILD_HDF5_PARALLEL=OFF"

if [ "$debug_mode" != "0" ]; then
   options="$options -DCMAKE_BUILD_TYPE=Debug"
fi

env CC=$CC FC=$FC cmake .. -DCMAKE_PREFIX_PATH=$TRUST_HDF5_ROOT -DCMAKE_INSTALL_PREFIX=$install_dir -DINSTALL_PDIPLUGINDIR=$plugin_install_dir $options

# make & install
$TRUST_MAKE
make install
status=$?

if ! [ $status -eq 0 ]; then
  echo "@@@@@@@@@@@@@@@@@@@@@"
  echo "Error at compilation/install."
  exit -1
fi

# cleaning
if [ "$debug_mode" = "0" ]; then
    rm -rf $build_dir
fi

