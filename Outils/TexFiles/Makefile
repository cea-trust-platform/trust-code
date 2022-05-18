FILE1=mini_Generic_Guide
FILE2=mini_Reference_Manual
FILE3=mini_Tutorial
FILE4=mini_prm
FILE5=mini_jupyter


default:  $(FILE1) $(FILE2) $(FILE3) $(FILE4) 

jupyter: $(FILE5)

$(FILE1): $(FILE1).pdf
$(FILE1).pdf: $(FILE1).tex Makefile 
	pdflatex -interaction=nonstopmode $(FILE1).tex > LOG

$(FILE2): $(FILE2).pdf
$(FILE2).pdf: $(FILE2).tex Makefile 
	pdflatex -interaction=nonstopmode $(FILE2).tex > LOG

$(FILE3): $(FILE3).pdf
$(FILE3).pdf: $(FILE3).tex Makefile 
	pdflatex -interaction=nonstopmode $(FILE3).tex > LOG

$(FILE4): $(FILE4).pdf
$(FILE4).pdf: $(FILE4).tex Makefile 
	pdflatex -interaction=nonstopmode $(FILE4).tex > LOG

$(FILE5): $(FILE5).pdf
$(FILE5).pdf: $(FILE5).tex Makefile
	xelatex -interaction=nonstopmode $(FILE5).tex > LOG

