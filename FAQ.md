# **FAQ**

**Qu'a t'on besoin pour utiliser TRUST ?**

- Avoir une machine Unix ayant au moins 64 Mo de RAM et 400 Mo de place disque. 

- L'IHM de TRUST necessite la presence du navigateur Netscape et du script ksh.

**Qu'a t'on besoin pour developpeur TRUST ?**

- Idem que precedemment plus un compilateur C++ et Fortran et environ 200Mo de place disque supplementaire.

**Que signifie l'erreur "Erreur dans OctreeRoot::rang_sommet"?, du au maillage.**

- Il s'agit d'un probleme de realisation du maillage avec le mailleur interne de TRUST. Bien souvent il s'agit d'un mauvais decoupage d'un pave, avec un nombre de sommets trop petit ou trop grand. Verifier les pas du maillage sur chaque pave et s'assurer que le maillage global sera donc bien conforme.

**Peut t'on nommer d'un meme nom des bords differents dans un fichier de maillage ?**

- Oui, et par la suite le code considerera les bords nommes identiquement comme un seul et unique bord pour lequel une seule condition peut etre  appliquee. Cette facon de faire est meme recommandee.

**Quelle est la facon de faire pour appliquer une condition de periodicite ?**

- On doit nommer les bords ou s'applique la periodicite selon une direction avec un seul meme nom, sinon une erreur se produit.

**Le bouton droit de la souris ne marche pas avec Meshtv sur Linux ?**

- Le pave numerique est active, desactivez le.
