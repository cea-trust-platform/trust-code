22/11/2013 Update:
In archives/med-3.2.1.tar.gz, the .html,.map,.jpg files
have been removed to gain space disk and, above all, avoid
a slowness during install (/usr/bin/install so slow...)
on CCRT clusters (cd med-3.2.1/doc/html.dox ; rm -f *.html *.map *.jpg; Makefile.in et Makefile.am modified)



Procedure d'installation de MED et de HDF5
==========================================

La procedure  d'installation de  MED a ete  modifiee pour  permettre a
l'utilisateur   de   choisir   entre   l'utilisation   d'une   version
pre-installee (par  exemple par un environnement  Salome ou Alliances)
et l'installation d'une version fournie par le noyau.

Par ailleurs, Trio-U  est livre avec les archives  de deux versions de
MED (med-3.2.1 et med-2.3.6) ainsi que celles de deux versions de HDF5
(hdf5-1.8.14  et  hdf5-1.6.9).    La  procedure  d'installation  permet
egalement a  l'utilisateur de specifier  la version de MED  qu'il veut
utiliser.

L'utilisateur peut specifier la configuration de l'installation de MED
en definissant differentes variables d'environnement :

 - TRUST_USE_EXTERNAL_HDF :  cette variable permet d'indiquer  que l'on veut
   utiliser une version pre-installee de  HDF5. Si elle est definie et
   si sa valeur n'est pas  une chaine de caracteres vide, on utilisera
   une version pre-installee. Dans  le cas contraire, on utilisera une
   version fournie par le noyau Trio-U.

 - HDF5_ROOT_DIR : si on desire utiliser une version pre-installee
   de  HDF5, cette variable  doit indiquer  le repertoire  dans lequel
   cette version a deja ete installee.

 - TRUST_USE_EXTERNAL_MED :  cette variable permet d'indiquer  que l'on veut
   utiliser une version  pre-installee de MED. Si elle  est definie et
   si sa valeur n'est pas  une chaine de caracteres vide, on utilisera
   une version pre-installee. Dans  le cas contraire, on utilisera une
   version fournie par le noyau Trio-U.

 - MEDFILE_ROOT_DIR : si on desire utiliser une version pre-installee
   de  MED, cette  variable doit  indiquer le  repertoire  dans lequel
   cette version a deja ete installee.

 - TRUST_USE_MED_VERSION  : cette variable  permet de  choisir le  couple de
   numeros de versions de MED et de HDF5 que l'on desire utiliser.
    - si elle vaut 3.2, on choisit le couple (MED-3.2, HDF5-1.8) ;
    - si elle vaut 2.3, on choisit le couple (MED-2.3, HDF5-1.6) ;
    - si elle n'est pas definie, on choisit  (MED-3.2, HDF5-1.8). 


Remarque 1
----------
Les tests utilises sont du genre :

if test "x_${TRUST_USE_EXTERNAL_HDF}_x" = "x__x"
then
  ...
fi

Donc si on a :

$ export TRUST_USE_EXTERNAL_HDF=""
 
alors  on utilise  la version  de HDF5  fournie par  le noyau.  Si par
contre, si on a :

$ export TRUST_USE_EXTERNAL_HDF=1
 
alors on utilise une version pre-existante.


Remarque 2 
----------
Un certain nombre de verifications sont realisees avant l'execution de
la procedure d'installation :

 - est-ce que les numeros de version choisis sont coherents ?

 - est-ce  que, si  on utilise  un MED  ou un  HDF5  pre-installe, les
   repertoires  indiques  par  MEDFILE_ROOT_DIR ou  HDF5_ROOT_DIR
   contiennent bien une installation valide de MED ou HDF5 ?

 - est-on en  train d'installer  une version fournie  par le  noyau de
   HDF5  et une  version pre-installee  de MED  (c'est  interdit, bien
   entendu) ?

 - ...


Remarque 3  
----------
Lors  de l'utilisation  d'une  installation pre-existante  de MED,  il
n'est  pas necessaire de  specifier la  variable TRUST_USE_MED_VERSION  : le
numero  de   la  version  choisie  sera  detectee   par  la  procedure
d'installation.   Cependant,   si  TRUST_USE_MED_VERSION  est   definie,  la
procedure  d'installation  verifiera l'adequation  entre  le choix  de
version  indique  par  TRUST_USE_MED_VERSION,  et la  version  effectivement
installee  dans  le  repertoire d'installation  (HDF5_ROOT_DIR  ou
MEDFILE_ROOT_DIR).

