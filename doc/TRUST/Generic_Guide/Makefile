FILE=Generic_Guide
FILE2=chap2_refs
FILE3=chap3_data_setting
FILE4=chap4_problem_definition
FILE5=chap5_ending
FILE6=chap6_post_processing
FILE7=chap7_parallel

all: ../$(FILE).pdf 
../$(FILE).pdf: $(FILE).tex $(FILE2).tex $(FILE3).tex $(FILE4).tex $(FILE5).tex $(FILE6).tex chap1_intro.tex mini.tex Makefile 
	$(MAKE) clean
	pdflatex -interaction=nonstopmode $(FILE).tex >> LOG
	pdflatex -interaction=nonstopmode $(FILE).tex >> LOG
	mv $(FILE).pdf ../TRUST_$(FILE).pdf
	$(MAKE) clean_build


clean_build:  
	@echo "Removing compilation files"
	@rm -f *.log *.aux *.toc *.out LOG
clean:  clean_build
	@rm -f ../TRUST_$(FILE).pdf
.PHONY: clean clean_build
