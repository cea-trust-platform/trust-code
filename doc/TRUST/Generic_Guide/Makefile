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
	cp $(FILE).tex $(FILE)_new.tex
	cp $(FILE2).tex $(FILE2)_new.tex
	cp $(FILE3).tex $(FILE3)_new.tex
	cp $(FILE4).tex $(FILE4)_new.tex
	cp $(FILE5).tex $(FILE5)_new.tex
	cp $(FILE7).tex $(FILE7)_new.tex
	sed -i "s/chap2_refs/chap2_refs_new/" $(FILE)_new.tex
	sed -i "s/chap3_data_setting/chap3_data_setting_new/" $(FILE)_new.tex
	sed -i "s/chap4_problem_definition/chap4_problem_definition_new/" $(FILE)_new.tex
	sed -i "s/chap5_ending/chap5_ending_new/" $(FILE)_new.tex
	sed -i "s/chap7_parallel/chap7_parallel_new/" $(FILE)_new.tex
	python bookmarks.py -p ../User_Manual_TRUST.pdf -t $(FILE)_new.tex
	python bookmarks.py -p ../User_Manual_TRUST.pdf -t $(FILE2)_new.tex
	python bookmarks.py -p ../User_Manual_TRUST.pdf -t $(FILE3)_new.tex
	python bookmarks.py -p ../User_Manual_TRUST.pdf -t $(FILE4)_new.tex
	python bookmarks.py -p ../User_Manual_TRUST.pdf -t $(FILE5)_new.tex
	python bookmarks.py -p ../User_Manual_TRUST.pdf -t $(FILE7)_new.tex
	pdflatex -interaction=nonstopmode $(FILE)_new.tex >> LOG
	pdflatex -interaction=nonstopmode $(FILE)_new.tex >> LOG
	mv $(FILE)_new.pdf ../TRUST_$(FILE).pdf
	$(MAKE) clean_build


clean_build:  
	@echo "Removing compilation files"
	@rm -f *.log *.aux *.toc *.out LOG *_new.tex ../*bookmarks.txt
clean:  clean_build
	@rm -f $(FILE).pdf
.PHONY: clean clean_build
