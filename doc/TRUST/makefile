dirs = Tutorial exercices/Tutorial_solutions/
all: Keywords.Vim
	@for d in $(dirs) ;do (cd $$d; $(MAKE)); done
Keywords.Vim: Keywords.txt
	configure_vim.sh build_syntax

clean:
	rm -f Keywords.Vim
	@for d in $(dirs) ;do (cd $$d; $(MAKE) clean ); done
