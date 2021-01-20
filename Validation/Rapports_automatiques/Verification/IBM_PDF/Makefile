all: fiche

only_fiche:
	Run_fiche -xpdf

fiche: partition
	Run_fiche -xpdf

partition:
	python partition.py

clean:
	python clean.py