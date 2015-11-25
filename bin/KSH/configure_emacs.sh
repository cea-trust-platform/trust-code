#!/bin/bash
# Configure emacs
VERSION=`echo $TRUST_VERSION | awk '{gsub("\\\.","",$0);print $0}'`
file=~/.emacs.d/TRUST_$VERSION.emacs
echo "Configure emacs: $file"

# Grab the TRUST keywords
KeywordsTRUST=`$TRUST_Awk '!/\|/ {k=k" "$1} /\|/ {gsub("\\\|"," ",$0);k=k" "$0} END {print k}' $TRUST_ROOT/doc/TRUST/Keywords.txt`

# Count the keywords
nbKeywordsTRUST=`echo $KeywordsTRUST | wc -w`
nbKeywords=`$TRUST_Awk '/syntax keyword/ {print NF-3}' $file 2>/dev/null`

# If different, rebuild the configuration file for Emacs
if [ "$nbKeywordsTRUST" != "$nbKeywords" ] || [ $0 -nt $file ]
then
   mkdir -p `dirname $file`
   KeywordsTRUST=`echo $KeywordsTRUST | awk '{gsub(" ","\" \"",$0);print $0}'`
   echo ";; TRUST mode is applied on .data files:
(setq auto-mode-alist (cons '(\"\\\.data$\" . TRUST_$VERSION-mode) auto-mode-alist))
;; Define the TRUST mode:
(define-derived-mode TRUST_$VERSION-mode fundamental-mode
  (setq font-lock-defaults '((TRUST_$VERSION-font-lock-keywords)))
  (setq TRUSTLanguageKeywords$VERSION-regexp nil)
  ;; To avoid _ is a delimiter:
  (modify-syntax-entry ?_ \"w\" TRUST_$VERSION-mode-syntax-table)
  ;; TRUST Comments # #
  (setq comment-start \"# \")
  (setq comment-end \" #\")
  (setq comment-style 'multi-line) 
  (modify-syntax-entry ?# \". 14\" TRUST_$VERSION-mode-syntax-table)
  (modify-syntax-entry ? \". 23\" TRUST_$VERSION-mode-syntax-table)
)
;; Case insensitive:
(setq-default font-lock-keywords-case-fold-search t)
;; List of keywords:
(setq TRUSTLanguageKeywords$VERSION '(\"$KeywordsTRUST\") )
;; Regexp:
(setq TRUSTLanguageKeywords$VERSION-regexp (regexp-opt TRUSTLanguageKeywords$VERSION 'words))
;; ?
(setq TRUSTLanguageKeywords$VERSION nil)
(setq TRUST_$VERSION-font-lock-keywords
  \`(
    (,TRUSTLanguageKeywords$VERSION-regexp . font-lock-keyword-face)
))" > $file.tmp

   if [ ! -f $file ] || [ "`diff $file $file.tmp`" != "" ]
   then
      mv -f $file.tmp $file
      echo "$file updated..."
   fi
   rm -f $file.tmp
fi   
# Changement eventuel des fichiers de configuration si l'utilisateur
# n'utilise pas xemacs (car bug si transfert de emacs vers xemacs)
if [ ! -d ~/.xemacs ]
then
   #for config_file in ~/.emacs ~/.xemacs/init.el
   for config_file in ~/.emacs
   do
      # S'il a commente la ligne, on ne fait rien:
      if [ "`grep load $config_file 2>/dev/null | grep TRUST_ | grep ';;'`" = "" ]
      then
	 if [ "`grep load $config_file 2>/dev/null | grep TRUST_`" = "" ]
	 then
	    # First change of .emacs
	    mkdir -p `dirname $config_file`
	    echo "(load \"~/.emacs.d/TRUST_$VERSION.emacs\")" >> $config_file
	    echo "-> $config_file updated to highlight TRUST keywords in a data file."
	    echo "-> You can uncomment with ;; the load line in the $config_file file if you do not want this feature."
	 elif [ "`grep TRUST_$VERSION\.emacs $config_file`" = "" ]
	 then 
	    # Change the load:
	    echo $ECHO_OPTS "1,$ s?TRUST_\(.*\)\.emacs?TRUST_$VERSION\.emacs?g\nw" | ed $config_file 1>/dev/null 2>&1
	    echo "-> $config_file updated to highlight TRUST keywords in a data file."
	    echo "-> You can uncomment with ;; the load line in the $config_file file if you do not want this feature."
	 fi
      fi
   done
fi
