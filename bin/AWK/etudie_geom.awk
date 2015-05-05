BEGIN { getline
domaine=mot_sans_casse()
a=mot()
sommets=mot()
dimension=mot()
a=mot()
xmin=1e7;ymin=xmin;zmin=xmin;
xmax=-xmin;ymax=-xmin;zmax=-xmin;
for (ns=0;ns<sommets;ns++) {
   x=mot()
   if (x<xmin) xmin=x
   else if (x>xmax) xmax=x
   y=mot()
   if (y<ymin) ymin=y
   else if (y>ymax) ymax=y
   if (dimension==3) {
      z=mot()
      if (z<zmin) zmin=z
      else if (z>zmax) zmax=z
   }
}
mailles=0
ibord=0
}

($1=="{") && (!mailles) {
  zone=mot_sans_casse()
  type_maille=tolower(mot())
  a=mot()
  mailles=mot() 
}

($1=="{") && (mailles) || ($1==",") {
  bord[ibord]=mot_sans_casse()
  if (tolower(bord[ibord])=="raccord_local_homogene") {
     bord[ibord]=mot_sans_casse()
     type_bord[ibord]="raccord_local_homogene" }
  else
     type_bord[ibord]="bord"
  a=mot()
  a=mot()
  faces[ibord]=mot()
  ibord++ 
}

END {
#print type_maille,dimension
print FILENAME
print "Domain name ("sommets" nodes for "mailles" meshes):"
print "---------------"
print domaine
print "\nDiscretisation:"
print "---------------"
if (type_maille=="triangle" && dimension==2) print "VEF 2D"
else if (type_maille=="tetraedre" && dimension==3) print "VEF 3D"
else if (type_maille=="rectangle" && dimension==2) print "VDF 2D"
else if (type_maille=="hexaedre" && dimension==3) print "VDF 3D"
else print "Inconnue"
#print "Nom de la zone:"zone
print "\nName of "ibord" bords:"
print "-----------------"
for (i=0;i<ibord;i++)
   print bord[i]"\t ("faces[i]" faces de "type_bord[i]")"
print "\nDomain dimensions:"
print "-------------------------------"
print xmin" < x < "xmax
print ymin" < y < "ymax
if (dimension==3)
   print zmin" < z < "zmax
}

function mot_sans_casse()
{
   mot_ind++
   if (mot_ind>NF) {
      getline
      while (NF==0) getline
      mot_ind=1
   }
#   print NF,tolower($mot_ind)
   return $mot_ind
}

function mot() 
{
   return tolower(mot_sans_casse())
}
