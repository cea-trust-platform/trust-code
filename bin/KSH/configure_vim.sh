#!/bin/bash
# Configure vim
VERSION=`echo $TRUST_VERSION | awk '{gsub("\\\.","",$0);print $0}'`
file=~/.vim/syntax/TRUST_$VERSION.vim
ftdetect=~/.vim/ftdetect/TRUST.vim
# Delete old configurations which caused issues:
rm -f ~/.vim/ftdetect/TRUST_*.vim
echo "Configure vim:   $file"

# Grab the TRUST keywords
KeywordsTRUST=`$TRUST_Awk '!/\|/ {k=k" "$1} /\|/ {gsub("\\\|"," ",$0);k=k" "$0} END {print k}' $TRUST_ROOT/doc/TRUST/Keywords.txt`

# Count the keywords
nbKeywordsTRUST=`echo $KeywordsTRUST | wc -w`
nbKeywords=`$TRUST_Awk '/syntax keyword/ {print NF-3}' $file 2>/dev/null`

# If different, rebuild the configuration file for vim
if [ "$nbKeywordsTRUST" != "$nbKeywords" ] || [ $0 -nt $file ]
then
   # Rebuild the file
   mkdir -p `dirname $file`
   echo "if exists(\"b:current_syntax\")
  finish
endif
let b:current_syntax = \"$type\"
hi def link TRUSTLanguageKeywords$VERSION Type
hi def link TRUSTComments Comment
syntax case ignore
syntax region TRUSTComments start=/# / end=/ #/ keepend contains=TRUSTComments
syntax region TRUSTComments start=/\/\* / end=/ \*\// contains=TRUSTComments
" > $file.tmp
   echo "syntax keyword TRUSTLanguageKeywords$VERSION$KeywordsTRUST" >> $file.tmp
   if [ ! -f $file ] || [ "`diff $file $file.tmp`" != "" ]
   then
      mv -f $file.tmp $file
      echo "$file updated..."
   fi
   rm -f $file.tmp
   
fi
# Changement eventuel du $ftdetect
if [ "`grep filetype=TRUST_ $ftdetect 2>/dev/null`" = "" ]
then
   # Configure vim to apply on .data files:   
   mkdir -p `dirname $ftdetect`
   echo "au BufRead,BufNewFile *.data set filetype=TRUST_$VERSION" > $ftdetect
   echo "$ftdetect updated..."
elif [ "`grep filetype=TRUST_$VERSION $ftdetect`" = "" ]
then 
   # Change the load:
   echo $ECHO_OPTS "1,$ s?filetype=TRUST_\(.*\)?filetype=TRUST_$VERSION?g\nw" | ed $ftdetect 1>/dev/null 2>&1
   echo "$ftdetect updated..."
fi
