#!/bin/bash
clean()
{
# return
   # Nettoyage des paquets
   echo $ECHO_OPTS "Cleaning...\c"
   # 
   for paquet in `\ls *.tar.gz *.tgz *.zip *.tar.bz2 2>/dev/null `
   do
      [ -d ${paquet%.tar.gz} ] && rm -r -f ${paquet%.tar.gz}
      [ -d ${paquet%.tgz} ]    && rm -r -f ${paquet%.tgz}
      [ -d ${paquet%.zip} ]    && rm -r -f ${paquet%.zip}
      [ -d ${paquet%.tar.bz2} ]    && rm -r -f ${paquet%.tar.bz2}
      rm -f $paquet
   done
   rm -r -f VTK-?.?.? VTK-*-build visit visit?.?.? visit-vtk-?.? visit-vtk-*-build Mesa-?.? Mesa-?.*.? qt-everywhere-opensource-src-?.?.?  
   rm -f build_visit2_5_2* build_visit*help
   rm -f build_visit*"_log" *.cmake *.conf tmp* *.py releases
   echo "OK"
}

# recuperation des packages
ln -sf $TRUST_ROOT/externalpackages/Python-* .

for file in `ls $TRUST_ROOT/externalpackages/VisIt/*`
do
ln -sf $file .
done

[ "$1" = "" ] && echo "Usage: `basename $0` clean | package [parallel]" && exit -1


package=$1 && [ "$package" = clean ] && clean && exit 0
option=$2
v=`echo $package | $TRUST_Awk -F. '{gsub("visit","",$1);gsub("_",".",$1);print $1}'`
version=`echo $v | $TRUST_Awk '{gsub("\\\.","_",$1);print $1}'`

####################################################
# Clean old install before install to avoid problems
####################################################
[ "`ls *visit2_5_2* 2>/dev/null`" != "" ] && ./build clean

#############################
# Verification du build_visit
#############################
build_visit=build_visit$version
if [ ! -f $build_visit ]
then
   CHECKOUT . 1>/dev/null 2>&1
   RM build_visit* 1>/dev/null 2>&1
   DOWNLOADED=0
   for HTTP in $HTTPS
   do
      [ $DOWNLOADED = 0 ] && echo "Try do download $HTTP/$v/$build_visit..." && wget_ $HTTP/$v/$build_visit 1>/dev/null 2>&1 && DOWNLOADED=1
   done   
   [ $DOWNLOADED = 0 ] && echo "Fail to download $build_visit" && exit -1
   chmod +x $build_visit  
   MKELEM $build_visit 1>/dev/null 2>&1
fi

###############################################
# Compilateurs choisis pour le build de VisIt #
###############################################
compilateurs="--cc $TRUST_cc --cxx $TRUST_CC"

#########################################################
# Les compilateurs Intel ne marchent pas toujours bien pour VisIt (plantages un peu incomprehensibles)
# Exemple curie: MakeFiles/vtkRenderingCore.dir/vtkMapper.cxx.o: file not recognized: File truncated
# On laisse faire VisIt (qui va prendre GNU) sauf sur certaines machines ou visiblement (callisto)
# il faut que plugin et VisIt soit compiles les 2 avec Intel
#########################################################
if [ ${TRUST_CC_BASE%icpc} != $TRUST_CC_BASE ]
then
   [ $HOST != callisto ] && compilateurs=""
fi

###################################################
# Cela ne marche pas bien aussi pour 3.4.6 (castor)
###################################################
if [ "$TRUST_VERSION_GNU" = 3.4.6 ]
then
   compilateurs=""
   # L'utilisation du cache semble poser probleme (.o not found parfois dans Mesa et Python)
   # Mais pour quand meme profiter de ccache:
   #if [ -d $TRUST_ROOT/exec/ccache/bin ]
   #then
   #   export PATH=$TRUST_ROOT/exec/ccache:$PATH
   #fi
fi
#################
# Mesa or not ? #
#################
mesa="--mesa"
# Zlib (libz.so is needed not libz.so.1...)
# Built also in gnuplot...
zlib="" && [ "`locate libz.so 2>/dev/null | grep ^/lib | grep lib.so$ 2>/dev/null`" = "" ] && [ "`ls /usr/lib*/libz.so 2>/dev/null`" = "" ] && zlib="--zlib"

###############
# Configuration
###############
# --python pour construire le module Python
# --console pour desactiver la construction via une IHM
# --parallel pour une construction du parallele
# --version pour dire quelle version construire
mkdir -p $TRUST_ROOT/exec/VisIt  # important sinon current pas cree au premier make install
prefix="--prefix $TRUST_ROOT/exec/VisIt" # Pour une installation directe
#options_build="$zlib --python --system-cmake --system-python --no-pyside --console $compilateurs $prefix"
# Using VisIt provided by cmake cause cmake 3.0.1 failed sometimes...
options_build="$zlib $mesa --python  --no-pyside --console $compilateurs $prefix"
# If Qt 4.8 detected, we use it (it works on sitre for example):
#module load qt 1>/dev/null 2>&1 # Trying to load Qt on CCRT
# optimization comment temporary, time to fix the Qt4 build failed with VisIt 2.8.2
#for qmake in `type -p qmake` /usr/lib64/qt4/bin/qmake /usr/bin/qmake
#do
#   if [ -f $qmake ] && [ "`$qmake -version 2>&1 | grep '4.8'`" != "" ]
#   then
#      options_build=$options_build" --system-qt"
#      break
#   fi
#done

# On ajoute le nombre de processeurs de la machine pour construire plus vite (make -j)
[ "$TRUST_NB_PROCS" != "" ] && options_build=$options_build" --makeflags -j$TRUST_NB_PROCS"

################################
# S'il manque des librairies dev 
################################
export MISSING_GL_DIR=$TRUST_ROOT/Outils/Gmsh/missing
# essai pour mpi3.1.4
export PROFILE_POSTLIB=-lpthread
if [ -d $MISSING_GL_DIR/lib ]
then
   echo "Using includes and librairies under \$TRUST_ROOT/Outils/Gmsh/missing..."
   # Pour MESA:
   export XLIBGL_CFLAGS="-I$MISSING_GL_DIR/include"
   export XLIBGL_LIBS="-L$MISSING_GL_DIR/lib -lXext"
   # Pour VTK:
   export CMAKE_INCLUDE_PATH=$MISSING_GL_DIR/include
   export CMAKE_LIBRARY_PATH=$MISSING_GL_DIR/lib
   # Pour QT:
   export EXTRA_QT_FLAGS="-I$MISSING_GL_DIR/include -L$MISSING_GL_DIR/lib -lXext"
   # Pour VisIt:
   export CFLAGS="-I$MISSING_GL_DIR/include"
   export CXXFLAGS="-I$MISSING_GL_DIR/include"
fi
# Installation plante sur is225222 en Fedora 18 avec OpenMPI 1.8.3
export CFLAGS="$CFLAGS -ldl"
export CXXFLAGS="$CXXFLAGS -ldl"
#export CXXFLAGS="$CXXFLAGS -ldl -Wno-narrowing -Wno-deprecated-declarations -std=gnu++98" 
export CXXFLAGS="$CXXFLAGS -std=gnu++98" 

if [ "$option" = parallel ]
then
   options_build=$options_build" --parallel"
   # export PAR_COMPILER=$TRUST_CC  # Better to use PAR_INCLUDE and PAR_LIBS
   # because it is either PAR_COMPILER or PAR_INCLUDE/PAR_LIBS
   # Le PAR_INCLUDE est important en cas de compilation MPICH pour IceT...
   export PAR_INCLUDE="-I$MPI_INCLUDE"
   # On essaie de recuperer les infos pour linker
   export PAR_LIBS=`$TRUST_CC --showme:link 2>/dev/null`
   if [ "$PAR_LIBS" = "" ]
   then
      # Sur d'autres machines c'est link_info et il faut enlever le compilateur:
      export PAR_LIBS=`$TRUST_CC -link_info 2>/dev/null`
      PAR_LIBS=${PAR_LIBS#$TRUST_CC_BASE}
      if [ "$PAR_LIBS" = "" ]
      then
         # Sur d'autres machines (supermuc) c'est -show et il faut enlever le compilateur:
         export PAR_LIBS=`$TRUST_CC -show 2>/dev/null`
         PAR_LIBS=${PAR_LIBS#$TRUST_CC_BASE}
         if [ "$PAR_LIBS" = "" ]
         then
	    echo "PAR_LIBS not found in `pwd`/build"
	    exit -1
         fi
      fi
   fi
else
  # si pas parallele on ne prend pas le compilo para de TRUST
 [ "$compilateurs" != "" ]  && compilateurs="--cc $TRUST_cc_BASE --cxx $TRUST_CC_BASE"
fi

#############
# Nettoyage #
#############
#clean
# To avoid an obscure recursive cmake shit prior to cmake 3.0 in VTK when compiler changes,
# we clean VTK and IceT builds
rm -r -f VTK*-build IceT-?-?-?
#if [ -d VTK-6.1.0 ]
#then
#echo patch VTK gcc6 
#cp VTK-6.1.0_patch_gcc6/CMake/* VTK-6.1.0/CMake
#fi

############################################
# Modification eventuelle du script de build
############################################
if [ $HOST = curie-ccrt ]
then
   # Hack du build pour Qt sur curie car sinon include non trouve:
   if [ "`grep ' \-opensource \-no-openssl' ./$build_visit`" = "" ]
   then
      echo "$build_visit hacked for Qt on $HOST..."
      echo $ECHO_OPTS "1,$ s? -opensource? -opensource -no-openssl?g\nw" | ed ./$build_visit 1>/dev/null 2>&1 
   fi
fi

#################################################
# Compilation des paquets necessaires et de VisIt
#################################################
echo $build_visit $options_build
rm -f build_visit*"_log" # On efface avant
# Environnement python si necessaire
#[ -f $TRUST_ROOT/exec/python/env.sh ] && source $TRUST_ROOT/exec/python/env.sh
# Generation du help
./$build_visit --help 1>$build_visit.help 2>&1
# Lancement du build
echo yes | ./$build_visit $options_build
if [ $? != 0 ]
then
   echo "$build_visit failed in `basename $0`. See $build_visit"_log
   exit -1
else
   echo "$build_visit succeeds."
   #rm -f build_visit*"_log" # On efface apres si succes
fi

#############
# Nettoyage #
#############
# Desormais on nettoie si le test a reussi
#clean
