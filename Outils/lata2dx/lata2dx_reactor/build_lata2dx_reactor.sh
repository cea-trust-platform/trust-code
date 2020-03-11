#!/bin/bash
[  -f $TRUST_ROOT/exec/lata2dx_reactor ] &&  [ $HOST != $TRUST_HOST_ADMIN ] &&  exit 0
mkdir -p build
cd build
echo "Building application lata2dx_reactor..."
for i in ../*.{h,cpp} ../../lata2dx/commun_triou/*.{h,cpp} ../../lata2dx/triou_compat/*.{h,cpp}
do
  ln -sf $i .
done
cp ../Makefile.in Makefile

# Compilation de l'appli
make depend
make $1
err_compilation=$?
if [ ! -f $TRUST_ROOT/exec/lata2dx_reactor ] || [ lata2dx_reactor -nt $TRUST_ROOT/exec/lata2dx_reactor ]
then
   # Test de lata2dx_reactor si nouveau binaire
   Build=$TRUST_ROOT/build/Outils/lata2dx/lata2dx_reactor
   [ ! -d ${Build} ] && echo Creating ${Build} directory && mkdir -p ${Build}
   rm -rf $Build/test
   cp -r ../test $Build/.
   cd $Build/test/src
   # Construction de la fiche de validation uniquement sur PC TMA
   if [ -f /usr/bin/pdflatex ] && [ $HOST = $TRUST_HOST_ADMIN ] && [ -f $TRUST_ROOT/Validation/Outils/Genere_courbe/Run ] && [ "`cd $TRUST_ROOT/Outils/VisIt;make -n 2>&1 | grep visit`" = "" ]
   then
      Run_fiche
      err_test=$?
      if [ -f ../build/rapport.pdf ]
      then
         mv ../build/rapport.pdf .
         rm -r -f ../build
         echo "File rapport.pdf created in `pwd`."
      fi      
   else
      # Sinon teste uniquement l'application sans construire la fiche de validation
      ./prepare
      err_test=$?
   fi
   if [ $err_test = 0 ] 
   then
      echo "Test lata2dx_reactor OK!"
       
      cd $TRUST_ROOT/Outils/lata2dx/lata2dx_reactor
      mv build/lata2dx_reactor $TRUST_ROOT/exec/lata2dx_reactor
      rm -rf build test/build 
      rm -rf $TRUST_ROOT/build/Outils/lata2dx
   else 
      echo "Test lata2dx_reactor KO!"
   fi
   exit $err_test
fi
exit $err_compilation

