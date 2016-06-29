/****************************************************************************
* Copyright (c) 2015 - 2016, CEA
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
// File:        Elem_geom.h
// Directory:   $TRUST_ROOT/src/Kernel/Geometrie
// Version:     /main/15
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Elem_geom_included
#define Elem_geom_included




#include <Elem_geom_base.h>


//////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION
//     Classe Elem_geom
//     Classe generique de la hierarchie des elements geometriques, un objet
//     Elem_geom peut referencer n'importe quel objet derivant de
//     Elem_geom_base.
//     La plupart des methodes appellent les methodes de l'objet Elem_geom_base
//     sous-jacent via la methode valeur() declaree grace a la macro
//Declare_deriv().;
// .SECTION voir aussi
//////////////////////////////////////////////////////////////////////////////
Declare_deriv(Elem_geom_base);
class Elem_geom : public DERIV(Elem_geom_base)
{

  Declare_instanciable(Elem_geom);

public:

  inline void associer_zone(const Zone& );
  inline void creer_faces_elem(Faces& ,int ) const;
  inline void creer_faces_elem(Faces& ,int ,Type_Face ) const;
  inline void face_sommet(int i, int j) const;
  inline int nb_som() const;
  inline int nb_faces(int=0) const;
  inline int nb_som_face(int=0) const;
  inline int est_structure() const;
  inline const Nom& nom_lml() const;
  inline int contient(const ArrOfDouble&, int ) const;
  inline int contient(const ArrOfInt&, int ) const;
  inline int nb_type_face() const;
  inline Type_Face type_face(int=0) const;
  inline void calculer_centres_gravite(DoubleTab& ) const;
  inline void reordonner() ;
  inline void calculer_volumes(DoubleVect& ) const;
  inline void calculer_normales(const IntTab& , DoubleTab& ) const;

};

// Description:
//    Appel a l'objet sous-jacent
//    Associe l'element geometrique a sa zone.
// Precondition:
// Parametre: Zone&  z
//    Signification: la zone dont fait partie l'element
//    Valeurs par defaut:
//    Contraintes: reference constante
//    Acces: entree
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
inline void Elem_geom::associer_zone(const Zone&  z)
{
  valeur().associer_zone(z);
}


// Description:
//    Appel a l'objet sous-jacent
//    Renvoie le j-ieme sommet de la i-ieme face de
//    l'element.
// Precondition:
// Parametre: int i
//    Signification: un numero de face
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Parametre: int j
//    Signification: un numero de sommet
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
inline void Elem_geom::face_sommet(int i, int j) const
{
  valeur().face_sommet(i, j);
}


// Description:
//    Appel a l'objet sous-jacent
//    Cree les faces de l'element specifie de la zone.
//    (sans specifier le type de face a creer)
// Precondition:
// Parametre: Faces& les_faces
//    Signification: les faces de l'element
//    Valeurs par defaut:
//    Contraintes:
//    Acces: sortie
// Parametre: int num_elem
//    Signification: le numero de l'element de la zone dont on
//                   doit creer les faces
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
inline void Elem_geom::creer_faces_elem(Faces& les_faces,int num_elem) const
{
  valeur().creer_faces_elem(les_faces,num_elem);
}


// Description:
//    Appel a l'objet sous-jacent
//    Cree les faces de l'element specifie de la zone
//    en specifiant le type de de face a creer.
// Precondition:
// Parametre: Faces& les_faces
//    Signification: les faces de l'element
//    Valeurs par defaut:
//    Contraintes:
//    Acces: sortie
// Parametre: int num_elem
//    Signification: le numero de l'element de la zone dont on
//                   doit creer les faces
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Parametre: Type_Face typ
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
inline void Elem_geom::creer_faces_elem(Faces& les_faces,int num_elem,Type_Face typ) const
{
  valeur().creer_faces_elem(les_faces,num_elem,typ);
}


// Description:
//    Appel a l'objet sous-jacent
//    Calcule les centres de gravites de tous les elements
//    de la zone geometrique associee.
// Precondition:
// Parametre: DoubleTab& xp
//    Signification: le tableau contenant les coordonnees des
//                   des centres de gravites.
//    Valeurs par defaut:
//    Contraintes:
//    Acces: sortie
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
inline void Elem_geom::calculer_centres_gravite(DoubleTab& xp) const
{
  valeur().calculer_centres_gravite(xp);
}


// Description:
//    Appel a l'objet sous-jacent
//    Calcule les volumes des elements de la zone associee.
// Precondition:
// Parametre: DoubleVect& volumes
//    Signification: le vecteur contenant les valeurs  des
//                   des volumes des elements de la zone
//    Valeurs par defaut:
//    Contraintes:
//    Acces: sortie
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
inline void Elem_geom::calculer_volumes(DoubleVect& volumes) const
{
  valeur().calculer_volumes(volumes);
}

// Description:
//    Appel a l'objet sous-jacent
//    Calcule les normales aux faces des elements de la zone associee.
// Precondition:
// Parametre: DoubleTab& face_sommets
//    Signification: les numeros des sommets des faces
//                   dans la liste des sommets de la zone
// Parametre: DoubleTab& face_normales
//    Signification: le vecteur contenant les normales
//                   aux faces des elements de la zone
//    Valeurs par defaut:
//    Contraintes:
//    Acces: sortie
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
inline void Elem_geom::calculer_normales(const IntTab& face_sommets, DoubleTab& face_normales) const
{
  valeur().calculer_normales(face_sommets,face_normales);
}

// Description:
//    Appel a l'objet sous-jacent
//    Renvoie le nombre de faces du type specifie
//    que possede l'element geometrique.
// Precondition:
// Parametre: int i
//    Signification: le type de face
//    Valeurs par defaut: 0
//    Contraintes: 0 <= i < nombre de type de face
//    Acces:
// Retour: int
//    Signification: le nombre de face du type specifie
//                   dans l'element geometrique courant
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
inline int Elem_geom::nb_faces(int i) const
{
  assert(i<nb_type_face());
  return valeur().nb_faces(i);
}


// Description:
//    Appel a l'objet sous-jacent
//    Renvoie le nombre de sommets de l'element
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: int
//    Signification: le nombre de sommets de l'element
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
inline int Elem_geom::nb_som() const
{
  return valeur().nb_som();
}

// Description:
//    Appel a l'objet sous-jacent
//    Renvoie le nombre de sommets des faces
//    du type specifie.
// Precondition:
// Parametre: int i
//    Signification: le type de face
//    Valeurs par defaut: 0
//    Contraintes: 0 <= i < nombre de type de face
//    Acces:
// Retour: int
//    Signification: le nombre de sommets des faces du type specifie
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
inline int Elem_geom::nb_som_face(int i) const
{
  assert(i<nb_type_face());
  return valeur().nb_som_face(i);
}


// Description:
//    Appel a l'objet sous-jacent
//    Renvoie 1 si l'element geometrique est structure,
//            0 sinon
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: int
//    Signification: renvoie 1 si l'element geometrique
//                   est structure, 0 sinon
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
inline int Elem_geom::est_structure() const
{
  return valeur().est_structure();
}


// Description
//    Appel a l'objet sous-jacent
//    Renvoie le nom de l'element geometrique
//    au format LML.
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: Nom&
//    Signification: le nom de l'element geometrique
//                   au format LML
//    Contraintes: reference constante
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
inline const Nom& Elem_geom::nom_lml() const
{
  return valeur().nom_lml();
}


// Description:
//    Appel a l'objet sous-jacent
//    Renvoie 1 si l'element i de la zone associee a
//              l'element geometrique contient le point
//              de coordonnees specifiees par le parametre "pos".
//    Renvoie 0 sinon.
// Precondition:
// Parametre: DoubleVect& pos
//    Signification: coordonnees du point que l'on
//                   cherche a localiser
//    Valeurs par defaut:
//    Contraintes: reference constante
//    Acces: entree
// Parametre: int i
//    Signification: le numero de l'element de la zone
//                   dans lequel on cherche le point.
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: int
//    Signification: 1 si le point de coordonnees specifiees
//                   appartient a l'element i
//                   0 sinon
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
inline int Elem_geom::contient(const ArrOfDouble& pos, int i ) const
{
  return valeur().contient(pos, i);
}


// Description:
//    Appel a l'objet sous-jacent
//    Renvoie 1 si les sommets specifies par le parametre "pos"
//    sont les sommets de l'element i de la zone associee a
//    l'element geometrique.
// Precondition:
// Parametre: IntVect& pos
//    Signification: les numeros des sommets a comparer
//                   avec ceux de l'elements i
//    Valeurs par defaut:
//    Contraintes: reference constante
//    Acces: entree
// Parametre: int i
//    Signification: le numero de l'element de la zone
//                   dont on veut comparer les sommets
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: int
//    Signification: 1 si les sommets passes en parametre
//                   sont ceux de l'element i, 0 sinon
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
inline int Elem_geom::contient(const ArrOfInt& pos, int i ) const
{
  return valeur().contient(pos, i);
}


// Description:
//    Appel a l'objet sous-jacent
//    Renvoie le i-eme type de face de l'element
//    geometrique.
// Precondition:
// Parametre: int i
//    Signification: le rang du type de face a renvoyer
//    Valeurs par defaut: 0
//    Contraintes: i < nombre de type de face de l'element geometrique
//    Acces:
// Retour: Type_Face
//    Signification: un objet Type_Face correspondant au type
//                   demande
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
inline Type_Face Elem_geom::type_face(int i) const
{
  assert(i<nb_type_face());
  return valeur().type_face(i);
}

// Description:
//    Appel a l'objet sous-jacent
//    Reordonne les sommets du maillage.
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
inline void Elem_geom::reordonner()
{
  valeur().reordonner();
}


// Description:
//    Appel a l'objet sous-jacent
//    Renvoie le nombre de type de face
//    de l'element geometrique.
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
// Postcondition: la methode ne modifie pas l'objet
inline int Elem_geom::nb_type_face() const
{
  return valeur().nb_type_face();
}

#endif
