#!/bin/bash
# Check the TRUST source files for several rules:
usage()
{
   echo "Usage: `basename $0` [-warning] -print|-update|-fix [-recurse]"
   echo "Print, update or fix files to respect some rules:"
   echo "Rule I : filename=classname where classname is the name of the first class found in the include file"
   echo "Rule II: filename=Ref|Deriv|Vect|List_classname if Ref|Deriv|Vect|List class"
   echo "Rule III : ..."
   exit 0
}
backward=$TRUST_ROOT/include/backward && [ ! -d $backward ] && echo "$backward DO NOT exist. Contact TRUST support." && exit -1
warning=0 && [ "$1" = -warning ] && shift && warning=1 # Print warnings too
if [ "$1" = -print ]
then
   fix=0
elif [ "$1" = -fix ]
then
   if [ ! -d $TRUST_ROOT/.git ]
   then
      echo "This command should be used under git only."
      exit -1
   fi
   fix=1
   if [ ! -d $backward ]
   then
      mkdir -p $backward
      git add $backward
   fi
elif [ "$1" = -update ]
then
   # Construction du fichier sed qui sera utilise pour -update
   sed_files=`mktemp_`
   regex="[ <>,;()]"
   for old in `cd $backward;ls *.h`
   do
      new=`head -1 $backward/$old | awk '{gsub("<","",$2);gsub(">","",$2);print $2}'`
      old=${old%.h}
      new=${new%.h}
      echo "s/\($regex\)$old\.\(h\|cpp\)\($regex\)/\1$new.\2\3/g" >> $sed_files # $old surrounded by $regex delimitors
      echo "s/\($regex\)$old\.\(h\|cpp\)$/\1$new.\2/g" >> $sed_files # $old at the end the line
      echo "/\/\/ \.\(NOM\|ENTETE\|LIBRAIRIE\|FILE\|CONTRAINTES\|INVARIANTS\|HTML\|EPS\) /d" >> $sed_files # Suppress old useless balises
      echo "/\/\/ \.\(NOM\|ENTETE\|LIBRAIRIE\|FILE\|CONTRAINTES\|INVARIANTS\|HTML\|EPS\)$/d" >> $sed_files # Suppress old useless balises
   done
   # Substitue tous les anciens includes par les nouveaux
   for file in `find . -type f \( -name '*'.cpp -o -name '*'.h -o -name '*'.cpp.P -o -name '*'.cpp.h -o -name '*'__ -o -name __'*' \) | grep -v /backward | grep -v lib/src`
   do
      sed -f $sed_files $file > $file.tmp
      diff $file $file.tmp 1>/dev/null 2>&1 || (CHECKOUT $file 1>/dev/null 2>&1;cat $file.tmp > $file && echo "Updating $file ...") 
      rm -f $file.tmp
      # Detect the file should be renamed:
      old=`basename $file`
      cpp=0 && [ ${old%.cpp} != $old ] && old=${old%.cpp}.h && cpp=1
      if [ -f $backward/$old ]
      then
         new=`head -1 $backward/$old | awk '{gsub("<","",$2);gsub(">","",$2);print $2}'`
         [ $cpp = 1 ] && new=${new%.h}.cpp
	 rep=`dirname $file`
	 if [ ! -f $rep/$new ]
	 then
	    if [ "`git branch 2>/dev/null`" != "" ]
	    then
	       # Git
	       git mv $file $rep/$new
	       under="under Git"
	    else
	       # Other
	       mv $file $rep/$new
	       under=""
	    fi
	    echo "-> $file is renamed to $rep/$new $under"
	 else
	    echo "Error ! $rep/$new is already here!" 
	    exit -1
	 fi
      fi 
   done  
   rm -f $sed_files
   exit 0
else
   usage
   exit 0
fi
shift   
err=0
files=$*
if [ "$files" = -recurse ]
then
   recurse=1
   reps=""
   for rep in `find . -type d`
   do
      if [ -f $rep/make.include ]
      then
         reps=$reps" "$rep
      fi
   done
else
   reps="."
   recurse=0
fi

ROOT=`pwd`
for rep in $reps
do
   cd $ROOT/$rep
   files=`ls *.h 2>/dev/null`
   for file in $files
   do
      if [ ! -d $file ]
      then
	 if [ ! -f $file ]
	 then
	    echo "Error: $file not found."
	    exit -1
	 else
	    # Find the include file
	    filename=${file%.h}
	    if [ $file != $filename ]
	    then
	       h=$filename.h
	       cpp=$filename.cpp
	       if [ -f $h ]
	       then
		  command="awk '(\$1==\"class\") && !/;/ && !/\\\/ && !/dummy/ {gsub(\":\",\"\",\$2);print \$2}' $h"
        	  nb_classes=`eval $command | wc -l`
		  if [ $nb_classes = 0 ]
		  then
		     # 0 classes detected, may be a Ref|Deriv|Vect|List
		     # Check Rule II:
		     classtype=`$TRUST_Awk -F"(" '/Declare_/ && !/\\\/ {gsub("Declare_","",$1);print $1}' $h`
		     classname=`$TRUST_Awk -F"(" '/Declare_/ && !/\\\/ {print (NF==3?$2" "$3:$2)}' $h | $TRUST_Awk -F")" '{print $1}'`
		     classname=`echo $classname | $TRUST_Awk '{gsub("REF ","Ref_",$0);gsub("VECT ","Vect_",$0);gsub("DERIV ","Deriv_",$0);gsub("LIST ","List_",$0); print $0}'`
		     if [ "$classtype" = ref ]
		     then
			classtype=Ref
		     elif [ "$classtype" = deriv ]
		     then
			classtype=Deriv
		     elif [ "$classtype" = vect ]
		     then
			classtype=Vect
		     elif [ "$classtype" = liste ]
		     then
			classtype=List
		     fi		
		     nb_classtype=`echo $classtype | wc -w`
		     nb_classname=`echo $classname | wc -w`
		     # One class found:
		     if [ "$nb_classtype" = 1 ] && [ "$nb_classname" = 1 ]
		     then
		        classname=$classtype"_"$classname
		     else
		        [ $warning = 1 ] && echo "WARNING: $nb_classes classes defined in $h !"
			classname="" # To bypass
		     fi
		  elif [ $nb_classes -gt 1 ]
		  then
		     # On prend la premiere classe
		     classname=`eval $command | head -1`		     
		     classnames=`eval $command`
		     [ $warning = 1 ] && echo $ECHO_OPTS "WARNING: $nb_classes classes defined in $h :\n$classnames"
		  else
		     # on prend la classe
		     classname=`eval $command`
		  fi
		  # Check Rules I and II:
		  if [ "$classname" != "" ]
		  then
		     if [ $classname != $filename ]
		     then
			err=1
			if [ $fix = 0 ]
			then
			   message="should be"
			else
			   message="is"
		           # Construction du fichier backward
		           echo $ECHO_OPTS "#include <$classname.h>
#warning #include <$h> should now be replaced by #include <$classname.h>
" > $backward/$h
			   # git commands
			   [ -f $classname.h ] && echo "Problem! $classname.h can't be erased by $h !" && exit -1
			   git mv $h $classname.h
			   if [ -f $cpp ]
			   then
			      [ -f $classname.cpp ] && echo "Problem! $classname.cpp can't be erased by $cpp !" && exit -1
			      git mv $cpp $classname.cpp
			   fi
			   git add $backward/$h
	        	fi
			echo "-> $h $message renamed $classname.h"
			[ -f $cpp ] && echo "-> $cpp $message renamed in $classname.cpp"
		     else
			# check include guards:
			# #ifndef classname_H
			# #define classname_H
			ifndef="#ifndef "$classname"_included"
			define="#define "$classname"_included"
			current_ifndef=`grep '^#ifndef ' $h | head -1`
			current_define=`grep '^#define ' $h | head -1`			
			if [ "$current_ifndef" != "$ifndef" ] || [ "$current_define" != "$define" ]
			then
		           if [ $fix = 0 ]
			   then
			      echo "Incorrect, you should have in $h:"
			      echo $ifndef instead of $current_ifndef
			      echo $define instead of $current_define
			      err=1
			   else
			      if [ "`echo $current_ifndef | wc -w`" = 2 ] && [ "`echo $current_define | wc -w`" = 2 ]
			      then
				 sed -e "s/$current_ifndef/$ifndef/g" -e "s/$current_define/$define/g" $h > $h.tmp
				 echo "Include guard changed in $h:"
				 diff $h $h.tmp
				 CHECKOUT $h 1>/dev/null 2>&1
				 cat $h.tmp > $h
				 rm -f $h.tmp
			      elif [ "$current_ifndef" = "" ] || [ "$current_define" = "" ]
			      then
				 echo "Missing include guard in $h ! Add:"
				 echo $ifndef
				 echo $define
				 echo ...
				 echo "#endif"
				 exit -1
			      else
			         echo "Strange in $h :"
				 echo $current_ifndef
				 echo $current_define
				 echo "Contact TRUST support !"
				 exit -1
			      fi
			   fi 
			fi
		     fi
		  fi
	       fi
	    fi
	 fi
      fi
   done
done
[ $err = 1 ] && [ $fix = 1 ] && echo "Now to change all the includes in source files, run: `basename $0` -update"
exit $err

#warning [Fast turnaround]: You can suppress -Werror of the \$TRUST_ROOT/env/make.$TRUST_ARCH_CC* files to suppress this warning
#warning [Slow but safe]  : Manually change the $h name by $classname.h in each source files  
#warning [Fast but risk]  : Update automatically all your source files with: check_file.sh -update
