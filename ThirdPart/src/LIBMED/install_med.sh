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
  sed -i 's/GET_PROPERTY(_lib_lst TARGET hdf5 PROPERTY IMPORTED_LINK_INTERFACE_LIBRARIES_NOCONFIG)/SET(_lib_lst)/' $(find . -name FindMedfileHDF5.cmake)

  # fPIC is not there by default in MED autotools ...
  Wno="-Wno-error -Wno-implicit-function-declaration"
  CFLAGS="${CFLAGS} -fPIC $Wno"
  CPPFLAGS="${CPPFLAGS} -fPIC $Wno"
  CXXFLAGS="${CXXFLAGS} -fPIC $Wno"
  FFLAGS="${FFLAGS} -fPIC"

  if [ "$TRUST_CC_BASE_EXTP" != "" ]
  then
     CXX=$TRUST_CC_BASE_EXTP
     CC=$TRUST_cc_BASE_EXTP
     FC=$TRUST_F77_BASE_EXTP
  else
     CXX=$TRUST_CC
     CC=$TRUST_cc
     FC=$TRUST_F77
     #LDFLAGS from TRUST
     fic_env=$TRUST_ROOT/env/make.$TRUST_ARCH_CC`[ "$debug" = "0" ] && echo _opt`
     LDFLAGS=`$TRUST_Awk '/SYSLIBS =/ {gsub("SYSLIBS =","",$0);print $0}' $fic_env`
  fi
  export CFLAGS CPPFLAGS CXXFLAGS FFLAGS LDFLAGS

  echo "Configuring with CMake ..."
  mkdir BUILD
  cd BUILD
  USE_MPI=ON && [ "$TRUST_DISABLE_MPI" -eq 1 ] && USE_MPI=OFF
  MED_INT=int
  if [ "$TRUST_INT64" = "1" ]
  then
     fic_env=$TRUST_ROOT/env/make.$TRUST_ARCH_CC$OPT
     [ "`$TRUST_Awk '/-fdefault-integer-8/ {print $0}' $fic_env`" != "" ] && export FFLAGS="${FFLAGS} -fdefault-integer-8"
     [ "`$TRUST_Awk '/-i8/ {print $0}' $fic_env`" != "" ] && export FFLAGS="${FFLAGS} -i8"
     MED_INT=long
  fi
  [ `uname -s` = Darwin ] && DARWIN_FLAGS="-DCMAKE_OSX_DEPLOYMENT_TARGET=$(sw_vers -productVersion)"
  echo "Setting FFLAGS=$FFLAGS and MED_INT=$MED_INT ..."
  env CC=$CC CXX=$CXX F77=$FC FC=$FC cmake ..  -DCMAKE_INSTALL_PREFIX="$actual_install_dir" -DMEDFILE_BUILD_STATIC_LIBS=ON -DMEDFILE_BUILD_SHARED_LIBS=OFF \
      -DMEDFILE_INSTALL_DOC=OFF -DMEDFILE_BUILD_PYTHON=OFF -DHDF5_ROOT_DIR=$TRUST_MED_ROOT/hdf5_install -DMEDFILE_USE_MPI=$USE_MPI -DMED_MEDINT_TYPE=$MED_INT \
      -DCMAKE_SHARED_LINKER_FLAGS="$LDFLAGS" -DMEDFILE_BUILD_TESTS=OFF $DARWIN_FLAGS

  $TRUST_MAKE  || exit -1
  make install || exit -1
  cd ..

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
[ "x$TRUST_USE_EXTERNAL_MED" = "x" ] && actual_install_dir=../`basename $actual_install_dir` # Pour creation de chemins relatifs
for ze_dir in bin lib include; do
  mkdir -p $install_dir/$ze_dir
  cd $install_dir/$ze_dir
  for f in $actual_install_dir/$ze_dir/*; do
    link_name=$(basename $f)
    ln -nsf $f $link_name || exit -1
  done  
done

echo "All done !"


