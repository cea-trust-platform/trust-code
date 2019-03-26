dirs = Tutorial exercices/Tutorial_solutions Generic_Guide
all: Keywords.Vim
ifeq ($(TRUST_WITHOUT_PDFLATEX),1)
	@echo "TRUST_WITHOUT_PDFLATEX=1 ... so we will not build the PDF documentation."
else
	@for d in $(dirs) ;do (cd $$d; $(MAKE)); done
endif

Keywords.Vim: Keywords.txt
	configure_vim.sh build_syntax

clean:
	rm -f Keywords.Vim
	@for d in $(dirs) ;do (cd $$d; $(MAKE) clean ); done
