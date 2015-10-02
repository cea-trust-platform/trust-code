FILE=TRUST_tutorial

all: ../$(FILE).pdf 
../$(FILE).pdf: $(FILE).tex Makefile 
	$(MAKE) clean
	pdflatex $(FILE).tex > LOG
	makeindex $(FILE).idx >> LOG
	pdflatex $(FILE).tex >> LOG
	pdflatex $(FILE).tex >> LOG
	mv $(FILE).pdf ..
	$(MAKE) clean_build


clean_build:  
	@echo "Removing compilation files and pdf"
	@rm -f *.log *.aux *.dvi *.toc *.lot *.lof *.out *.pdf *.idx *.ilg *.ind *.nav *.snm LOG
clean:  clean_build
	@rm -f ../$(FILE).pdf
.PHONY: clean clean_build
