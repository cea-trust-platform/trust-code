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
echo "
unset TRUST_ENV
TRUST_ROOT=$TRUST_ROOT
projet=$project

project_directory=`pwd`/$project
export project_directory

MODE=$MODE
#ORIG=`pwd`
cd \$TRUST_ROOT
source env/env_TRUST.sh
# [ \$? -ne 0 ] && echo KO , TRUST_ROOT: $TRUST_ROOT false ? 
cd $ORIG" > env.sh
[ "$VALGRIND" != "0" ] && MODE=${MODE}_valgrind && echo "# VALGRIND=1 ; export VALGRIND" >> env.sh
export MODE
[ "$PAR_F" != "not_set" ] && echo "PAR_F=0;export PAR_F" >> env.sh  && echo Info_global test_par OFF


source ./env.sh

[ "$TRUST_ENV" = "" ]  && echo "KO , env.sh false ?" && exit 1


# generation des autres scripts

for pp in configure make make_check make_install
  do
  echo "echo \$0 \$*
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
echo "./configure" >> configure.sh


echo "make $MODE" >> make.sh
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
echo "echo do nothing " >> make_install.sh
# on prepare le projet


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

cd $projet
../baltik/bin/baltik_build_configure

if [ $doclean -eq 1 ]
then
make clean
../baltik/bin/baltik_build_configure
fi


echo "Info_global release "`uname -r` 
echo "Info_global model "`uname -m`
mode=$MODE
[ "$VALGRIND" = "1" ] && mode=valgrind
echo "Info_global mode "$mode



echo Info_global CC `basename $TRUST_CC_BASE` `$TRUST_CC_BASE --version 2>&1 | head -1 | $TRUST_Awk '{print $3}'`
OS=`uname -s` && [ $TRUST_ARCH = linux ] && [ -f /etc/issue ] && OS=`awk '(NF>0) {gsub("Welcome to ","",$0);print $0}' /etc/issue | head -1`
#OS="Os "`uname -s` && [ $TRUST_ARCH = linux ] && OS=$OS" "`cat /proc/cpuinfo | $TRUST_Awk '/MHz/ {print $4"Mhz";exit}'`
echo Info_global Os $OS 
