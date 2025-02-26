#!/bin/bash

# Some clean due to the rename of lata2dx to lata_tools
if [ -d $TRUST_ROOT/exec/lata2dx_install ]
then
   rm -rf $TRUST_ROOT/exec/lata2dx_install $TRUST_ROOT/exec/lata2dx_reactor
   unlink $TRUST_ROOT/exec/lata2dx
   unlink $TRUST_ROOT/exec/compare_lata
fi

ORG=`pwd`
# Something changed -> we delete all
# Sometime TRUST_CC changes and produces recursive cmake so the last condition IS important
Build=$TRUST_ROOT/build/Outils/lata_tools
if [ build.sh -nt $Build ] || [ CMakeLists.txt -nt $Build ] || [ "`grep $TRUST_CC $Build/CMakeCache.txt`" = "" ]
then
   rm -r -f $Build
fi
mkdir -p $Build
cd $Build
# Better detection of SWIG on Ubuntu 16
SWIG_EXECUTABLE=`type -p swig`
# PL inutile de specifier swig (dans PATH normalement):
build_mode="Release"
if [ "x$exec" == "x$exec_debug" ]; then
   echo "*****************************************"
   echo "lata_tools will be built in DEBUG mode!!"
   echo "*****************************************"
   build_mode="Debug"
fi
if [ "$TRUST_CC_BASE_EXTP" != "" ] && [ "`basename $TRUST_CC_BASE`" != crayCC ] # Sur adastra GNU ici ne marche pas... tout comme medcoupling
then
   export OMPI_CXX=$TRUST_CC_BASE_EXTP
   export OMPI_CC=$TRUST_cc_BASE_EXTP
   export MPICH_CXX=$OMPI_CXX
   export MPICH_CC=$OMPI_CC
fi
[ "`basename $TRUST_CC_BASE`" != crayCC ] && export CXXFLAGS="-Wno-inconsistent-missing-override -Wno-deprecated-declarations" # to suppress warnings from medcoupling

if [[ $(uname -s) == "Darwin" ]]
then
   PYTHON_VERSION=`python -c "import sys; print(str(sys.version_info.major)+'.'+str(sys.version_info.minor))"`
   export CXXFLAGS="$CXXFLAGS -I${TRUST_ROOT}/exec/python/include/python${PYTHON_VERSION}"
   export LDFLAGS="$LDFLAGS -Wl,-flat_namespace -Wl,-w -L${TRUST_ROOT}/exec/python/lib -lpython${PYTHON_VERSION}"
fi

# CMake option + implicit use of TRUST_DEFINES and EXTRA_SRCS env var:
cmake_opt="-DCMAKE_BUILD_TYPE=$build_mode -DCMAKE_INSTALL_PREFIX=$TRUST_ROOT/exec/lata_tools -DCMAKE_C_COMPILER=$TRUST_cc "
cmake_opt="$cmake_opt -DCMAKE_CXX_COMPILER=$TRUST_CC -DSWIG_EXECUTABLE=$SWIG_EXECUTABLE "
cmake $ORG $cmake_opt || exit -1

make -j  $TRUST_NB_PROCS  install || exit -1

echo "Check if compare_lata works ..."
echo "   with LATA files 32b ..."
tar xzf $TRUST_ROOT/Outils/lata_tools/src/tests/vdf32.tar.gz
$TRUST_ROOT/exec/lata_tools/bin/compare_lata vdf32/vdf.lata vdf32/vdf.lata || exit -1
echo "   with LATA files 64b ..."
tar xzf $TRUST_ROOT/Outils/lata_tools/src/tests/upwind64.tar.gz
$TRUST_ROOT/exec/lata_tools/bin/compare_lata upwind64/upwind.lata upwind64/upwind.lata || exit -1
echo "   with LML files ..."
tar xzf $TRUST_ROOT/Outils/lata_tools/src/tests/upwind.tar.gz
$TRUST_ROOT/exec/lata_tools/bin/compare_lata upwind.lml upwind.lml || exit -1
if [ "$TRUST_MEDCOUPLING_ROOT" != "" ]; then
  echo "   with MED files ..."
  cp $TRUST_ROOT/Outils/lata_tools/src/tests/upwind_0000.med .
  $TRUST_ROOT/exec/lata_tools/bin/compare_lata upwind_0000.med upwind_0000.med || exit -1
fi
echo "   with FORT21 files ..."
$TRUST_ROOT/exec/lata_tools/bin/compare_lata $TRUST_ROOT/Outils/lata_tools/src/tools/FORT21 $TRUST_ROOT/Outils/lata_tools/src/tools/FORT21 || exit -1
$TRUST_ROOT/exec/lata_tools/bin/lata_analyzer $TRUST_ROOT/Outils/lata_tools/src/tools/FORT21 write_singlelata=testf21 || exit -1
nc=$(grep CHAMP testf21.lata | wc| awk '{print $1}')
[ $nc -ne 57 ] && echo "Invalid number of CHAMP ($nc != 57)" && exit -2

cd ..
rm -rf $Build
