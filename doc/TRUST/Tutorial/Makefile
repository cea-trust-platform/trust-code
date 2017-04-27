FILE1=TRUST_tutorial
FILE2=Baltik_tutorial

default:  all

$(FILE1): ../$(FILE1).pdf
../$(FILE1).pdf: $(FILE1).tex Makefile 
	$(MAKE) clean_$(FILE1)
	pdflatex -interaction=nonstopmode $(FILE1).tex > LOG
	makeindex $(FILE1).idx >> LOG
	pdflatex -interaction=nonstopmode $(FILE1).tex >> LOG
	pdflatex -interaction=nonstopmode $(FILE1).tex >> LOG
	mv $(FILE1).pdf ..
	$(MAKE) clean_build

$(FILE2): ../$(FILE2).pdf
../$(FILE2).pdf: $(FILE2).tex Makefile 
	$(MAKE) clean_$(FILE2)
	pdflatex -interaction=nonstopmode $(FILE2).tex > LOG
	pdflatex -interaction=nonstopmode $(FILE2).tex >> LOG
	mv $(FILE2).pdf ..
	$(MAKE) clean_build

all:  $(FILE1) $(FILE2)

clean_build:  
	@echo "Removing compilation files"
	@rm -f *.log *.aux *.dvi *.toc *.lot *.lof *.out *.pdf *.idx *.ilg *.ind *.nav *.snm LOG

clean_$(FILE1):  clean_build
	@echo "Removing $(FILE1).pdf"
	@rm -f ../$(FILE1).pdf

clean_$(FILE2):  clean_build
	@echo "Removing $(FILE2).pdf"
	@rm -f ../$(FILE2).pdf

clean:  clean_$(FILE1) clean_$(FILE2)

.PHONY: clean clean_build
