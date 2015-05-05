all:	test

test:
	@rm -f all
	for rep in `\ls`;do [ -d $$rep ] && echo $$rep;done > all
	echo all | lance_test
	@rm -f all
	@echo
