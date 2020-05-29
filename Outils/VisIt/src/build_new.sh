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
conda=$TRUST_ROOT/exec/python
options="--tarball $src"					# Construction de VisIt depuis les sources
options=$options" --prefix $TRUST_ROOT/exec/VisIt"		# Repertoire installation
options=$options" --cc $TRUST_cc_BASE"				# Compilateur C
options=$options" --cxx $TRUST_CC_BASE"				# Compilateur C++
options=$options" --fc $TRUST_F77_BASE" 			# Compilateur Fortran
options=$options" --makeflags -j$TRUST_NB_PROCS"		# Parallel build
options=$options" --thirdparty-path $TRUST_TMP" 		# Pour partage par plusieurs installations TRUST des 3rd party
options=$options" --system-cmake --alt-cmake-dir $conda" 	# Cmake de Miniconda
options=$options" --system-qt --alt-qt-dir $conda" 		# Qt5 de Miniconda
options=$options" --no-sphinx"					# Disable pour eviter l'installation de Python3 et pas mal de modules...
#options=$options" --system-python --alt-python-dir $conda" 	# VisIt ne supporte pas Python3 systeme
if [ "$build_parallel" != "" ]
then
   export PAR_COMPILER=$TRUST_cc
   export PAR_COMPILER_CXX=$TRUST_CC
   export PAR_INCLUDE=-I$MPI_INCLUDE
   export PAR_LIBS=""
   options=$options" --parallel"			# Construction de VisIt parallele
   options=$options" --osmesa --llvm"			# OsMesa3D en // pour faster rendering (LLVM prerequis de OsMesa3D)
   options=$options" --no-icet"				# Car icet (optimisation du rendering en //) ne compile pas
fi

########
# Hacks:
########
unset OPT 												# Sinon numpy plante a la compilation car $OPT utilise...
pkg=xorg-libsm && [ "`conda list $pkg | grep 1.2.3`" = "" ] && conda install -c conda-forge $pkg -y	# Sinon /usr/bin/ld: /usr/lib64/libSM.so: undefined reference to `uuid_generate@UUID_1.0'
VISIT_QT_DIR="VISIT_OPTION_DEFAULT(VISIT_QT_DIR \${QT_BIN_DIR}/..)"					# VISIT_QT_DIR non fixe lors du build de VisIt
if [ "`grep "$VISIT_QT_DIR" $build`" = "" ]
then
   sed -i "1,$ s?VISIT_OPTION_DEFAULT(VISIT_QT_BIN?$VISIT_QT_DIR\nVISIT_OPTION_DEFAULT(VISIT_QT_BIN?" $build	
fi
mkdir -p $TRUST_ROOT/exec/VisIt  									# Important sinon current pas cree au premier make install

########
# Build:
########
echo "Creating `pwd`/$build.help ..."
./$build --help 1>./$build.help 2>&1 
log=build_visit$vt"_log"
echo -e "yes\n" | ./$build $options
err=$?
# Nouvel essai avec des options supplementaires:
if [ "`grep 'mesagl to the command line' $log`" != "" ]
then
   options=$options" --mesagl --llvm" 			# Si machine virtuelle (pas teste) ou OpenGL trop vieux (ex: is213120)
   echo -e "yes\n" | ./$build $options
   err=$?
fi
if [ $err != 0 ]
then
   echo "$build_visit failed in `basename $0`. See $log"
   exit -1
else
   echo "$build_visit succeeds."
   rm -r -f visit$vp 		# Sources inutiles une fois le build construit
   rm -f visit$vt*tar.gz 	# Package construit et installe par le build (inutile de le garder) 
   exit 0
fi
