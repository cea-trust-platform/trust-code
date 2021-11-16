#!/bin/bash
echo $0 $*

#set -xv
PID=$$
echo current_PID $PID start
trap "echo Info_global $pp KILL;echo current_PID $PID stop ;exit 1" HUP INT PIPE QUIT TERM  KILL
trap "echo current_PID $PID stop" EXIT

right()
{
 word=$1
 suff=`echo $word | awk -F= '{print $1"="}'`
 echo ${word#${suff}}
 
}



## analyse des options

archive=$1
shift
path_to_run=$2
shift
machine_cible=$3
shift

### traite_option()

VALGRIND=0
MODE=mode_not_set
TRUST_ROOT=trio_not_set
PAR_F=not_set
doclean=0
norun=0
last_pb="";
cases=""
clean_after=0
cible=0
for word in $*
  do
  case $word in
      --with-trio*) TRUST_ROOT=`right $word` ;;
      --with-source-trio*) SOURCE_TRUST=`right $word` ;;
      --with-TRUST*) TRUST_ROOT=`right $word` ;;
      --with-source-TRUST*) SOURCE_TRUST=`right $word` ;;
      --with-mode*) MODE=`right $word` ;;
      --with-valgrind) VALGRIND=1 ;;
      --without-parallel) PAR_F=0;;
      --with-clean) doclean=1 ;;
      --with-efface-dir-before) doclean=3 ;;
      --with-efface) doclean=2 ;;
      --without-run) norun=1 ;;
      --with-case*) cases=`right $word`;  last_pb="_last_pb"  ;;
      --last-pb) last_pb="_last_pb";;
      --check-all) last_pb="_all";;
      --check-trust) last_pb="_trust";;
      --clean-after-if-ok) clean_after=1 ;;
      --cible) cible=1;;
      *) echo $word non compris;echo options possibles ; grep '\-\-' $0| awk -F\) '{print $1}'  ;exit 1;;
  esac
done

ORIG=`pwd`
if [ "$SOURCE_TRUST" != "" ]
then
source $SOURCE_TRUST
fi
files=`tar tf $archive | grep .tar.gz`
for file in $files
do
  f2=`basename $file .tar.gz`
  [ -f $f2.tar.gz ] && project=$f2
done
#project=pena
echo "Project :" $project
## generation de env.sh

pwd
[ "$VALGRIND" != "0" ] && MODE=${MODE}_valgrind 
echo "
unset TRUST_ENV
TRUST_ROOT=$TRUST_ROOT
projet=$project

project_directory=`pwd`/$project
export project_directory

MODE=$MODE
#ORIG=`pwd`
cd \$TRUST_ROOT
source env_TRUST.sh
# [ \$? -ne 0 ] && echo KO , TRUST_ROOT: $TRUST_ROOT false ? 
cd $ORIG" > env.sh
[ "$VALGRIND" != "0" ] &&  echo "# VALGRIND=1 ; export VALGRIND" >> env.sh
export MODE
[ "$PAR_F" != "not_set" ] && echo "PAR_F=0;export PAR_F" >> env.sh  && echo Info_global test_par OFF


source ./env.sh

[ "$TRUST_ENV" = "" ]  && echo "KO , env.sh false ?" && exit 1


# generation des autres scripts

for pp in configure make make_check make_install
  do
  echo "#!/bin/bash
echo \$0 \$*
PID=\$\$
echo current_PID \$PID start
trap \"echo current_PID \$PID stop\" EXIT
trap \"echo Info_global $pp KILL;echo current_PID \$PID kill;exit 1\" HUP INT PIPE QUIT TERM  KILL

source ./env.sh
cd \${projet}
[ \$? -ne 0 ] && echo error with \$projet dir&& exit 1  
" > $pp.sh
  chmod +x $pp.sh
done
# echo "pwd;ls" >> configure.sh
# droits apres la compilation
titan=0 && [ "`id | grep dm2s-projet-trust_trio-rw`" != "" ] && [ -L ~/scripts_ACL ] && [ -e ~/scripts_ACL ] && titan=1 && echo "sh ~/scripts_ACL/Update_ACL.sh `pwd` " >> configure.sh  
echo "./configure" >> configure.sh


#echo "./configure && make $MODE" >> make.sh # ajout du configure, necessaire quand il a un pre_configure (ex -std=c++0x)
echo "make $MODE" >> make.sh

# droits apres la compilation
if [ "$titan" = "1" ]
then
   if [ "$project" = triocfd ]
   then
      echo "chmod ugo+x TrioCFD*" >> make.sh
   elif [ "$project" = flica5 ] && [ "`id | grep dm2s-projet-trust_trio-r`" != "" ] 
   then
      echo "chmod ugo+x FLICA5*" >> make.sh
   elif [ "$project" = genepi3 ] && [ "`id | grep dm2s-projet-trust_trio-r`" != "" ] 
   then
      echo "chmod ugo+x genepi*" >> make.sh
   fi
elif [ "$project" = triocfd ]
then
  echo "cd ../../" >> make.sh
  echo "chmod -R u+w,gou+r triocfd 1>/dev/null 2>&1" >> make.sh
  echo "find triocfd -type d | xargs chmod +x  1>/dev/null 2>&1" >> make.sh
  echo "find triocfd -type d -exec chmod 755 {} \;" >> make.sh
  echo "cd triocfd/triocfd" >> make.sh
  echo "chmod ugo+x TrioCFD*" >> make.sh
elif [ "$project" = flica5 ] && [ "`id | grep dm2s-projet-trust_trio-r`" != "" ] 
then
  echo "cd ../../
setfacl -Rm g:dm2s-user-cat-b:rx $projet
setfacl -Rdm g:dm2s-user-cat-b:rx $projet
setfacl -Rm g:dm2s-user-cat-a:rx $projet
setfacl -Rdm g:dm2s-user-cat-a:rx $projet
setfacl -Rx g:dm2s-projet-trust_trio-r $projet
cd $projet/$projet" >> make.sh
elif [ "$project" = genepi3 ] && [ "`id | grep dm2s-projet-trust_trio-r`" != "" ] 
then
  echo "cd ../../
setfacl -Rdm g:dm2s-projet-genepi-rw:rwx .
setfacl -Rm g:dm2s-projet-genepi-rw:rwx $projet
setfacl -Rdm g:dm2s-projet-genepi-rw:rwx $projet
setfacl -Rx g:dm2s-projet-trust_trio-r $projet
cd $projet/$projet" >> make.sh
fi

if [ "$cases" != ""  ]
then
    echo $cases | awk -F, '{for (i=1;i<=NF;i++) {print $i}}' > liste_pb.all
    echo "cp ../liste_pb.all ." >>make_check.sh
fi
if [ $norun -eq 0 ]
    then
    echo "make check${last_pb}_$MODE" >> make_check.sh
else
    echo "echo Info_global make_check OFF" >> make_check.sh
fi
if [ $clean_after -eq 0 ]
then
    echo "echo do nothing " >> make_install.sh
else
    echo "cd ..; if [ -d $projet ]
     then
           rm -rf $projet *.sh  update_report  $archive $projet.tar.gz baltik
 echo repertoire $projet efface
 pwd;ls 
fi " >> make_install.sh    
fi
# on prepare le projet
sed -i "10a source env_*.sh" make_check.sh
#sed -i "10a export PATH=\${project_directory}/build/xdata:\$PATH" make_check.sh
#sed -i "11a export TRUST_TESTS=${project_directory}/build/tests/all/" make_check.sh
#sed -i "12a export Rapports_auto_root=${project_directory}/build/tests/share/all/ " make_check.sh
#sed -i "13a export exec=\${project_directory}/$projet_exe_opt" make_check.sh


if [ $doclean -ge 2 ]  
    then
    echo on efface le repertoire
    [ -d $projet ] && rm -rf $projet  && echo repertoire $projet efface
    if [ $doclean -eq 2 ]  && [ -f $project.tar ]
	then
	set -xv
	rm -rf `tar tf $projet.tar | grep -v baltik `
	rm -rf $project.tar* baltik
	ls
	# echo Info_global prepare KO efface
	exit 1
    fi

    
fi

tar zxf $projet.tar.gz
echo "we remove $project.tar and $project.tar.gz"
rm $projet.tar $projet.tar.gz


if [ "$titan" = "1" ]
then
   echo "we do nothing"
elif [ "$project" = flica5 ] && [ "`id | grep dm2s-projet-trust_trio-r`" != "" ] 
then
   # droits au moment de la creation du dossier
   cd ..
   setfacl -Rx g:dm2s-projet-trust_trio-r flica5
   setfacl -Rm g:dm2s-user-cat-b:rx flica5
   setfacl -Rdm g:dm2s-user-cat-b:rx flica5
   setfacl -Rm g:dm2s-user-cat-a:rx flica5
   setfacl -Rdm g:dm2s-user-cat-a:rx flica5
   cd -
elif [ "$project" = genepi3 ] && [ "`id | grep dm2s-projet-trust_trio-r`" != "" ] 
then
   # droits au moment de la creation du dossier
   cd ..
   setfacl -Rx g:dm2s-projet-trust_trio-r genepi3
   setfacl -Rm g:dm2s-projet-genepi-rw:rwx genepi3
   setfacl -Rdm g:dm2s-projet-genepi-rw:rwx genepi3
   cd -
fi


cd $projet
../baltik/bin/baltik_build_configure

if [ $doclean -eq 1 ]
then
make clean
../baltik/bin/baltik_build_configure
fi


#echo "Info_global release "`uname -r`
echo "Info_global model "`uname -m`
mode=$MODE
# [ "$VALGRIND" = "1" ] && mode=valgrind
echo "Info_global mode "$mode



echo Info_global CC `basename $TRUST_CC_BASE` `$TRUST_CC_BASE --version 2>&1 | head -1 | $TRUST_Awk '{print $3}'`
#OS=`uname -s` && [ $TRUST_ARCH = linux ] && [ -f /etc/issue ] && OS=`awk '(NF>0) {gsub("Welcome to ","",$0);print $0}' /etc/issue | head -1`
print_OS()
{
   for file in /etc/system-release /etc/release /etc/issue.net /etc/issue
   do
      [ -f $file ] && echo `head -1 $file` && break
   done
}
OS=`uname -s` && [ $TRUST_ARCH = linux ] && OS=`print_OS` 
#OS="Os "`uname -s` && [ $TRUST_ARCH = linux ] && OS=$OS" "`cat /proc/cpuinfo | $TRUST_Awk '/MHz/ {print $4"Mhz";exit}'`
echo Info_global Os $OS 
echo Info_global cible $cible
