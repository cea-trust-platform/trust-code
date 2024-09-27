#!/bin/bash

path_file="${TRUST_TMP}/trust-mode.el"
keywords="${TRUST_ROOT}/doc/TRUST/Keywords.txt"
baltik_keywords="${TRUST_TMP}/baltik_keywords.txt"
rm -f "${baltik_keywords}"

# prise en compte des keywords des baltiks
if [ -f "${project_directory}/share/doc_src/Keywords.txt.n" ]
then
    keywords="${project_directory}/share/doc_src/Keywords.Gedit"
    # baltik_keywords=$TRUST_TMP/baltik_keywords.txt

    cat "${TRUST_ROOT}/doc/TRUST/Keywords.txt"              >  "${keywords}"
    cat "${project_directory}/share/doc_src/Keywords.txt.n" >  "${baltik_keywords}"
    # pour supprimer les lignes contenant "|\hyperpage{99}," et "|{" -> sinon probleme avec nedit
    # pour supprimer les lignes contenant "|/*" et "|\#" -> sinon probleme avec gedit
    sed -i "/hyperpage/d; /{/d; /*/d; /#/d" ${keywords}
    sed -i "/hyperpage/d; /{/d; /*/d; /#/d" ${baltik_keywords}
fi

echo 'Configure emacs: '$path_file

# Grab the TRUST keywords
KeywordsTRUST=`$TRUST_Awk '!/\|/ {k=k" "$1} /\|/ {gsub("\\\|"," ",$0);k=k" "$0} END {print k}' $keywords`
if [ -f ${baltik_keywords} ]; then
    KeywordsBALTIK=`$TRUST_Awk '!/\|/ {k=k" "$1} /\|/ {gsub("\\\|"," ",$0);k=k" "$0} END {print k}' $baltik_keywords`
else
    KeywordsBALTIK=""
fi

cat > ${path_file} <<EOF
;;; trust-mode.el --- Major mode to manage data file for TRUST-based computations

;; Copyleft 🄯 2024 Alan Burlot

;; Author: Alan Burlot <alan.burlot@cea.fr>
;; Maintainer: Alan Burlot <alan.burlot@cea.fr>
;; Version: 2024.07.05
;; Packages-Requires: ((emacs "26"))
;; Keywords: trust, triocfd, data

;; This file is NOT par of GNU Emacs

;;; License:

;;; Commentary:
;;
;; This file has been adapted from Alan Burlot own trust-mode. Feel free to report any questions.

;;; Code:

(defvar trust-mode-syntax-table nil
  "Syntax table for \`trust-mode'.")

;; remove numbers and signs from
(setq-local listexclude '("0" "1" "2" "<=" "="))
(defvar trust-keywords
  (let ((thekeywords (split-string "${KeywordsTRUST}")))
	     (dolist (elt listexclude thekeywords)
	       (setq thekeywords (delete elt thekeywords)))
	     thekeywords)
  "TRUST keywords.")


(defvar baltik-keywords
  (split-string "${KeywordsBALTIK}")
  "BALTIK keywords.")

(defvar trust-mode-indent-offset 2
  "Indentation offset for \`TRUST-mode'.")

(setq trust-mode-syntax-table
  (let ((trust-table (make-syntax-table)))
    ; The block delimiters
    (modify-syntax-entry ?\{ "(}" trust-table)
    (modify-syntax-entry ?\} "){" trust-table)
    ; Comment style "# toto #"
    (modify-syntax-entry ?# ". 14" trust-table)
    (modify-syntax-entry ?  "- 23" trust-table) ; a dash to keep whitespace as a whitespace!
    ; Comment style "/* toto */" nested
    (modify-syntax-entry ?\/ ". 14bn" trust-table)
    (modify-syntax-entry ?* ". 23bn" trust-table)
    ; Stuff that are words and not delimiters
    (modify-syntax-entry ?_ "w" trust-table)
    (modify-syntax-entry ?\[ "w" trust-table) ; for math function
    (modify-syntax-entry ?\] "w" trust-table) ; for math function
    trust-table))

(defun trust-mode-indent-line ()
  "Indent current line for \`TRUST-mode'."
  ;; Slightly adapted from https://stackoverflow.com/a/4158816/1879269.
  (interactive)
  (let ((indent-col 0))
    (save-excursion
      (beginning-of-line)
      (condition-case nil
          (while t
            (backward-up-list 1)
            (when (looking-at "[{]")
              (setq indent-col (+ indent-col trust-mode-indent-offset))))
        (error nil)))
    (save-excursion
      (back-to-indentation)
      (when (and (looking-at "[}]") (>= indent-col trust-mode-indent-offset))
        (setq indent-col (- indent-col trust-mode-indent-offset))))
    (indent-line-to indent-col)))


;; Completion (inspired by gmsh-mode)
(defvar trust--completion-keywords
  (append trust-keywords baltik-keywords)
  "TRUST completion list.")

;;;###autoload
(defun trust-mode-completion-at-point ()
  "TRUST function to be used for the hook \`completion-at-point-functions'."
  (interactive)
  (let* ((bds (bounds-of-thing-at-point 'symbol))
         (start (car bds))
         (end (cdr bds)))
    (list start end trust--completion-keywords . nil)))

;; Syntax coloring
;; http://xahlee.info/emacs/emacs/elisp_syntax_coloring.html
(defvar trust-font-lock nil
  "Font lock for data file managed by \`TRUST-mode'.")

(setq trust-font-lock
      (let* (;; define several category of keywords
             (x-keywords-trust trust-keywords)
             (x-keywords-baltik baltik-keywords)
             (x-comments '("#" "#"))

             ;; generate regex string for each category of keywords
             (x-keywords-trust-regexp (regexp-opt x-keywords-trust 'words))
             (x-keywords-baltik-regexp (regexp-opt x-keywords-baltik 'words))
             (x-comments-regexp (regexp-opt x-comments 'words))
             )

        \`((,x-comments-regexp . 'font-lock-comment-face)
          (,x-keywords-baltik-regexp . 'font-lock-type-face)
          (,x-keywords-trust-regexp . 'font-lock-keyword-face)
          ;; note: order above matters, because once colored, that part won't change.
          ;; in general, put longer words first. Trio first in case word is overrided.
          )))

;; Mode definition
(define-derived-mode trust-mode text-mode "TRUST-mode"
  "Mode for editing TRUST custom data files."

  ;; = Indentation
  ;; Define offset for indentation
  (make-local-variable 'trust-mode-indent-offset)
  ;; Set the indentation manager
  (set (make-local-variable 'indent-line-function) 'trust-mode-indent-line)

  ;; = Comments managment. We prefer the dash for automatic comments
  (setq-local comment-start "#") ;; Allow to use the comment-region
  (setq-local comment-end "#") ;; and uncomment-region functions
  (setq-local comment-start-skip "/*")
  (setq-local comment-end-skip "*/")
  (setq-local comment-style 'multi-line)

  ;; = Associate the syntax table
  (set-syntax-table trust-mode-syntax-table)

  ;; Font-lock-face
  (setq-local font-lock-defaults '((trust-font-lock)
                                   ;; options mix the behavior with delimiters
                                   ;; nil ;; to allow comment font change
                                   ;; t ;; to allow case insensitive
                                   ))


  ;; Local variables
  (setq-local indent-tabs-mode nil) ;; Tab insert space
  (setq-local tab-width trust-mode-indent-offset) ;; Width of a tab hit
  (setq-local tab-stop-list '(number-sequence trust-mode-indent-offset 40 trust-mode-indent-offset))
  (setq completion-ignore-case t)
  (setq company-minimum-prefix-length 2)

  ;; Awesome completion !
  (add-hook 'completion-at-point-functions 'trust-mode-completion-at-point nil 'local)
  )

;; Font-lock case insensitive
;; Should be in the font-lock-default, but it mixes with the delimiters coloring
(defun case-insensitive-advice ()
  "Add a case advice for some obscur reasons."
  (set (make-local-variable 'font-lock-keywords-case-fold-search) t))
(advice-add 'trust-mode :after #'case-insensitive-advice)

;; Add trust-mode to manage data files.
(add-to-list 'auto-mode-alist '("\\\\.data\\\\'" . trust-mode))

(provide 'trust-mode)
;;; trust-mode.el ends here
EOF

# User message
cat <<EOF

================================================================

    To activate the trust-mode in emacs, follow this steps:
    1. copy ${path_file} in your ~/.emacs.d/ directory
    2. add this line to your init.el file
    (load "~/.emacs.d/trust-mode.el)
    3. You can add the following minor-modes to enhance the buffer
      - rainbow-delimiters-mode (to highlight the braces)
      - indent-bars-mode (for a nice indent indicator)
      - company-mode (for completion!)
EOF
