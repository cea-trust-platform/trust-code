#!/bin/bash

# Installation script for gtest

archive_gtest=$1
src_dir_rel=$(basename $archive_gtest .tar.gz)
build_root=$TRUST_ROOT/build/lib
build_dir=$build_root/gtest_build
install_dir=$TRUST_GTEST_ROOT
org=$(pwd)

# Ensure required directories exist
rm -rf $build_dir
mkdir -p "$install_dir"
mkdir -p "$build_dir"

echo "@@@@@@@@@@@@ Unpacking ..."
cd "$build_root"

# Check if the archive file exists before extracting
if [ ! -f "$archive_gtest" ]; then
  echo "$archive_gtest: no such file"
  exit 1
fi

tar zxf "$archive_gtest" || exit 1

echo "@@@@@@@@@@@@ Configuring, compiling and installing ..."
cd "$build_dir"

src_dir="$build_root/$src_dir_rel"

OPTIONS=""

if [ "$TRUST_CC_BASE_EXTP" != "" ] && [ "$TRUST_USE_CUDA" = 1 ]
then
   OPTIONS="-DCMAKE_C_COMPILER=$TRUST_cc_BASE_EXTP -DCMAKE_CXX_COMPILER=$TRUST_CC_BASE_EXTP"
else
   OPTIONS="-DCMAKE_C_COMPILER=$TRUST_cc_BASE -DCMAKE_CXX_COMPILER=$TRUST_CC_BASE"
fi

# Run CMake to configure the project
cmake "$src_dir" $OPTIONS -DCMAKE_INSTALL_PREFIX="$install_dir" -DCMAKE_BUILD_TYPE=Release || exit 1

# Compile and install
$TRUST_MAKE || exit 1
make install || exit 1

echo "gtest - All done!"
