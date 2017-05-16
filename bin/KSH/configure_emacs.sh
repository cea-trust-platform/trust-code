#!/bin/bash
# Configure emacs

path_file=$TRUST_ROOT/doc/TRUST/Keywords_Emacs.el
keywords=$TRUST_ROOT/doc/TRUST/Keywords.txt

echo 'Configure emacs: '$path_file

# Grab the TRUST keywords
KeywordsTRUST=`$TRUST_Awk '!/\|/ {k=k" "$1} /\|/ {gsub("\\\|"," ",$0);k=k" "$0} END {print k}' $keywords`

# Count the keywords
nbKeywordsTRUST=`echo $KeywordsTRUST | wc -w`
nbKeywords=`$TRUST_Awk '/dimension/ {print NF-3}' $path_file 2>/dev/null`

# If different, rebuild the configuration file for Emacs
if [ "$nbKeywordsTRUST" != "$nbKeywords" ] || [ $0 -nt $path_file ]
then
   mkdir -p `dirname $path_file`
   KeywordsTRUST=`echo $KeywordsTRUST | awk '{gsub(" ","\" \"",$0);print $0}'`

   echo ";; TRUST mode is applied on .data files:
(setq auto-mode-alist (cons '(\"\\\.data$\" . TRUST-mode) auto-mode-alist))

;; Define the TRUST mode:
;;;###autoload
(define-derived-mode TRUST-mode fundamental-mode \"TRUST-mode\"
  \"Major mode for editing TRUST data files\"

  ;; code for syntax highlighting
  (setq font-lock-defaults '((TRUST-font-lock-keywords)))

  ;; To avoid _ is a delimiter:
  (modify-syntax-entry ?_ \"w\" TRUST-mode-syntax-table)

  ;; TRUST Comments # #
  (setq comment-start \"# \")
  (setq comment-end \" #\")
  (setq comment-style 'multi-line) 
  (modify-syntax-entry ?# \". 14\" TRUST-mode-syntax-table)
  (modify-syntax-entry ? \". 23\" TRUST-mode-syntax-table)

  ;; TRUST Comments /* */
  (modify-syntax-entry ?/ \"w\" TRUST-mode-syntax-table)
  (modify-syntax-entry ?* \"w\" TRUST-mode-syntax-table)

  ;; Adding { and } to the words, so we can highlight them
  (modify-syntax-entry ?{ \"w\" TRUST-mode-syntax-table)
  (modify-syntax-entry ?} \"w\" TRUST-mode-syntax-table)
)

;; Case insensitive:
(setq-default font-lock-keywords-case-fold-search t)

;; List of keywords:
(setq TRUSTLanguageKeywords '(\"$KeywordsTRUST\") )
(setq TRUSTAccolades '(\"{\" \"}\") )
(setq TRUSTSecondComments '(\"/*\" \"*/\") )

;; Regexp:
(setq TRUSTLanguageKeywords-regexp (regexp-opt TRUSTLanguageKeywords 'words))
(setq TRUSTAccolades-regexp (regexp-opt TRUSTAccolades 'words))
(setq TRUSTSecondComments-regexp (regexp-opt TRUSTSecondComments 'words))

;; create the list for font-lock.
;; each category of keyword is given a particular face
(setq TRUST-font-lock-keywords
  \`(
    (,TRUSTLanguageKeywords-regexp . font-lock-keyword-face)
    (,TRUSTAccolades-regexp . font-lock-function-name-face)
    (,TRUSTSecondComments-regexp . font-lock-function-name-face)
))

;; clear memory. no longer needed
(setq TRUSTLanguageKeywords nil)
(setq TRUSTAccolades nil)
(setq TRUSTSecondComments nil)

;; clear memory. no longer needed
(setq TRUSTLanguageKeywords-regexp nil)
(setq TRUSTAccolades-regexp nil)
(setq TRUSTSecondComments-regexp nil)

;; add the mode to the 'features' list
(provide 'TRUST-mode)

;; Local Variables&#58;
;; coding: utf-8
;; End:

;;; TRUST-mode.el ends here"      > $path_file.tmp


   if [ ! -f $path_file ] || [ "`diff $path_file $path_file.tmp`" != "" ]
   then
      mv -f $path_file.tmp $path_file
      echo ""
      echo $path_file' updated...'
   fi
   rm -f $path_file.tmp
fi
echo ""

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
#	    echo "(load \"~/.emacs.d/TRUST.emacs\")" >> $config_file
	    echo "(load $path_file)" >> $config_file
	    echo "Loading $path_file file."
	    echo "-> $config_file updated to highlight TRUST keywords in a data file."
	    echo "-> You can comment with ;; the load line in the $config_file file if you do not want this feature."
	 elif [ "`grep TRUST\.emacs $config_file`" = "" ]
	 then 
	    # Change the load:
	    echo $ECHO_OPTS "1,$ s?TRUST_\(.*\)\.emacs?TRUST\.emacs?g\nw" | ed $config_file 1>/dev/null 2>&1
	    echo "-> $config_file updated to highlight TRUST keywords in a data file."
	    echo "-> You can comment with ;; the load line in the $config_file file if you do not want this feature."
	 fi
      fi
   done
fi
