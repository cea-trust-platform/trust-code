#!/bin/bash

DIR=`dirname $0`
proj=`(cd $DIR/../..;pwd)`


mkdir -p $proj/build/rapport
cp $DIR/* $proj/build/rapport

cd $proj
make check_all_optim validation doxygen 
[ $? -ne 0 ] && echo "pb ? make check_all_optim validation doxygen " && exit



cd $proj/build/rapport

for file in classProbleme__base.png classEquation__base.png classConvection__Diffusion__std.png classConvection__Diffusion__Concentration.png
do

cp $TRUST_ROOT/doc/html/$file .

done

cp classConvection__Diffusion__std.png  convection_diffusion_std.png




cp $proj/project.cfg .
cp $proj/src/*.cpp .
cp $proj/src/*.h .

tar zxvf  $proj/build/Validation/archives/*.tgz

sed "s/subsection/subsubsection/;s?\\\section?\\\subsection?" preserve/.tmp/corps.tex > corps.tex 

pdflatex rapport.tex
bibtex rapport
pdflatex rapport.tex
pdflatex rapport.tex
cp rapport.pdf $proj
