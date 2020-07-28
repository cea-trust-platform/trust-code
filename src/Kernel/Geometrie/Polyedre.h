/****************************************************************************
* Copyright (c) 2020, CEA
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
// File:        Polyedre.h
// Directory:   $TRUST_ROOT/src/Kernel/Geometrie
// Version:     /main/13
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Polyedre_included
#define Polyedre_included

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
//    Classe Polyedre
//    Cette represente l'element geometrique Polyedre.
//    Un polyedre est un element defini par ses faces
//    de type Faces::polygone_3D
// .SECTION voir aussi
//    Poly_geom_base Elem_geom
//////////////////////////////////////////////////////////////////////////////
class Polyedre  : public Poly_geom_base
{

  Declare_instanciable(Polyedre);

public :


  void calculer_un_centre_gravite(const int elem,DoubleVect& xp) const;
  void calculer_centres_gravite(DoubleTab& xp) const;
  // Fonctions d'acces aux membres prives:
  inline int face_sommet(int i, int j) const;
  inline int nb_som() const;
  inline int nb_faces(int=0) const;
  inline int nb_som_face(int=0) const;
  inline int est_structure() const;
  virtual const Nom& nom_lml() const;
  virtual int contient(const ArrOfDouble&, int ) const;
  virtual int contient(const ArrOfInt&, int ) const;
  inline Type_Face type_face(int=0) const;
  virtual void reordonner() ;
  virtual void calculer_volumes(DoubleVect& ) const;
  virtual int nb_type_face() const;
  int get_tab_faces_sommets_locaux(IntTab& faces_som_local) const;
  int get_tab_faces_sommets_locaux(IntTab& faces_som_local,int elem) const;
  void affecte_connectivite_numero_global(const ArrOfInt& Nodes,const ArrOfInt& FacesIndex,const ArrOfInt& PolyhedronIndex,IntTab& les_elems);
  int get_nb_som_elem_max() const;
  inline int get_nb_face_elem_max() const
  {
    return nb_face_elem_max_ ;
  } ;
  inline int get_nb_som_face_max() const
  {
    return nb_som_face_max_ ;
  } ;
  int get_somme_nb_faces_elem() const;

  inline const ArrOfInt& getFacesIndex() const
  {
    return FacesIndex_;
  };
  inline const ArrOfInt& getPolyhedronIndex() const
  {
    return PolyhedronIndex_;
  };
  inline const ArrOfInt& getElemIndex() const
  {
    return PolyhedronIndex_;
  };
  void remplir_Nodes_glob(ArrOfInt& Nodes_glob,const IntTab& les_elems ) const;
  // void affecte_connectivite_numero_local(Nodes,FacesIndex,PolyhedronIndex);
  void ajouter_elements(const Elem_geom_base& new_elem, const IntTab& new_elems, IntTab& les_elems);
  void build_reduced(Elem_geom& type_elem, const ArrOfInt& elems_sous_part) const;
  void compute_virtual_index();

private :
  ArrOfInt Nodes_,FacesIndex_,PolyhedronIndex_;     //         PolyhedronIndex_[ele]  numero premiere face de l'ele
  // FacesIndex_[f] numero premier indice de sommet de la face f dans Nodes
  // Nodes_[s] numero local du sommet s d'une face, elem(ele,s) -> numero global
  int nb_som_elem_max_,nb_face_elem_max_,nb_som_face_max_;
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
inline int Polyedre::face_sommet(int face, int sommet) const
{
  BLOQUE;
  return -1;
}


// Description:
//    Renvoie le nombre de sommets d'un Polyedre
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
inline int Polyedre::nb_som() const
{
  return get_nb_som_elem_max();

}


// Description:
//    Renvoie le nombre de faces du type specifie
//    que possede l'element geometrique.
//    Un Polyedre a 1 type de faces: polygon_3D
// Precondition:
// Parametre: int i
//    Signification: le type de face
//    Valeurs par defaut: 0
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
inline int Polyedre::nb_faces(int i) const
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
inline int Polyedre::nb_som_face(int i) const
{
  assert(i==0);
  return get_nb_som_face_max();
}


//    Renvoie toujours 0, car l'element geometrique
//    Polyedre n'est pas de type structure.
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
inline int Polyedre::est_structure() const
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
inline Type_Face Polyedre::type_face(int i) const
{
  assert(i<=0);
  switch(i)
    {
    case 0:
      return Faces::polygone_3D;
    default :
      Cerr << "Error, a polyhedron has 1 type of faces and not " << i << finl;
      exit();
      return Faces::quadrangle_3D;
    }
}

#endif
