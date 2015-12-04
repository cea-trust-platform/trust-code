#!/bin/bash
# Configure vim

build=0

VERSION=`echo $TRUST_VERSION | awk '{gsub("\\\.","",$0);print $0}'`
file=~/.vim/syntax/TRUST.vim
ftdetect=~/.vim/ftdetect/TRUST.vim

file_syntax=$TRUST_ROOT/doc/TRUST/Keywords.Vim

if [ "$1" = "build_syntax" ]
then


# Grab the TRUST keywords
KeywordsTRUST=`$TRUST_Awk '!/\|/ {k=k" "$1} /\|/ {gsub("\\\|"," ",$0);k=k" "$0} END {print k}' $TRUST_ROOT/doc/TRUST/Keywords.txt`

# Count the keywords
nbKeywordsTRUST=`echo $KeywordsTRUST | wc -w`
nbKeywords=`$TRUST_Awk '/syntax keyword/ {print NF-3}' $file_syntax 2>/dev/null`

# If different, rebuild the configuration file for vim
if [ "$nbKeywordsTRUST" != "$nbKeywords" ] || [ $0 -nt $file_syntax ]
then
   # Rebuild the file
   echo "syntax keyword TRUSTLanguageKeywords$KeywordsTRUST" >> $file.tmp
   if [ ! -f $file_syntax ] || [ "`diff $file $file.tmp`" != "" ]
   then
      mv -f $file.tmp $file_syntax
      echo "$file_syntax updated..."
   fi
   rm -f $file.tmp
   
fi
exit 
fi

# Delete old configurations which caused issues:
rm -f ~/.vim/*/TRUST_*.vim

echo "Configure vim:   $file"


   # Rebuild the file
   mkdir -p `dirname $file`
   echo "if exists(\"b:current_syntax\")
  finish
endif
let b:current_syntax = \"$type\"
hi def link TRUSTLanguageKeywords Type
hi def link TRUSTComments Comment
syntax case ignore
syntax region TRUSTComments start=/# / end=/ #/ keepend contains=TRUSTComments
syntax region TRUSTComments start=/\/\* / end=/ \*\// contains=TRUSTComments
" > $file.tmp
#   echo "syntax keyword TRUSTLanguageKeywordsKeywordsTRUST" >> $file.tmp

echo "let titi = \$TRUST_ROOT.\"/doc/TRUST/Keywords.Vim\"
if filereadable(titi)
source \$TRUST_ROOT/doc/TRUST/Keywords.Vim
endif" >> $file.tmp
echo "let titi = \$project_directory.\"/share/doc_src/Keywords.Vim\"
if filereadable(titi)
source \$project_directory/share/doc_src/Keywords.Vim
endif" >> $file.tmp

   if [ ! -f $file ] || [ "`diff $file $file.tmp`" != "" ]
   then
      mv -f $file.tmp $file
      echo "$file updated..."
   fi
   rm -f $file.tmp
   


# Changement eventuel du $ftdetect
if [ "`grep filetype=TRUST $ftdetect 2>/dev/null`" = "" ]
then
   # Configure vim to apply on .data files:   
   mkdir -p `dirname $ftdetect`
   echo "au BufRead,BufNewFile *.data set filetype=TRUST" > $ftdetect
   echo "$ftdetect created..."
elif [ "`grep filetype=TRUST $ftdetect`" = "" ]
then 
   # Change the load:
   echo $ECHO_OPTS "1,$ s?filetype=TRUST_\(.*\)?filetype=TRUST?g\nw" | ed $ftdetect 1>/dev/null 2>&1
   echo "$ftdetect updated..."
fi
