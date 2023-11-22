#!/bin/bash

DIR=`dirname $0`
proj=`(cd $DIR/../..;pwd)`


mkdir -p $proj/build/rapport
cp $DIR/* $proj/build/rapport

cd $proj
baltik_build_configure -execute
make check_all_optim validation check_gui #  doxygen 

[ $? -ne 0 ] && echo "pb ? make check_all_optim validation check_gui  " && exit



cd $proj/build/rapport

for file in classProbleme__base classEquation__base classConvection__Diffusion__std classConvection__Diffusion__Concentration
do

cp $TRUST_ROOT/doc/html/$file"__inherit__graph.png" $file.png

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
