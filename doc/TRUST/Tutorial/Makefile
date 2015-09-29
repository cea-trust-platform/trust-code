all: clean
	pdflatex TRUST_tutorial.tex
	makeindex TRUST_tutorial.idx
	pdflatex TRUST_tutorial.tex
	pdflatex TRUST_tutorial.tex


clean:
	@echo "Removing compilation files and pdf"
	@rm -f *.log *.aux *.dvi *.toc *.lot *.lof *.out *.pdf *.idx *.ilg *.ind *.nav *.snm
