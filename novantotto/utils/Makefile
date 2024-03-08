SHELL=/bin/bash -e

help:
	@echo - make black ------ Format code
	@echo - make clean ------ Clean virtual environment
	@echo - make venv ------- Create virtual environment

black:
	black -S *.py

clean:
	-rm -rf build dist __pycache__
	-rm -rf *.egg-info
	-rm -rf bin lib share pyvenv.cfg lib64

venv:
	python3 -m venv .
	. bin/activate; pip install -Ur requirements.txt
