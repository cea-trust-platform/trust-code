#!/bin/bash

#echo "Usage: $0 file"

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

datas=`ls *.data 2>/dev/null`
if [ "$datas" = "" ]
then
	echo "no datafile in src."
	exit
fi
cd ../
python $TRUST_ROOT/bin/KSH/create_initial_notebook.py -n $1 -d $datas -t $2

echo "-> Generate jupyter notebook... OK"
echo "=========================="
echo "You can run: Run_fiche "
echo "and edit your notebook"
echo "=========================="
