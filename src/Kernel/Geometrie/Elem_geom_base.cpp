/****************************************************************************
* Copyright (c) 2015, CEA
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:
* 1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
* 2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
* 3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
* IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
* OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
*****************************************************************************/
//////////////////////////////////////////////////////////////////////////////
//
// File:        Elem_geom_base.cpp
// Directory:   $TRUST_ROOT/src/Kernel/Geometrie
// Version:     /main/18
//
//////////////////////////////////////////////////////////////////////////////

#include <Elem_geom_base.h>
#include <Domaine.h>

Implemente_base(Elem_geom_base,"Elem_geom_base",Objet_U);


// Description:
//    NE FAIT RIEN
// Precondition:
// Parametre: Sortie& s
//    Signification: un flot de sortie
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: Sortie&
//    Signification: le flot de sortie
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
Sortie& Elem_geom_base::printOn(Sortie& s ) const
{
  return s;
}



// Description:
//    NE FAIT RIEN
// Precondition:
// Parametre: Entree& s
//    Signification: un flot d'entree
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: Entree&
//    Signification: le flot d'entree
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
Entree& Elem_geom_base::readOn(Entree& s )
{
  return s;
}


// Description:
//    NE FAIT RIEN:
//    A surcharger dans les classes derivees.
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
void Elem_geom_base::reordonner()
{
}


// Description:
//    Renvoie le parametre face si le type
//    de face specifie est le meme que celui de l'element
//    geometrique.
// Precondition:
// Parametre: int face
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Parametre: Type_Face& type
//    Signification: un type de face
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour:
//    Signification:
//    Contraintes:
// Exception: mauvais type de face specifie
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
int Elem_geom_base::num_face(int face, Type_Face& type) const
{
  assert(type==type_face());
  return face;
}

// Description:
//    Cree les faces de l'element geometrique specifie de la zone
//    en precisant le type de face a creer.
// Precondition:
// Parametre: Faces& les_faces
//    Signification: les faces a creer
//    Valeurs par defaut:
//    Contraintes:
//    Acces: sortie
// Parametre: int num_elem
//    Signification: le numero de l'element dont on veut creer
//                   les faces
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Parametre: Type_Face type
//    Signification: le type de face a creer
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
void Elem_geom_base::creer_faces_elem(Faces& les_faces ,
                                      int num_elem,
                                      Type_Face type) const
{
  int type_id=0;
  for(; ((type_id<nb_type_face())
         &&(type!=type_face(type_id))); type_id++)
    ;
  const IntTab& les_Polys = ma_zone->les_elems();
  assert(les_Polys.dimension_tot(0) > num_elem);
  int face, i;
  les_faces.dimensionner(nb_faces(type_id));
  les_faces.associer_zone(ma_zone.valeur());

  for(face=0; face<nb_faces(type_id); face++)
    {
      int face_id=num_face(face, type);
      for (i=0; i<nb_som_face(type_id); i++)
        les_faces.sommet(face_id,i) = les_Polys(num_elem,face_sommet(face_id, i));
      les_faces.completer(face_id, num_elem);
    }
}


// Description:
//    Calcule les centres de gravites de tous les elements
//    de la zone associee a l'element goemetrique.
// Precondition:
// Parametre: DoubleTab& xp
//    Signification: le tableau contenant les coordonnees des
//                   centres de gravite
//    Valeurs par defaut:
//    Contraintes:
//    Acces: sortie
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
void Elem_geom_base::calculer_centres_gravite(DoubleTab& xp) const
{
  const IntTab& les_Polys = ma_zone->les_elems();
  const Domaine& le_domaine = ma_zone->domaine();
  int nb_elem;
  if(xp.dimension(0)==0)
    {
      nb_elem = ma_zone->nb_elem_tot();
      xp.resize(nb_elem,dimension);
    }
  else
    nb_elem=xp.dimension(0);

  int num_som;

  xp=0;
  for (int num_elem=0; num_elem<nb_elem; num_elem++)
    {
      int nb_som_reel=nb_som();
      while (les_Polys(num_elem,nb_som_reel-1)==-1)  nb_som_reel--;
      for(int s=0; s<nb_som_reel; s++)
        {
          num_som = les_Polys(num_elem,s);
          for(int i=0; i<dimension; i++)
            xp(num_elem,i) += le_domaine.coord(num_som,i)/nb_som_reel;
        }
    }
}

// Description:
//    Sort en erreur. Cette methode n'est pas virtuelle pure
//    pour des raisons de commodite
// Precondition:
// Parametre: DoubleTab& xp
//    Signification: le tableau contenant les coordonnees des
//                   centres de gravite
//    Valeurs par defaut:
//    Contraintes:
//    Acces: sortie
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
void Elem_geom_base::calculer_normales(const IntTab& faces_sommets , DoubleTab& face_normales) const
{
  Cerr << "calculer_normales method is not coded for an element " << finl;
  Cerr << "of type que_suis_je() " << finl;
  exit();
}

// Description:
//    Renvoie le nombre de type de face de l'element
//    geometrique.
//    Par exemple un prisme (Classe Prisme)
//    a 2 types de faces: un triangle ou un quadrangle.
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: int
//    Signification: renvoie toujours 1
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
int Elem_geom_base::nb_type_face() const
{
  return 1;
}

// Description: remplit le tableau faces_som_local(i,j) qui donne pour
//  0 <= i < nb_faces()  et  0 <= j < nb_som_face(i) le numero local du sommet
//  sur l'element.
//  On a  0 <= faces_sommets_locaux(i,j) < nb_som()
// Si toutes les faces de l'element n'ont pas le meme nombre de sommets, le nombre
// de colonnes du tableau est le plus grand nombre de sommets, et les cases inutilisees
// du tableau sont mises a -1
// On renvoie 1 si toutes les faces ont le meme nombre d'elements, 0 sinon.
int Elem_geom_base::get_tab_faces_sommets_locaux(IntTab& faces_som_local) const
{
  Cerr << "Elem_geom_base::faces_sommets_locaux : error.\n"
       << " Method not implemented for the object "
       << que_suis_je() << finl;
  exit();
  return 0;
}

// Description: idem que Elem_geom_base::get_tab_faces_sommets_locaux mais pour les aretes:
//  aretes_som_local.dimension(0) = nombre d'aretes sur l'element de reference
//  aretes_som_local.dimension(1) = 2  (nombre de sommets par arete)
//  aretes_som_local(i,j) = numero d'un sommet de l'element (0 <= n < nb_sommets_par_element)
void Elem_geom_base::get_tab_aretes_sommets_locaux(IntTab& aretes_som_local) const
{
  Cerr << "Elem_geom_base::aretes_sommets_locaux : error.\n"
       << " Method not implemented for the object "
       << que_suis_je() << finl;
  exit();
}
