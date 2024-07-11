#!/bin/bash
# Script d'indentation automatique des sources TRUST (appele par le trigger en pre checkin)
checkout=1 && [ "$1" = -nocheckout ] && checkout=0 && shift
files=$* && [ "$1" = -fix ] && files=`cleartool find . -name '*'.cpp -print -nxn;cleartool find . -name '*'.h -print -nxn`
for file in $files
do
   # Si le fichier source existe
   if [ -f $file ]
   then
      #if [ "`head -4 $file | grep _ROOT | grep Directory:`" != "" ]
      #if [ "`head -15 $file | grep Copyright | grep CEA`" != "" ]
      if [ ${file%.cpp} != $file ] || [ ${file%.tpp} != $file ] || [ ${file%.h} != $file ]  # Source code file 
      then
         # Skip file if the tag is found in the first 10 lines
         if [ "`head -10 $file | grep TRUST_NO_INDENT`" != "" ]; then exit 0; fi
         
         tmp=`mktemp_`
         cp $file $tmp
         # Skip lines that start with a comment (//) and then replace all patterns with a 'for' or an 'if' placed after a ')' to break the line.
         # This avoids patterns like 'for() for() if(...)' on a single line ... 
         sed -i "/^ *\/\//! s@\()\) *\(\(for *(\)\|\(if *(\)\)@\1\n\2@g" $tmp
         LC_ALL="en_US.UTF-8" $TRUST_ROOT/exec/astyle/bin/astyle --keep-one-line-blocks --style=gnu --indent=spaces=2 --indent-cases --align-reference=type --max-instatement-indent=120 $tmp 1>/dev/null
         # New version astyle:
         # LC_ALL="en_US.UTF-8" $TRUST_ROOT/exec/astyle/bin/astyle --keep-one-line-blocks --style=gnu --indent=spaces=2 --indent-cases --align-reference=type --max-continuation-indent=120 $tmp 1>/dev/null
         rm -f $tmp.orig
         if [ "`diff $file $tmp`" != "" ]
         then
                if [ "$1" != -fix ]
                then
                   echo "$file indented."
                   [ "$checkout" = 1 ] && CHECKOUT $file 1>/dev/null 2>&1
                   cat $tmp > $file
                else
                    echo "$file indented."
                    CHECKOUT $file 1>/dev/null 2>&1
                    cat $tmp > $file
                    #CHECKIN $file
                fi
         fi
         rm -f $tmp $tmp"~"
      elif [ ${file%.data} != $file ] && [ "`grep -i dimension $file`" != "" ]
      then
         # TRUST data file
         activated=NO # Not activated yet cause # comment is not as /* May be changed # # in /* */ everywhere ? 
         #$TRUST_ROOT/exec/astyle/bin/astyle -n --indent=spaces=8 $files 1>/dev/null
         #echo "$file indented."
      fi
   fi
done
