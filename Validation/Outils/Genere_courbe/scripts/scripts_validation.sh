#!/bin/bash


echo def Extract_Times DIR
Extract_Times()
{
org=`pwd`
cd $1
ref=`pwd`
archives=`ls *.tgz`
archives_ok=`for arc in $archives ; do [ -f ${arc%.tgz}.pdf ] && echo ${arc}; done`
echo $archives_ok

mkdir -p build_prov
cd build_prov
stt=0
for arc in $archives_ok
  do
  cas=${arc%.tgz}
  mkdir -p $cas
  cd $cas
  tar zxf $ref/$arc preserve/temps_total
  echo  `head -1 preserve/temps_total`  $cas
  t=`head -1 preserve/temps_total | awk '{print $1}'`
  stt=`echo $stt+$t | bc -l`
  cd $ref/build_prov 
done    > Times
echo $stt Total  >> Times
echo $stt Total 
sort -n Times > ../Times
cd ..
rm -rf build_prov
cd $org
}

echo def verif_BUILD
verif_BUILD()
{
  if [ "$Rapports_auto_root" = "" ]
	then
	Rapports_auto_root=$TRUST_ROOT
	export Rapports_auto_root
  fi
  for f in `ls BUILD`
    do
    [ ! -d BUILD/$f ]  && continue
    echo $f
    f2=`find  $Rapports_auto_root/Validation/Rapports_automatiques -follow  -name $f -type d | grep -v $f/src`
    diff -r BUILD/$f/src $f2/src
  done
}


echo def verif_archives
verif_archives()
{
 org=`pwd`

    if [ "$Rapports_auto_root" = "" ]
	then
	Rapports_auto_root=$TRUST_ROOT
	export Rapports_auto_root
    else
	echo "Use of $Rapports_auto_root directory for validation insteed of \$TRUST_ROOT"
    fi
    DIR=`dirname $0`
    DIR=`(cd $DIR;pwd)`
    cat /dev/null > nettoie
    cd archives
    for f in `ls *.pdf`
      do
      if [ ! -f `basename $f .pdf`.tgz ] 
	  then
	  echo $f pb ?
      else
	  
	  [ -d ../BUILD/`basename $f .pdf`/build ] && echo "rm -rf BUILD/`basename $f .pdf`/build" >> ../nettoie
      fi
      [ "`find $Rapports_auto_root/Validation/Rapports_automatiques/ -follow -type d -name  $(basename $f .pdf)`" = "" ] && [ "`find $Rapports_auto_root/Validation/Rapports_automatiques/ -follow -type l -name  $(basename $f .pdf)`" = "" ] &&echo $f n existe plus ?  
    done

    for file in `find  $Rapports_auto_root/Validation/Rapports_automatiques/$1 -follow -name '*'.prm`
      do
      f=`dirname $file`
      f=`dirname $f`
      f=`basename $f`
      if [ ! -f $f.pdf ] 
	  then
	  echo il manque $f ? 
      fi
    done
    echo `wc -l ../nettoie` repertoires a effacer
    chmod +x ../nettoie
    cd $org
}


echo def compare_pdf
compare_pdf()
{
    ref=$1
    new=$2

# ref=rapport.pdf
# new=tableau.pdf
    
 
    ref=`dirname $ref`/`basename $ref .pdf`
    new=`dirname $new`/`basename $new .pdf`

    rm -f ${new}_oo*png ${ref}_oo*png
# convert -density 300 $ref.pdf ${ref}_oo.png
# convert -density 300 $new.pdf ${new}_oo.png
    pdftoppm -r 100 $ref.pdf  ${ref}_oo
    pdftoppm -r 100 $new.pdf  ${new}_oo
    ko=0
    for file in `ls ${ref}_oo*ppm`
      do
      f2=`echo $file | sed "s?$ref?$new?"`
      if [ "`diff $file $f2 2>&1 `" = "" ]
	  then
	  echo compare $file OK
	  rm -f $file $f2
      else
	  ko=1
	  difffile=`echo $file | sed "s?_oo?_diff?"`
	  echo compare $file K0 : voir $difffile
	  compare $file $f2 $difffile
	  montfile=`echo $file | sed "s?_oo?_mont?"`
	  montage -geometry 630x900  -tile 3x1  $file  $difffile  $f2 $montfile
	  echo display $montfile >> avoir
      fi
    done
    return $ko
}



echo "def comp_fiche"
comp_fiche()
{
fiche=$1
echo "testing $fiche"
[ "$fiche" = "" ] && exit -1
[ ! -f old_rap/$fiche ] && echo old_rap/$fiche absent &&  return


rm -rf avoir 


compare_pdf new_rap/$fiche old_rap/$fiche

if [ ! -f avoir ] 
then

OK="o"
else
if [ "$mode" != "batch" ]
then
sh ./avoir



echo "OK ? (o/n)"
read OK
else
OK="batch"
fi
fi

if [ "$OK" = "o" ]
then 
mv new_rap/$fiche  new_rap/OK
else
if  [ "$OK" != "batch" ]
then
mv new_rap/$fiche  new_rap/KO
fi
fi
f=`basename $fiche .pdf`

rm -f *_rap/${f}*ppm

}


echo def compare_new_rap_old_rap [ batch ]





compare_new_rap_old_rap()
{

cd new_rap/; fiches=`ls *.pdf`; cd -

#. /users/fauchet/VUES/168/trio_u/TRUST/env/env_TRUST.sh


mkdir -p new_rap/OK new_rap/KO

export mode=$1

for fiche in $fiches
 do
comp_fiche $fiche

done

}


echo def genere_new_rap_old_rap  [ -perf ] newarchives oldarchives
gen_fiche()
{
cd preserve
$TRUST_ROOT/Validation/Outils/Genere_courbe/src/genererCourbes.py -p `ls *prm| grep -v test_lu.prm`
cd ..
}
gen_rap_fiche()
{

ref=$REF
new=$NEW
perf=""

perf=""
[ "$1" = "-perf" ] && perf=$1 && shift

fiche=$1
[ "$fiche" = "" ] && exit -1
[ ! -f $ref/$fiche ] && echo $ref/$fiche absent #  &&  return
pdf=`basename $fiche .tgz`.pdf
if [ -f new_rap/$pdf ] || [ -f new_rap/OK/$pdf ] || [ -f new_rap/KO/$pdf ]
then
echo on  a deja la fiche  $pdf
return
fi
echo on traite la fiche $pdf
rm -rf avoir preserve new
tar zxf $new/$fiche
# cd preserve; /users/fauchet/VUES/169/trio_u/TRUST/Validation/Outils/Genere_courbe/src/genererCourbes.py -p *prm -d -2 ;exit
gen_fiche

mv preserve new
mv new/rapport.pdf new_rap/$pdf

[ ! -f $ref/$fiche ] && echo $ref/$fiche absent  &&  return

tar zxf $ref/$fiche
\cp new/version_utilisee preserve
if [ "$perf" != "" ]
then
 cd new
 tar cvf perf.tar `find . -name '*'.perf` temps_total
 cd ../preserve
 tar cvf perf.tar `find . -name '*'.perf` temps_total
 tar xvf ../new/perf.tar
 cd ..
fi
gen_fiche
mv preserve/rapport.pdf old_rap/$pdf


}

genere_new_rap_old_rap()
{
perf=""
[ "$1" = "-perf" ] && perf=$1 && shift
NEW=$1
REF=$2
[ "$NEW" = "" ] && exit
[ "$REF" = "" ] && exit
cd $NEW/; fiches=`ls *.tgz`; cd -

mkdir -p new_rap old_rap
for fiche in $fiches
 do
  gen_rap_fiche $perf $fiche
done

}


echo def Generate_makefile_validation [ -without_deps_exe ]


Generate_makefile_validation()
{
deps="\$(exec)"
[ "$1" = "-without_deps_exe" ] && deps=""
LANCE=$TRUST_ROOT/Validation/Outils/Genere_courbe/scripts/Lance_gen_fiche
. $LANCE

prm=`get_list_prm`

echo "all:\t `get_list_prm`" > makefile

res=`for  cas in $prm
do
  basename $cas
done`
resb=""
res2=""
for  cas in $res
do
  res2=$res2" "archives/$cas.pdf
  resb=$resb" "$cas
done
echo $ECHO_OPTS "all_tgz: $resb" >makefile

echo $ECHO_OPTS "all:\t $res2" >> makefile

for p in $prm
do
cas=$(basename $p)
pdf="archives/$(basename $p).pdf"
echo $ECHO_OPTS "BUILD/deps_$cas: $deps \$(shell find $p/src -newer $pdf 2>/dev/null)" 
echo $ECHO_OPTS "\t mkdir -p BUILD;touch BUILD/deps_$cas"
echo "${pdf}: BUILD/deps_$cas"
echo $ECHO_OPTS "\t$LANCE \$(option_fast) -pdf_only  $p"
done >> makefile


for p in $prm
do
cas=$(basename $p)
echo $cas: archives/$cas.tgz
echo archives/$cas.tgz: archives/$cas.pdf
echo $ECHO_OPTS "\tenv OPTIONS_PRM=-suite_run $LANCE \$(option_fast) $p"
done >> makefile


}
