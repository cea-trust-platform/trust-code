FILE1=TRUST_tutorial
FILE2=Baltik_tutorial
FILE3=Readme_eclipse
FILE4=ICoCo_tutorial

default:  all

$(FILE1): ../$(FILE1).pdf
../$(FILE1).pdf: $(FILE1).tex Makefile 
ifeq (, $(shell which pdflatex))
	@echo "No pdflatex command, consider installing texlive package"
else
	$(MAKE) clean_$(FILE1)
	pdflatex -interaction=nonstopmode $(FILE1).tex > LOG
	makeindex $(FILE1).idx >> LOG
	pdflatex -interaction=nonstopmode $(FILE1).tex >> LOG
	pdflatex -interaction=nonstopmode $(FILE1).tex >> LOG
	mv $(FILE1).pdf ..
	$(MAKE) clean_build
endif

$(FILE2): ../$(FILE2).pdf
../$(FILE2).pdf: $(FILE2).tex Makefile 
ifeq (, $(shell which pdflatex))
	@echo "No pdflatex command, consider installing texlive package"
else
	$(MAKE) clean_$(FILE2)
	pdflatex -interaction=nonstopmode $(FILE2).tex > LOG
	pdflatex -interaction=nonstopmode $(FILE2).tex >> LOG
	mv $(FILE2).pdf ..
	$(MAKE) clean_build
endif

$(FILE3): ../$(FILE3).pdf
../$(FILE3).pdf: $(FILE3).tex Makefile 
ifeq (, $(shell which pdflatex))
	@echo "No pdflatex command, consider installing texlive package"
else
	$(MAKE) clean_$(FILE3)
	pdflatex -interaction=nonstopmode $(FILE3).tex > LOG
	pdflatex -interaction=nonstopmode $(FILE3).tex >> LOG
	mv $(FILE3).pdf $(TRUST_ROOT)/src
	$(MAKE) clean_build
endif

$(FILE4): ../$(FILE4).pdf
../$(FILE4).pdf: $(FILE4).tex Makefile 
ifeq (, $(shell which pdflatex))
	@echo "No pdflatex command, consider installing texlive package"
else
	$(MAKE) clean_$(FILE4)
	pdflatex -interaction=nonstopmode $(FILE4).tex > LOG
	pdflatex -interaction=nonstopmode $(FILE4).tex >> LOG
	mv $(FILE4).pdf ..
	$(MAKE) clean_build
endif

all:  $(FILE1) $(FILE2) $(FILE3) $(FILE4)

clean_build:  
	@echo "Removing compilation files"
	@rm -f *.log *.aux *.dvi *.toc *.lot *.lof *.out *.pdf *.idx *.ilg *.ind *.nav *.snm LOG

clean_$(FILE1):  clean_build
	@echo "Removing $(FILE1).pdf"
	@rm -f ../$(FILE1).pdf

clean_$(FILE2):  clean_build
	@echo "Removing $(FILE2).pdf"
	@rm -f ../$(FILE2).pdf

clean_$(FILE3):  clean_build
	@echo "Removing $(FILE3).pdf"
	@rm -f ../$(FILE3).pdf

clean_$(FILE4):  clean_build
	@echo "Removing $(FILE4).pdf"
	@rm -f ../$(FILE4).pdf

clean:  clean_$(FILE1) clean_$(FILE2) clean_$(FILE3) clean_$(FILE4)

.PHONY: clean clean_build
