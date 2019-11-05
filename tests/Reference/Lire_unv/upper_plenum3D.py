from mygeompy import *
from math import sin,cos,asin

# Donnees geometriques ----------------------------------------------------------------------------------#
print "donnees geometriques"
rayon_plenum=3.55
maille_DHR=0.366
maille_IHX=0.4
longueur_cylindre_DHR=rayon_plenum+maille_DHR
longueur_cylindre_IHX=rayon_plenum+maille_IHX

rayon_IHX=0.775
rayon_DHR=0.8

hauteur_calotte=2.5
hauteur_cylindre=8.75
# hauteurs a preciser
hauteur_DHR=2.5
hauteur_IHX=2.5

pi=asin(1.)*2.
ang1_DHR=pi/3.
ang2_DHR=pi
ang3_DHR=5*pi/3.

ang1_IHX=0.
ang2_IHX=2*pi/3.
ang3_IHX=4*pi/3.


# Points de construction ----------------------------------------------------------------------------------#
print "Points de construction"

point_bas=MakeVertex(0,0,0)
point_haut_cylindre=MakeVertex(0,0,hauteur_cylindre)
point_ox=MakeVertex(1,0,0)
point_hauteur_DHR=MakeVertex(0,0,hauteur_DHR)
point_hauteur_IHX=MakeVertex(0,0,hauteur_IHX)

# Points pour le trace des cylindres DHR et IHX #
point_vecteur_DHR1=MakeVertex(cos(ang1_DHR),sin(ang1_DHR),0)
point_vecteur_DHR2=MakeVertex(cos(ang2_DHR),sin(ang2_DHR),0)
point_vecteur_DHR3=MakeVertex(cos(ang3_DHR),sin(ang3_DHR),0)

point_vecteur_IHX1=MakeVertex(cos(ang1_IHX),sin(ang1_IHX),0)
point_vecteur_IHX2=MakeVertex(cos(ang2_IHX),sin(ang2_IHX),0)
point_vecteur_IHX3=MakeVertex(cos(ang3_IHX),sin(ang3_IHX),0)

# Points servant a definir les Conditions limites avec la commande create_bord_near en 3D#
# DHR #
point_face_DHR1=MakeVertex(longueur_cylindre_DHR*cos(ang1_DHR),longueur_cylindre_DHR*sin(ang1_DHR),hauteur_DHR)
point_face_DHR2=MakeVertex(longueur_cylindre_DHR*cos(ang2_DHR),longueur_cylindre_DHR*sin(ang2_DHR),hauteur_DHR)
point_face_DHR3=MakeVertex(longueur_cylindre_DHR*cos(ang3_DHR),longueur_cylindre_DHR*sin(ang3_DHR),hauteur_DHR)
#addToStudy(point_face_DHR1,"pts_DHR1")
#addToStudy(point_face_DHR2,"pts_DHR2")
#addToStudy(point_face_DHR3,"pts_DHR3")

# IHX #
point_face_IHX1=MakeVertex(longueur_cylindre_IHX*cos(ang1_IHX),longueur_cylindre_IHX*sin(ang1_IHX),hauteur_IHX)
point_face_IHX2=MakeVertex(longueur_cylindre_IHX*cos(ang2_IHX),longueur_cylindre_IHX*sin(ang2_IHX),hauteur_IHX)
point_face_IHX3=MakeVertex(longueur_cylindre_IHX*cos(ang3_IHX),longueur_cylindre_IHX*sin(ang3_IHX),hauteur_IHX)
#addToStudy(point_face_IHX1,"pts_IHX1")
#addToStudy(point_face_IHX2,"pts_IHX2")
#addToStudy(point_face_IHX3,"pts_IHX3")

# Vecteurs de construction ----------------------------------------------------------------------------------- #
print "Vecteurs de construction"

vecteur_cuve=MakeVector(point_bas,point_haut_cylindre)
axe_ox=MakeVector(point_bas,point_ox)

vecteur_DHR1=MakeVector(point_bas,point_vecteur_DHR1)
vecteur_DHR2=MakeVector(point_bas,point_vecteur_DHR2)
vecteur_DHR3=MakeVector(point_bas,point_vecteur_DHR3)

vecteur_IHX1=MakeVector(point_bas,point_vecteur_IHX1)
vecteur_IHX2=MakeVector(point_bas,point_vecteur_IHX2)
vecteur_IHX3=MakeVector(point_bas,point_vecteur_IHX3)


# Construction  et fusion des cylindres ---------------------------------------------------------------------- #
print "construction cylindre"
cylindre_plenum=MakeCylinder(point_bas,vecteur_cuve,rayon_plenum,hauteur_cylindre)

print "construction DHR"
cylindre_DHR1=MakeCylinder(point_hauteur_DHR,vecteur_DHR1,rayon_DHR,longueur_cylindre_DHR)
cylindre_DHR2=MakeCylinder(point_hauteur_DHR,vecteur_DHR2,rayon_DHR,longueur_cylindre_DHR)
cylindre_DHR3=MakeCylinder(point_hauteur_DHR,vecteur_DHR3,rayon_DHR,longueur_cylindre_DHR)
# addToStudy(cylindre_DHR1,"cy_DHR1")
# addToStudy(cylindre_DHR2,"cy_DHR2")
# addToStudy(cylindre_DHR3,"cy_DHR3")

print "construction IHX"
cylindre_IHX1=MakeCylinder(point_hauteur_IHX,vecteur_IHX1,rayon_IHX,longueur_cylindre_IHX)
cylindre_IHX2=MakeCylinder(point_hauteur_IHX,vecteur_IHX2,rayon_IHX,longueur_cylindre_IHX)
cylindre_IHX3=MakeCylinder(point_hauteur_IHX,vecteur_IHX3,rayon_IHX,longueur_cylindre_IHX)

# addToStudy(cylindre_IHX1,"cy_IHX1")
# addToStudy(cylindre_IHX2,"cy_IHX2")
# addToStudy(cylindre_IHX3,"cy_IHX3")

F1=MakeFuse(cylindre_plenum,cylindre_DHR1)
F1=MakeFuse(F1,cylindre_DHR2)
F1=MakeFuse(F1,cylindre_DHR3)
F1=MakeFuse(F1,cylindre_IHX1)
F1=MakeFuse(F1,cylindre_IHX2)
F1=MakeFuse(F1,cylindre_IHX3)

# Construction  et fusion de la calotte spherique ------------------------------------------------------------ #
print "construction calotte superieure"
# Problemes de construction avec une ellipse lie a un bug dans Salome pour l'instruction fuse. Premiere 
# tentative avec une sphere
# le 27/3/06, Gauthier cree une commande 'ellipsoide' qui va bien
#
# ellipse=MakeEllipse(point_haut_cylindre,axe_ox,rayon_plenum,hauteur_calotte)
# calotte_sup=MakeRevolution(ellipse,vecteur_cuve,pi)
# calotte_sup=MakeSpherePntR(point_haut_cylindre,rayon_plenum)
#
calotte_sup=MakeEllipsoide(0.,0.,hauteur_cylindre,rayon_plenum,rayon_plenum,hauteur_calotte)
# addToStudy(calotte_sup,"c")

F1=MakeFuse(F1,calotte_sup)
addToStudy(F1,"F1")


# Creation des groupes: conditions aux limites ------------------------------------------------------------------

# le 27/3/06 methode create_fgroup dans mon repertoire, qui devra etre mise en commun
# calcul = F1: Gauthier tient a ce que le pb s'appelle calcul pour lui permettre d'importer ses fichiers 
# sans se poser de questions

from create_group import *
calcul=F1
id_calcul=addToStudy(calcul,"calcul")

# la liste des groupes est un tableau
list_grp_def=[]

# cherche le bord le plus proche du point de bord: ne marche que en 3D
# create_bord_near("gauche",calcul, [PointCoordinates(point_face_DHR1),PointCoordinates(point_face_DHR2)],list_grp_def)
create_bord_near("cl_DHR1",calcul, [PointCoordinates(point_face_DHR1)],list_grp_def)
create_bord_near("cl_DHR2",calcul, [PointCoordinates(point_face_DHR2)],list_grp_def)
create_bord_near("cl_DHR3",calcul, [PointCoordinates(point_face_DHR3)],list_grp_def)
create_bord_near("cl_IHX1",calcul, [PointCoordinates(point_face_IHX1)],list_grp_def)
create_bord_near("cl_IHX2",calcul, [PointCoordinates(point_face_IHX2)],list_grp_def)
create_bord_near("cl_IHX3",calcul, [PointCoordinates(point_face_IHX3)],list_grp_def)
create_bord_near("cl_coeur",calcul, [PointCoordinates(point_bas)],list_grp_def)

# On met dans le groupe defaut (ou paroi pour nous, toutes les Conditions limites qui n'ont pas ete definies
list_grp=list_grp_def

# defaut,list_grp=create_defaut("defaut",calcul,list_grp)
defaut,list_grp=create_defaut("paroi",calcul,list_grp)

# permet le display de la geometrie dans la fenetre ------------------------------------------------------------
import salome
salome.sg.updateObjBrowser(1)



print "DONE"
