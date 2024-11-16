org=$(TRUST_ROOT)/Outils/trustify

all: Makefile clean install check 

install: install.sh 
	@env TRUSTIFY_FROM_TRUST=1 ./install.sh

check:
	@env TRUSTIFY_FROM_TRUST=1 ./tools/check_all.sh
	
doc: install
	@env TRUSTIFY_FROM_TRUST=1 ./tools/gen_doc.sh

clean:
	@rm -rfv **/*.egg-info .pytest_cache/ build/ test/generated/* install/
	@find . -name __pycache__ -exec rm -rfv {} +

# For developpers:
test:
	@(source $(org)/env.sh && env python -m unittest test.test_rw_elementary.TestCase )
	@(source $(org)/env.sh && env python -m unittest test.test_rw_full_datasets.TestCase )
	@(source $(org)/env.sh && env jupyter-nbconvert --to html --no-input --output $(TRUST_TMP)/trustify_ex.html --execute doc/examples/hello_world/HelloWorld.ipynb )

test_debug:
	@(source $(org)/env.sh && env TRUSTIFY_DEBUG=1 python -m unittest test.test_rw_elementary.TestCase )
	@(source $(org)/env.sh && env TRUSTIFY_DEBUG=1 python -m unittest test.test_rw_full_datasets.TestCase )

help:
	@echo The following make targets are available
	@echo    - all: will clean, install and finally execute the check target.
	@echo    - install: install the 'trustify' package
	@echo    - check: check all TRUST datasets
	@echo    - doc: generate documentation for keywords
	@echo    - clean: cleans everything installed by the package
	@echo    - test: launch the unittests of the trustify package (for developpers)
	@echo    - test_debug: launch the unittests of the trustify package in debug mode (very verbose!)  (for developpers)
	@echo    - help: displays this message

.PHONY: help clean install test test_debug check all doc
