FILE=Tutorial_solutions

all: ../$(FILE).pdf
../$(FILE).pdf: $(FILE).tex Makefile
	$(MAKE) clean
	pdflatex -interaction=nonstopmode $(FILE).tex > LOG
#	 makeindex $(FILE).idx >> LOG
	pdflatex -interaction=nonstopmode  $(FILE).tex >> LOG
	pdflatex -interaction=nonstopmode  $(FILE).tex >> LOG
	mv $(FILE).pdf ..
	$(MAKE) clean_build


clean_build:  
	@echo "Removing compilation files and pdf"
	@rm -f *.log *.aux *.dvi *.toc *.lot *.lof *.out *.pdf *.idx *.ilg *.ind *.nav *.snm LOG
clean:  clean_build
	@rm -f ../$(FILE).pdf
.PHONY: clean clean_build
