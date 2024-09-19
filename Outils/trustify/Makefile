clean:
	@rm -rfv **/*.egg-info .pytest_cache/ build/
	@find . -name __pycache__ -exec rm -rfv {} +

install:
	@pip install -e .

test:
	@env python -m unittest  

help:
	@echo The following make targets are available
	@echo    - install: install the 'trustify' package
	@echo    - clean: cleans everything installed by the package
	@echo    - test: launch the unittests of the trustify package
	@echo    - help: displays this message

.PHONY: help clean install test
