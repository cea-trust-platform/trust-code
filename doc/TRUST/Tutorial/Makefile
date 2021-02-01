FILE1=../TRUST_tutorial.pdf
FILE2=../Baltik_tutorial.pdf
FILE3=../ICoCo_tutorial.pdf

default:  all

$(FILE1): TRUST_tutorial.tex Makefile
ifeq (, $(shell which pdflatex 2>/dev/null))
	@echo "No pdflatex command, consider installing texlive package"
else
	$(MAKE) clean_TRUST_tutorial
	pdflatex -interaction=nonstopmode TRUST_tutorial.tex > LOG
	makeindex TRUST_tutorial.idx >> LOG
	pdflatex -interaction=nonstopmode TRUST_tutorial.tex >> LOG
	pdflatex -interaction=nonstopmode TRUST_tutorial.tex >> LOG
	mv TRUST_tutorial.pdf ..
	$(MAKE) clean_build
endif

$(FILE2): Baltik_tutorial.tex Makefile
ifeq (, $(shell which pdflatex 2>/dev/null))
	@echo "No pdflatex command, consider installing texlive package"
else
	$(MAKE) clean_Baltik_tutorial
	pdflatex -interaction=nonstopmode Baltik_tutorial.tex > LOG
	pdflatex -interaction=nonstopmode Baltik_tutorial.tex >> LOG
	mv Baltik_tutorial.pdf ..
	$(MAKE) clean_build
endif

$(FILE3): ICoCo_tutorial.tex Makefile
ifeq (, $(shell which pdflatex 2>/dev/null))
	@echo "No pdflatex command, consider installing texlive package"
else
	$(MAKE) clean_ICoCo_tutorial
	pdflatex -interaction=nonstopmode ICoCo_tutorial.tex > LOG
	pdflatex -interaction=nonstopmode ICoCo_tutorial.tex >> LOG
	mv ICoCo_tutorial.pdf ..
	$(MAKE) clean_build
endif

all:  $(FILE1) $(FILE2) $(FILE3)

clean_build:  
	@echo "Removing compilation files"
	@rm -f *.log *.aux *.dvi *.toc *.lot *.lof *.out *.pdf *.idx *.ilg *.ind *.nav *.snm LOG

clean_TRUST_tutorial:  clean_build
	@echo "Removing TRUST_tutorial.pdf"
	@rm -f ../TRUST_tutorial.pdf

clean_Baltik_tutorial:  clean_build
	@echo "Removing Baltik_tutorial.pdf"
	@rm -f ../Baltik_tutorial.pdf

clean_ICoCo_tutorial:  clean_build
	@echo "Removing ICoCo_tutorial.pdf"
	@rm -f ../ICoCo_tutorial.pdf

clean:  clean_build

.PHONY: clean clean_build
