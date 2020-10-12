#!/bin/bash
#
# MED file installation procedure
#

if [ "x$TRUST_ROOT" = "x" ]; then
  echo TRUST_ROOT not defined!
  exit -1
fi

archive=$1
install_dir=$TRUST_MED_ROOT

###############################
# Compilation and installation
###############################
if [ "x$TRUST_USE_EXTERNAL_MED" = "x" ]; then
  echo "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@"
  echo "Installing MED file from externalpackages : $archive"
  echo "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@"

  build_root=$TRUST_ROOT/build/lib
  #build_dir=$build_root/med_build
  actual_install_dir=$install_dir/med_install

  #mkdir -p $build_dir
  mkdir -p $actual_install_dir
  cd $build_root
  
  echo "Un-packing ..."
  tar xzf $archive
  if ! [ $? -eq 0 ]; then
    echo "MED installation: archive '$archive' could not be found or could not be extracted!!!"
    exit -1
  fi
  
  archive_short=$(basename $archive)
  src_dir=${archive_short%.tar.gz}
  cd $src_dir
  echo "Patching (to avoid doc ...)"
  modified_file=Makefile.in
  sed -i "s?tests tools doc?tools?g" $modified_file

  echo "Patching header file to avoid compilation error"
  sed -i "s/extern MEDC_EXPORT const char \* const  MEDget/extern MEDC_EXPORT const char *  MEDget/g"  $(find . -name med.h.in)
  sed -i "s/const char \* const  MEDget/const char * MEDget/g"  $(find . -name MEDiterators.c)
  # patching med to avoid duplicated #include "_MEDfieldValueUpdateEntityList41.c"
  sed -i '30d' $(find . -name MAJ_400_410_champs.c)

  echo "Configuring with autotools  ..."  # [ABN] CMake is there too in MED, but for how long?? Eric prefers autotools ...
  # fPIC is not there by default in MED autotools ...
  CFLAGS="${CFLAGS} -fPIC"
  CPPFLAGS="${CPPFLAGS} -fPIC"
  export CFLAGS CPPFLAGS

  # Options: no Python, static libraries and path to HDF5 
  options="--enable-static --disable-python --enable-installtest --with-hdf5=$TRUST_MED_ROOT"  # TRUST_MED_ROOT is also HDF5 root ...
  #INT64
  if [ "$TRUST_INT64" = "1" ]
  then
      options="$options  --with-med_int=long"
  fi
  # Ajout de python/lib car parfois zlib pas installe sur la machine (Ubuntu 20)
  LDFLAGS="" && [ ! -f /usr/lib64/libz.so ] && LDFLAGS="LDFLAGS=-L$TRUST_ROOT/exec/python/lib"
  env $LDFLAGS CC=$TRUST_cc CXX=$TRUST_CC F77=$TRUST_F77 ./configure --prefix="$actual_install_dir" $options #   For debug, add:   CFLAGS="-g -O0" CXXFLAGS="-g -O0"
  
  $TRUST_MAKE  || exit -1
  make install || exit -1

  # Clean build folder
  (cd .. ; rm -rf med*)
else  
  if ! [ -d "$TRUST_USE_EXTERNAL_MED" ]; then
    echo "Variable TRUST_USE_EXTERNAL_MED has been defined but points to an invalid directory: $TRUST_USE_EXTERNAL_MED"
    exit -1
  fi
  actual_install_dir=$TRUST_USE_EXTERNAL_MED
fi

#################
# Links creation
#################
# If we point to an external installation the 'actual_install_dir' variable will point to the right place
echo "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@"
echo "Creating MED file links ..."
echo "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@"

for ze_dir in bin lib include; do
  mkdir -p $install_dir/$ze_dir
  cd $install_dir/$ze_dir
  for f in $actual_install_dir/$ze_dir/*; do
    link_name=$(basename $f)
    ln -nsf $f $link_name || exit -1
  done  
done

echo "All done !"


