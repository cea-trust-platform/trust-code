/****************************************************************************
* Copyright (c) 2017, CEA
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
// File:        Zone_VEF.h
// Directory:   $TRUST_ROOT/src/VEF/Zones
// Version:     /main/20
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Zone_VEF_included
#define Zone_VEF_included



#include <math.h>
#include <Zone_VF.h>
#include <Elem_VEF.h>

class Geometrie;

//
// .DESCRIPTION class Zone_VEF
//
//         Classe instanciable qui derive de Zone_VF.
//         Cette classe contient les informations geometriques que demande
//         la methode des Volumes Elements Finis (element de Crouzeix-Raviart)
//         La classe porte un certain nombre d'informations concernant les faces
//         Dans cet ensemble de faces on fait figurer aussi les faces du bord et
//      des joints. Pour manipuler les faces on distingue 2 categories:
//           - les faces non standard qui sont sur un joint, un bord ou qui sont
//             internes tout en appartenant a un element du bord
//           - les faces standard qui sont les faces internes n'appartenant pas
//             a un element du bord
//      Cette distinction correspond au traitement des conditions aux limites:les
//      faces standard ne "voient pas" les conditions aux limites.
//      L'ensemble des faces est numerote comme suit:
//           - les faces qui sont sur une Zone_joint apparaissent en premier
//                   (dans l'ordre du vecteur les_joints)
//                - les faces qui sont sur une Zone_bord apparaissent ensuite
//               (dans l'ordre du vecteur les_bords)
//               - les faces internes non standard apparaissent ensuite
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


class Zone_VEF : public Zone_VF
{

  Declare_instanciable(Zone_VEF);

public :

  void discretiser();
  void swap(int, int, int);
  virtual void reordonner(Faces&);
  void modifier_pour_Cl(const Conds_lim& );
  void typer_elem(Zone&);

  inline const Elem_VEF& type_elem() const;
  inline int nb_elem_Cl() const;
  inline int nb_faces_joint() const;
  inline int nb_faces_std() const;
  inline int nb_elem_std() const;
  inline int premiere_face_std() const;
  inline int nb_faces_non_std() const;
  inline double carre_pas_du_maillage() const;
  inline double carre_pas_maille(int i) const
  {
    return h_carre_(i);
  };
  inline double face_normales(int ,int ) const;
  void calculer_face_surfaces();
  inline DoubleTab& face_normales()
  {
    return face_normales_;
  };
  inline const DoubleTab& face_normales() const
  {
    return face_normales_;
  };
  inline DoubleTab& facette_normales();
  inline const DoubleTab& facette_normales() const;
  inline IntVect& rang_elem_non_std();
  inline const IntVect& rang_elem_non_std() const;
  inline int oriente_normale(int face_opp, int elem2)const;
  DoubleTab& vecteur_face_facette();
  inline const ArrOfInt& ind_faces_virt_non_std() const;
  void calculer_volumes_entrelaces();
  void calculer_h_carre();

private:

  double h_carre;                         // carre du pas du maillage
  DoubleVect h_carre_;                        // carre du pas d'une maille
  Elem_VEF type_elem_;                  // type de l'element de discretisation
  DoubleTab face_normales_;             // normales aux faces
  DoubleTab facette_normales_;          // normales aux faces des volumes entrelaces
  DoubleTab vecteur_face_facette_;                // vecteur centre face->centre facette
  int nb_faces_std_;                    // nombre de faces standard
  int nb_elem_std_;                     // nombre d'elements standard
  IntVect rang_elem_non_std_;                 // rang_elem_non_std_= -1 si l'element est standard
  // rang_elem_non_std_= rang de l'element dans les tableaux
  // relatifs aux elements non standards

  ArrOfInt ind_faces_virt_non_std_;      // contient les indices des faces virtuelles non standard
  void remplir_elem_faces();
  Sortie& ecrit(Sortie& os) const;
  void creer_faces_virtuelles_non_std();
  IntVect orientation_;
};

// Fonctions inline

// Decription:
// renvoie le type d'element utilise.
inline const Elem_VEF& Zone_VEF::type_elem() const
{
  return type_elem_;
}

// Decription:
// renvoie la composante comp de la surface normale a la face.
inline double Zone_VEF::face_normales(int face,int comp) const
{
  return face_normales_(face,comp);
}

// Decription:
// renvoie le tableau des surfaces normales des facettes.
inline DoubleTab& Zone_VEF::facette_normales()
{
  return facette_normales_;
}

// Decription:
// renvoie le tableau des surfaces normales des facettes.
inline const DoubleTab& Zone_VEF::facette_normales() const
{
  return facette_normales_;
}

// Decription:
inline IntVect& Zone_VEF::rang_elem_non_std()
{
  return rang_elem_non_std_;
}

// Decription:
inline const IntVect& Zone_VEF::rang_elem_non_std() const
{
  return rang_elem_non_std_;
}


// Decription:
inline int Zone_VEF::nb_faces_joint() const
{
  return 0;
  //    return nb_faces_joint_;    A FAIRE
}

// Decription:
inline int Zone_VEF::nb_faces_std() const
{
  return nb_faces_std_;
}

// Decription:
inline int  Zone_VEF::nb_elem_std() const
{
  return nb_elem_std_;
}

// Decription:
inline int Zone_VEF::nb_elem_Cl() const
{
  return nb_elem() - nb_elem_std_;
}

// Decription:
inline int Zone_VEF::premiere_face_std() const
{
  return nb_faces() - nb_faces_std_;
}

// Decription:
inline int Zone_VEF::nb_faces_non_std() const
{
  return nb_faces() - nb_faces_std_;
}

// Decription:
inline double Zone_VEF::carre_pas_du_maillage() const
{
  return h_carre;
}

// Decription:
inline int Zone_VEF::oriente_normale(int face_opp, int elem2) const
{
  if(face_voisins(face_opp,0)==elem2)
    return 1;
  else return -1;
}


// Decription:
// Renvoie le tableau des indices des faces virtuelles non standard
//inline const VECT(ArrOfInt)& Zone_VEF::faces_virt_non_std() const
//{
//  return faces_virt_non_std_;
//}

// Decription:
// Renvoie le tableau des indices des faces distantes non standard
inline const ArrOfInt& Zone_VEF::ind_faces_virt_non_std() const
{
  return ind_faces_virt_non_std_;
}
#endif
