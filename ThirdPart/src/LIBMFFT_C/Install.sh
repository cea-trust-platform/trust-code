#!/bin/bash

OPT=_opt
export rep_dev=`pwd`
Makeatelier.sccs NO_IHM -1 depend
# on detruit le Makefile cree par l'atelier
rm Makefile
cd $rep_dev/exec$OPT
make MALIB=malib.a malib.a 
ar dv malib.a info_atelier.o

cp malib.a $1

#rm -rf src/exec* sh_install build makefile" > makefile
