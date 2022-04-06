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
//////////////////////////////////////////////////////////////////////////////
//
// File:        Zone_EF.h
// Directory:   $TRUST_ROOT/src/EF/Zones
// Version:     1
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Zone_EF_included
#define Zone_EF_included



#include <Zone_VF.h>
#include <Elem_EF.h>
class Geometrie;

//
// .DESCRIPTION class Zone_EF
//
// 	Classe instanciable qui derive de Zone_VF.
// 	Cette classe contient les informations geometriques que demande
// 	la methode des Volumes Elements Finis (element de Crouzeix-Raviart)
// 	La classe porte un certain nombre d'informations concernant les faces
// 	Dans cet ensemble de faces on fait figurer aussi les faces du bord et
//      des joints. Pour manipuler les faces on distingue 2 categories:
//           - les faces non standard qui sont sur un joint, un bord ou qui sont
//             internes tout en appartenant a un element du bord
//           - les faces standard qui sont les faces internes n'appartenant pas
//             a un element du bord
//      Cette distinction correspond au traitement des conditions aux limites:les
//      faces standard ne "voient pas" les conditions aux limites.
//      L'ensemble des faces est numerote comme suit:
//           - les faces qui sont sur une Zone_joint apparaissent en premier
//    	       (dans l'ordre du vecteur les_joints)
//   	     - les faces qui sont sur une Zone_bord apparaissent ensuite
//	       (dans l'ordre du vecteur les_bords)
//  	     - les faces internes non standard apparaissent ensuite
//           - les faces internes standard en dernier
//      Finalement on trouve regroupees en premier toutes les faces non standard
//      qui vont necessiter un traitement particulier
//      On distingue deux types d'elements
//           - les elements non standard : ils ont au moins une face de bord
//           - les elements standard : ils n'ont pas de face de bord
//      Les elements standard (resp. les elements non standard) ne sont pas ranges
//      de maniere consecutive dans l'objet Zone. On utilise le tableau
//      rang_elem_non_std pour acceder de maniere selective a l'un ou
//      l'autre des types d'elements
//
//


class Zone_EF : public Zone_VF
{

  Declare_instanciable(Zone_EF);

public :
  void typer_elem(Zone& zone_geom) override;
  void discretiser() override;
  void swap(int, int, int);
  void reordonner(Faces&) override;
  void modifier_pour_Cl(const Conds_lim& ) override;

  inline const Elem_EF& type_elem() const;
  inline int nb_elem_Cl() const;
  inline int nb_faces_joint() const;
  inline int nb_faces_std() const;
  inline int nb_elem_std() const;
  inline double carre_pas_du_maillage() const;
  inline double carre_pas_maille(int i) const
  {
    return h_carre_(i);
  };
  inline double face_normales(int ,int ) const override;
  inline DoubleTab& face_normales();
  inline const DoubleTab& face_normales() const;
  inline IntVect& rang_elem_non_std();
  inline const IntVect& rang_elem_non_std() const;
  inline int oriente_normale(int face_opp, int elem2)const;
  inline const ArrOfInt& ind_faces_virt_non_std() const;
  void calculer_volumes_entrelaces();
  void calculer_volumes_sommets();
  virtual void calculer_IPhi();
  virtual void calculer_Bij(DoubleTab& bij_) ;
  virtual void calculer_Bij_gen(DoubleTab& bij);

  //  inline const DoubleVect& volumes_sommets() const { return volumes_sommets_; }
  inline const DoubleVect& volumes_thilde() const
  {
    return volumes_thilde_;
  }
  inline const DoubleVect& volumes_sommets_thilde() const
  {
    return volumes_sommets_thilde_;
  }
  inline const DoubleVect& porosite_sommet() const
  {
    return porosite_sommets_;
  }
  inline DoubleVect& set_porosite_sommet()
  {
    return porosite_sommets_;
  }

  void calculer_h_carre();
  void calculer_porosites_sommets();
  inline const DoubleTab& Bij() const
  {
    return Bij_ ;
  };
  inline const DoubleTab& Bij_thilde() const
  {
    return Bij_thilde_ ;
  };
  inline const DoubleTab& IPhi() const
  {
    return IPhi_ ;
  };
  inline const DoubleTab& IPhi_thilde() const
  {
    return IPhi_thilde_ ;
  };

  virtual void verifie_compatibilite_domaine();

protected:
  DoubleTab IPhi_,IPhi_thilde_;
private:
  DoubleVect porosite_sommets_ ,volumes_sommets_thilde_,volumes_thilde_;
  //  Champ_Don champ_porosite_sommets_,champ_porosite_lu_;
  DoubleTab Bij_,Bij_thilde_;                         // stockage des matrice Bije

  double h_carre;			 // carre du pas du maillage
  DoubleVect h_carre_;			// carre du pas d'une maille
  Elem_EF type_elem_;                  // type de l'element de discretisation
  DoubleTab face_normales_;             // normales aux faces
  int nb_faces_std_;                    // nombre de faces standard
  int nb_elem_std_;                     // nombre d'elements standard
  IntVect rang_elem_non_std_;		 // rang_elem_non_std_= -1 si l'element est standard
  // rang_elem_non_std_= rang de l'element dans les tableaux
  // relatifs aux elements non standards

  ArrOfInt ind_faces_virt_non_std_;      // contient les indices des faces virtuelles non standard
  void remplir_elem_faces() override;
  Sortie& ecrit(Sortie& os) const;
  void creer_faces_virtuelles_non_std();
  IntVect orientation_;
};

// Fonctions inline

// Decription:
// renvoie le type d'element utilise.
inline const Elem_EF& Zone_EF::type_elem() const
{
  return type_elem_;
}

// Decription:
// renvoie la composante comp de la surface normale a la face.
inline double Zone_EF::face_normales(int face,int comp) const
{
  return face_normales_(face,comp);
}

// Decription:
// renvoie le tableau des surfaces normales.
inline DoubleTab& Zone_EF::face_normales()
{
  return face_normales_;
}

// Decription:
// renvoie le tableau des surfaces normales.
inline const DoubleTab& Zone_EF::face_normales() const
{
  return face_normales_;
}


// Decription:
inline IntVect& Zone_EF::rang_elem_non_std()
{
  return rang_elem_non_std_;
}

// Decription:
inline const IntVect& Zone_EF::rang_elem_non_std() const
{
  return rang_elem_non_std_;
}


// Decription:
inline int Zone_EF::nb_faces_joint() const
{
  return 0;
  //    return nb_faces_joint_;    A FAIRE
}

// Decription:
inline int Zone_EF::nb_faces_std() const
{
  return nb_faces_std_;
}

// Decription:
inline int  Zone_EF::nb_elem_std() const
{
  return nb_elem_std_;
}

// Decription:
inline int Zone_EF::nb_elem_Cl() const
{
  return nb_elem() - nb_elem_std_;
}


// Decription:
inline double Zone_EF::carre_pas_du_maillage() const
{
  return h_carre;
}

// Decription:
inline int Zone_EF::oriente_normale(int face_opp, int elem2) const
{
  if(face_voisins(face_opp,0)==elem2)
    return 1;
  else return -1;
}


// Decription:
// Renvoie le tableau des indices des faces virtuelles non standard
//inline const ArrsOfInt& Zone_EF::faces_virt_non_std() const
//{
//  return faces_virt_non_std_;
//}

// Decription:
// Renvoie le tableau des indices des faces distantes non standard
inline const ArrOfInt& Zone_EF::ind_faces_virt_non_std() const
{
  return ind_faces_virt_non_std_;
}
#endif
