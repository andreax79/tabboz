SHELL=/bin/bash -e

help:
	@echo - make export ----- Export resources
	@echo - make black ------ Format code
	@echo - make clean ------ Clean virtual environment
	@echo - make venv ------- Create virtual environment

black:
	black -S *.py

clean:
	-rm -rf build dist
	-rm -rf *.egg-info
	-rm -rf bin lib share pyvenv.cfg

venv:
	python3 -m virtualenv .
	. bin/activate; pip install -Ur requirements.txt

export:
	. bin/activate; python export.py ../../*.RES
