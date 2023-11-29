FILE2=mini_Reference_Manual
FILE3=mini_prm
FILE4=mini_jupyter


default:  $(FILE2) $(FILE3)

jupyter: $(FILE4)

$(FILE2): $(FILE2).pdf
$(FILE2).pdf: $(FILE2).tex Makefile 
	pdflatex -interaction=nonstopmode $(FILE2).tex > LOG

$(FILE3): $(FILE3).pdf
$(FILE3).pdf: $(FILE3).tex Makefile 
	pdflatex -interaction=nonstopmode $(FILE3).tex > LOG

$(FILE4): $(FILE4).pdf
$(FILE4).pdf: $(FILE4).tex Makefile 
	xelatex -interaction=nonstopmode $(FILE4).tex > LOG
