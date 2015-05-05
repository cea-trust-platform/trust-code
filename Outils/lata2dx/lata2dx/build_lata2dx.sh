#!/bin/bash
echo Compilation of the lata2dx executable in `pwd`
if [ ! -d commun_triou -o  ! -d triou_compat ]
then
  echo Directories commun_triou and triou_compat not found.
  echo Build_lata2dx must be run in the lata2dx source code directory.
  exit
fi
# debut du check du plugin lata V1 avec la version de TRUST
#cd commun_triou
#echo Checking plugin lata
#err=`./Check_LataV1.sh`
#[ "$err" != "" ] && echo "$err" && exit 1
#cd ../
# fin du check : Cette verification pourra etre supprimee quand le plugin V2 sera fonctionnel et qu'on enlevera le plugin V1.
echo Creating ./build directory, symlinks to source code and makefile
#rm -rf build
mkdir -p  build
if [ "$1" = "quick" ]
then
	echo Quick compilation: creating a big all_sources.cpp file and compile it.
	shift
	for i in commun_triou/*.h triou_compat/*.h
	do
	  ln -f -s ../$i build/`basename $i`
	done
	for i in commun_triou/*.cpp triou_compat/*.cpp lata2dx.cpp
	do
		cat $i
	done > build/all_sources.cpp
else	
	for i in commun_triou/*.{h,cpp} triou_compat/*.{h,cpp}
	do
	  ln -f -s ../$i build/`basename $i`
	done
	ln -f -s ../lata2dx.cpp build/lata2dx.cpp
	ln -f -s ../compare_lata.cpp build/compare_lata.cpp
fi
cp Makefile.in build/Makefile
cd build
chmod +w Makefile
if test -z "$TRUST_CC"
then
	export TRUST_CC=g++
fi
make depend
make $1

