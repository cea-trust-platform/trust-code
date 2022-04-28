#!/bin/bash

echo "Usage: $0 file"

data=$1.data
result=${PWD##*/}

if [ -d "src/" ] 
then
	echo "=========================="
	echo "src directory exists"
	cd src
elif [ "$result" = "src" ]
then
	echo "=========================="
	echo "src directory exists"
else
	echo "Directory src/ DOES NOT exists."
	exit
fi

if [ "$1" = "" ] || [ ! -f $data ]
then
	echo "$data does not exists in src."
else
	echo "=========================="
	echo "$data exists."
fi
cd ../

python $TRUST_ROOT/bin/KSH/create_initial_notebook.py -i $1

echo "-> Generate jupyter notebook... OK"
echo "=========================="
echo "You can run: Run_fiche "
echo "and edit your notebook"
echo "=========================="
