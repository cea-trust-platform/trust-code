FILE=Tutorial_solutions
SUB_FILE1=Canal
SUB_FILE2=diagonal_VEF
SUB_FILE3=FTD_all_VDF
SUB_FILE4=Marche3D_keps
SUB_FILE5=Obstacle_reprise
SUB_FILE6=P1toP1Bulle
SUB_FILE7=prepare
SUB_FILE8=Tutorial_solutions
SUB_FILE9=TP_Temp_QC_VEF
SUB_FILE10=DEC_Obstacle
SUB_FILE11=docond_impl
SUB_FILE12=gmsh_3D
SUB_FILE13=Marche3D
SUB_FILE14=Obstacle_VDF
SUB_FILE15=PAR_Obstacle
SUB_FILE16=TP_Temp_QC_VEF_impl
SUB_FILE17=post_run
SUB_FILE18=P1toP1Bulle_impl
SUB_FILE19=diagonal
SUB_FILE20=docond
SUB_FILE21=gmsh
SUB_FILE22=Marche

all: ../$(FILE).pdf
../$(FILE).pdf: $(FILE).tex Makefile $(SUB_FILE1).tex $(SUB_FILE2).tex $(SUB_FILE3).tex $(SUB_FILE4).tex $(SUB_FILE5).tex $(SUB_FILE6).tex $(SUB_FILE7).tex $(SUB_FILE8).tex $(SUB_FILE9).tex $(SUB_FILE10).tex $(SUB_FILE11).tex $(SUB_FILE12).tex $(SUB_FILE13).tex $(SUB_FILE14).tex $(SUB_FILE15).tex $(SUB_FILE16).tex $(SUB_FILE17).tex $(SUB_FILE18).tex $(SUB_FILE19).tex $(SUB_FILE20).tex $(SUB_FILE21).tex $(SUB_FILE22).tex
ifeq (, $(shell which pdflatex))
	@echo "No pdflatex command, consider installing texlive package"
else
	$(MAKE) clean
	pdflatex -interaction=nonstopmode $(FILE).tex > LOG
#	 makeindex $(FILE).idx >> LOG
	pdflatex -interaction=nonstopmode  $(FILE).tex >> LOG
	pdflatex -interaction=nonstopmode  $(FILE).tex >> LOG
	mv $(FILE).pdf ..
	$(MAKE) clean_build
endif

clean_build:  
	@echo "Removing compilation files and pdf"
	@rm -f *.log *.aux *.dvi *.toc *.lot *.lof *.out *.pdf *.idx *.ilg *.ind *.nav *.snm LOG
clean:  clean_build
	@rm -f ../$(FILE).pdf
.PHONY: clean clean_build
