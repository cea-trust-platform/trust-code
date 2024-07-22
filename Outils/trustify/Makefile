clean:
	@rm -rf **/*.egg-info .pytest_cache/ build/
	@find . -name __pycache__ -exec rm -rfv {} +

install:
	@pip install -e .

test:
	@env python -m unittest test.test_rw_elementary.TestCase
	@env python -m unittest test.test_rw_full_datasets.TestCase

test_debug:
	@env TRUSTIFY_DEBUG=1 python -m unittest test.test_rw_elementary.TestCase
	@env TRUSTIFY_DEBUG=1 python -m unittest test.test_rw_full_datasets.TestCase

help:
	@echo The following make targets are available
	@echo    - install: install the 'trustify' package
	@echo    - clean: cleans everything installed by the package
	@echo    - test: launch the unittests of the trustify package
	@echo    - help: displays this message

.PHONY: help clean install test
