/****************************************************************************
* Copyright (c) 2022, CEA
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

#ifndef Polygone_included
#define Polygone_included

#ifdef linux
#define BLOQUE Cerr<<__PRETTY_FUNCTION__<< " "<<__FILE__<<":"<<(int)__LINE__<<" not coded" <<finl;exit()
#define WARN Cerr<<"Warning: "<<__PRETTY_FUNCTION__<< " "<<__FILE__<<":"<<(int)__LINE__<<" bad coded" <<finl
#else
#define BLOQUE Cerr<<__FILE__<<":"<<(int)__LINE__<<" not coded" <<finl;exit()
#define WARN Cerr<<"Warning: "<<__FILE__<<":"<<(int)__LINE__<<" bad coded" <<finl
#endif

#include <Poly_geom_base.h>


//////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION
//    Classe Polygone
//    Cette represente l'element geometrique Polygone.
//    Un polyedre est un element defini par ses faces
//    de type Faces::polygone_3D
// .SECTION voir aussi
//    Poly_geom_base Elem_geom
//////////////////////////////////////////////////////////////////////////////
class Polygone  : public Poly_geom_base
{

  Declare_instanciable(Polygone);

public :


  // Fonctions d'acces aux membres prives:
  inline int face_sommet(int i, int j) const override;
  inline int nb_som() const override;
  inline int nb_faces(int=0) const override;
  inline int nb_som_face(int=0) const override;
  inline int est_structure() const override;
  const Nom& nom_lml() const override;
  int contient(const ArrOfDouble&, int ) const override;
  int contient(const ArrOfInt&, int ) const override;
  inline Type_Face type_face(int=0) const override;
  void reordonner() override ;
  void calculer_volumes(DoubleVect& ) const override;
  void calculer_centres_gravite(DoubleTab& xp) const override;
  void calculer_un_centre_gravite(const int elem,DoubleVect& xp) const override ;

  int nb_type_face() const override;
  int get_tab_faces_sommets_locaux(IntTab& faces_som_local) const override;
  int get_tab_faces_sommets_locaux(IntTab& faces_som_local,int elem) const override;
  void affecte_connectivite_numero_global(const ArrOfInt& FacesIndex,const ArrOfInt& PolygonIndex,IntTab& les_elems);
  int get_nb_som_elem_max() const;
  inline int get_nb_face_elem_max() const
  {
    return nb_face_elem_max_ ;
  } ;

  int get_somme_nb_faces_elem() const override;

  inline const ArrOfInt& getFacesIndex() const override
  {
    return FacesIndex_;
  };
  inline const ArrOfInt& getPolygonIndex() const
  {
    return PolygonIndex_;
  };
  inline const ArrOfInt& getElemIndex() const override
  {
    return PolygonIndex_;
  };
  void rebuild_index();
  void build_reduced(Elem_geom& type_elem, const ArrOfInt& elems_sous_part) const override;
  void compute_virtual_index() override;
private :
  ArrOfInt FacesIndex_,PolygonIndex_;  //  PolygoneIndex_[ele] numero premiere face de ele (sert sutrout en diff)
  // FaceIndex_ numerotation globale a la zone ne sert qu'a med
  int nb_som_elem_max_,nb_face_elem_max_;
};


// Description:
//    Renvoie le numero du j-ieme sommet de la i-ieme face de
//    l'element.
// Precondition:
// Parametre: int i
//    Signification: un numero de face
//    Valeurs par defaut:
//    Acces:
// Parametre: int j
//    Signification: un numero de sommet
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: int
//    Signification: le numero du j-ieme sommet de la i-ieme face
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
inline int Polygone::face_sommet(int face, int sommet) const
{
  BLOQUE;
  return -1;
}


// Description:
//    Renvoie le nombre de sommets d'un Polygone
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: int
//    Signification: le nombre de sommets maximum d'un polyedre
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
inline int Polygone::nb_som() const
{
  return get_nb_som_elem_max();

}


// Description:
//    Renvoie le nombre de faces du type specifie
//    que possede l'element geometrique.
//    Un Polygone a 1 type de faces: polygon_3D
// Precondition:
// Parametre: int i
//    Signification: le type de face
//    Valeurs par defaut: 0
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
inline int Polygone::nb_faces(int i) const
{
  assert(i==0);
  switch(i)
    {
    case 0:
      return get_nb_face_elem_max();
    default :
      Cerr << "Error, a polyhedron has 1 type of faces and not " << i << finl;
      exit();
      return -1;
    }
}


// Description:
//    Renvoie le nombre maximum de sommets des faces
//    du type specifie.
// Precondition:
// Parametre: int i
//    Signification: le type de face
//    Valeurs par defaut: 0
//    Acces:
// Retour: int
//    Signification: le nombre de sommets des faces de type i
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
inline int Polygone::nb_som_face(int i) const
{
  assert(i==0);
  return 2;
}


//    Renvoie toujours 0, car l'element geometrique
//    Polygone n'est pas de type structure.
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: int
//    Signification: 1 si l'element est de type structure
//                   0 sinon
//    Contraintes: toujours egal a 0
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
inline int Polygone::est_structure() const
{
  return 0;
}


// Description:
//    Renvoie le i-ieme type de face.
//    Un polyedre a 2 types de face: quadrangle et triangle
// Precondition:
// Parametre: int i
//    Signification: le rang du type de face a renvoyer
//    Valeurs par defaut: 0
//    Acces:
// Retour: Type_Face
//    Signification: le type de la face i
//    Contraintes: i=0 (Faces::polygone_3D)
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
inline Type_Face Polygone::type_face(int i) const
{
  assert(i<=0);
  switch(i)
    {
    case 0:
      return Faces::segment_2D;
    default :
      Cerr << "Error, a polyhedron has 1 type of faces and not " << i << finl;
      exit();
      return Faces::quadrangle_3D;
    }
}

#endif
