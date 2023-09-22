#!/bin/bash
# PL: Reecrit depuis VisIt 3.1.1
clean()
{
   # Clean plus necessaire
   return
}

#########
# Options
#########
[ "$1" = "" ] && echo "Usage: `basename $0` clean | package [parallel]" && exit -1
package=$1 && [ "$package" = clean ] && clean && exit 0
build_parallel=$2
# Variables:
vp=`echo $package | $TRUST_Awk -F. '{gsub("visit","",$1);gsub("_",".",$1);print $1}'`
vt=`echo $vp | $TRUST_Awk '{gsub("\\\.","_",$1);print $1}'`
src=visit$vp.tar.gz
build=build_visit$vt

###########################
# Recuperation des packages
###########################
for file in `ls ../../../externalpackages/VisIt/*`
do
   ln -sf $file .
done

###############
# Configuration
###############
#conda=$TRUST_ROOT/exec/python
#with_conda=0 && [ -f $TRUST_ROOT/exec/python/bin/conda ] && with_conda=1
options="--tarball $src"					# Construction de VisIt depuis les sources
options=$options" --prefix $TRUST_ROOT/exec/VisIt"		# Repertoire installation
options=$options" --cc $TRUST_cc"				# Compilateur C
options=$options" --cxx $TRUST_CC"				# Compilateur C++
options=$options" --fc $TRUST_F77"				# Compilateur Fortran
options=$options" --makeflags -j$TRUST_NB_PROCS"		# Parallel build
options=$options" --thirdparty-path $TRUST_TMP"			# Pour partage par plusieurs installations TRUST des 3rd party
options=$options" --no-sphinx"					# Disable pour eviter l'installation de Python3 et pas mal de modules...
options=$options" --qt"                                         #
#if [ "$with_conda" = "1" ]
#then
#   options=$options" --system-cmake"				# we use system cmake de Miniconda; sauf pour topaze ou on prend cmake 3.22 module
#   options=$options" --system-qt --alt-qt-dir $conda" 		# Qt5 de Miniconda
#   options=$options" --system-python --alt-python-dir $conda" 	# VisIt 3.3.3 supporte Python3.7
#else
    # VisIt 3.3.3 ne s'installe pas si python>=3.8 et si qt>=5.15, on demande a visit de les installer
    options=$options" --python --cmake --qt"				# VisIt installe son python (3.7) et son qt (5.14).
#fi
if [ "$build_parallel" != "" ]
then
   #export PAR_COMPILER=$TRUST_cc
   #export PAR_COMPILER_CXX=$TRUST_CC
   #export PAR_INCLUDE=-I$MPI_INCLUDE
   #export PAR_LIBS=""
   options=$options" --parallel"			# Construction de VisIt parallele
   options=$options" --osmesa --llvm"			# OsMesa3D en // pour faster rendering (LLVM prerequis de OsMesa3D)
   options=$options" --no-icet"				# Car icet (optimisation du rendering en //) ne compile pas
fi

########
# Hacks:
########
unset OPT 												# Sinon numpy plante a la compilation car $OPT utilise...
VISIT_QT_DIR="VISIT_OPTION_DEFAULT(VISIT_QT_DIR \${QT_BIN_DIR}/..)"					# VISIT_QT_DIR non fixe lors du build de VisIt
if [ "`grep "$VISIT_QT_DIR" $build`" = "" ]
then
   sed -i "1,$ s?VISIT_OPTION_DEFAULT(VISIT_QT_BIN?$VISIT_QT_DIR\nVISIT_OPTION_DEFAULT(VISIT_QT_BIN?" $build	
fi

# List of supplementary patches applied for VisIt 3.3.3 :
# 1. Python-3.7.7.tgz when compiled with mpicc runs into error (adds intel flag), in function apply_python_patch :
#        # Apply patch for python if built with mpicc instead of gcc/icc
#        sed -i 's/^case "$CC" in/case `basename "$CC"` in/' configure
#        sed -i 's/^\*icc\*)/icc)/' configure
# 2. in function build_mesagl, patch needed for gcc 10:
#         sed -i "s/struct util_hash_table/static struct util_hash_table/" src/gallium/auxiliary/util/u_debug_symbol.c
# 3. in function build_osmesa, patch needed for gcc 10:
#         sed -i "s/struct util_hash_table/static struct util_hash_table/" src/gallium/auxiliary/util/u_debug_symbol.c
# 
# 4. apply_qt_5142_numeric_limits_patch and apply_qt_5142_cmath_patch fail on irene/topaze, we do not exit:
#     elif [[ ${QT_VERSION} == 5.14.2 ]] ; then
#        apply_qt_5142_numeric_limits_patch
#     +  echo "skipped, patch KO on irene"
#        if [[ $? != 0 ]] ; then
#            return 1
#        fi
#        apply_qt_5142_cmath_patch
#     +  echo "skipped, patch KO on irene"
#        if [[ $? != 0 ]] ; then
#            return 1
#        fi
 
mkdir -p $TRUST_ROOT/exec/VisIt  									# Important sinon current pas cree au premier make install

########
# Build:
########
echo "Creating `pwd`/$build.help ..."
./$build --help 1>./$build.help 2>&1 
log=build_visit$vt"_log"
echo -e "yes\n" | ./$build $options 1>>$log 2>&1
err=$?
# Nouvel essai avec des options supplementaires:
if [ "`grep 'mesagl to the command line' $log`" != "" ]
then
   options=$options" --mesagl --llvm" 			# Si machine virtuelle (pas teste) ou OpenGL trop vieux (ex: is213120)
   echo "Retry with $options"
   echo -e "yes\n" | ./$build $options 1>>$log 2>&1
   err=$?
fi
echo "========================================"
if [ $err != 0 ]
then
   echo "$build failed in `basename $0`. See $log"
   exit -1
else
   echo "$build succeeds."
   rm -r -f visit$vp 		# Sources inutiles une fois le build construit
   rm -f visit$vt*tar.gz 	# Package construit et installe par le build (inutile de le garder) 
   exit 0
fi

#patch src/mesa-17.3.9/src/gallium/auxiliary/util/u_debug_symbol.c
#sed -i "s/struct util_hash_table/static struct util_hash_table/" src/mesa-17.3.9/src/gallium/auxiliary/util/u_debug_symbol.c

