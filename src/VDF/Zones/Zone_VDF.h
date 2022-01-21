/****************************************************************************
* Copyright (c) 2021, CEA
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
// File:        Zone_VDF.h
// Directory:   $TRUST_ROOT/src/VDF/Zones
// Version:     /main/17
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Zone_VDF_included
#define Zone_VDF_included


#include <math.h>
#include <map>
#include <array>
#include <Zone_VF.h>
#include <Domaine.h>
class Geometrie;

//////////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION class Zone_VDF
//
//         Classe instanciable qui derive de Zone_VF.
//         Cette classe contient les informations geometriques que demande
//         la methode des Volumes Differences Finis.
//         La classe porte un certain nombre d'informations concernant les faces
//      L'ensemble des faces est numerote comme suit:
//           - les faces qui sont sur une Zone_joint apparaissent en premier
//                   (dans l'ordre du vecteur les_joints)
//                - les faces qui sont sur une Zone_bord apparaissent ensuite
//               (dans l'ordre du vecteur les_bords)
//               - les faces internes apparaissent ensuite
//      A chaque face on fait correspondre un int qui indique son orientation.
//      On suppose qu'a l'interieur de chaque famille de faces (bord,joint,interne)
//      on trouve:
//           - le bloc des faces d'equation x = cte (faces d'orientation 0)
//           - le bloc des faces d'equation y = cte (faces d'orientation 1)
//           - le bloc des faces d'equation z = cte (faces d'orientation 2)
//      Pour le bloc des faces de bord on conserve les sous-blocs constitues par
//      les faces d'un meme bord.
//      On n'a pas besoin d'une numerotation particuliere des elements.
//      On a introduit la notion d'arete pour le calcul des flux diffusifs et
//      convectifs dans la conservation de la quantite de mouvement.
//      Le tableau Qdm contient la connectivite arete/faces. Dans le tableau
//      Qdm les aretes apparaissent dans l'ordre suivant:
//           - bloc des aretes joints
//           - bloc des aretes bord
//           - bloc des aretes mixtes
//           - bloc des aretes internes
//      A l'interieur de chaque bloc les aretes apparaissent dans l'ordre
//      suivant: aretes XY, aretes XZ et aretes YZ.
//
/////////////////////////////////////////////////////////////////////////////////

class Zone_VDF : public Zone_VF
{

  Declare_instanciable(Zone_VDF);

public :

  void discretiser();
  Faces* creer_faces();
  virtual void reordonner(Faces& );
  inline int nb_faces_X() const;
  inline int nb_faces_Y() const;
  inline int nb_faces_Z() const;
  inline int nb_aretes() const;
  inline int nb_aretes_joint() const;
  inline int nb_aretes_coin() const;
  inline int premiere_arete_coin() const;
  inline int nb_aretes_bord() const;
  inline int premiere_arete_bord() const;
  inline int nb_aretes_mixtes() const;
  inline int premiere_arete_mixte() const;
  inline int nb_aretes_internes() const;
  inline int premiere_arete_interne() const;
  inline double h_x() const;
  inline double h_y() const;
  inline double h_z() const;

  inline int Qdm(int num_arete,int ) const;
  //inline double porosite_face(int ) const;
  //inline double porosite_elem(int i) const;
  inline int orientation(int ) const;
  inline double face_normales(int , int ) const;
  inline double dist_face(int , int , int k) const;
  inline double dist_norm(int num_face) const ;
  inline double dist_norm_bord(int num_face) const;
  inline double dist_face_elem0(int ,int ) const;
  inline double dist_face_elem1(int ,int ) const;
  inline double dist_face_axi(int , int , int k) const;
  inline double dist_face_period(int , int , int ) const;
  inline double dist_norm_period(int ,double ) const;
  inline double dist_face_elem0_period(int ,int ,double ) const;
  inline double dist_face_elem1_period(int ,int ,double ) const;
  inline double dist_norm_axi(int num_face) const ;
  inline double dist_norm_bord_axi(int num_face) const;
  inline double dist_face_elem0_axi(int ,int ) const;
  inline double dist_face_elem1_axi(int ,int ) const;
  inline double distance_face(int , int , int k) const;
  inline double distance_normale(int num_face) const;
  inline double dist_elem(int ,int ,int ) const;
  inline double dist_elem_period(int ,int ,int ) const;
  inline double dim_elem(int ,int ) const;
  inline double dim_face(int ,int ) const;
  inline double delta_C(int ) const;
  inline int amont_amont(int, int ) const;
  inline int face_amont_princ(int ,int ) const;
  inline int face_amont_conj(int ,int ,int ) const;
  inline int face_bord_amont(int ,int ,int ) const;
  inline int elem_voisin(int , int , int ) const;

  inline IntVect& orientation();
  inline const IntVect& orientation() const;

  // inline DoubleVect& porosite_face();
  // inline const DoubleVect& porosite_face() const;
  // inline DoubleVect& porosite_elem();
  // inline const DoubleVect& porosite_elem() const;

  inline IntTab& Qdm();
  inline const IntTab& Qdm() const;
  void calculer_volumes_entrelaces();
  void modifier_pour_Cl(const Conds_lim& cl);
  void creer_elements_fictifs(const Zone_Cl_dis_base& );
  DoubleVect& dist_norm_bord(DoubleVect& , const Nom& nom_bord) const;

  //std::map permettant de retrouver le couple (proc, item local) associe a un item virtuel pour le mdv_elem
  void init_virt_e_map() const;
  mutable std::map<std::array<int, 2>, int> virt_e_map;

private:

  IntVect orientation_;                    // orientation des faces
  // 0 si face perpendiculaire a l'axe des X
  // 1 si face perpendiculaire a l'axe des Y
  // 2 si face perpendiculaire a l'axe des Z
  int nb_faces_X_;                         // nombre de faces perpendiculaires a l'axe des X
  int nb_faces_Y_;                         // nombre de faces perpendiculaires a l'axe des Y
  int nb_faces_Z_;                         // nombre de faces perpendiculaires a l'axe des Z
  int nb_aretes_;                          // nombre d'aretes tous types confondus
  int nb_aretes_joint_;                    // nombre d'aretes joint
  int nb_aretes_coin_;                     // nombre d'aretes coin
  int nb_aretes_bord_;                     // nombre d'aretes bord
  int nb_aretes_mixtes_;                   // nombre d'aretes mixtes
  int nb_aretes_internes_;                 // nombre d'aretes internes
  IntTab Qdm_;                            // connectivites aretes/faces
  // DoubleVect porosite_elem_;               // Porosites volumiques pour les volumes de
  // controle de masse
  // DoubleVect porosite_face_;               // Porosites surfaciques en masse et volumiques
  // en quantite de mouvement
  double h_x_,h_y_,h_z_;                   // pas du maillage dans les trois directions
  // d'espace;h_x_ (resp h_y_) est la plus petite
  // distance entre deux centres de faces d'equation
  // X = cte (resp Y= cte)

  void remplir_elem_faces();
  // void calculer_porosites();
  void genere_aretes();
  void calcul_h();
};

// Fonctions inline

// Description:
inline IntTab& Zone_VDF::Qdm()
{
  return Qdm_;
}

// Description:
inline const IntTab& Zone_VDF::Qdm() const
{
  return Qdm_;
}

// Description:
inline int Zone_VDF::nb_faces_X() const
{
  return nb_faces_X_;
}

// Description:
inline int Zone_VDF::nb_faces_Y() const
{
  return nb_faces_Y_;
}

// Description:
inline int Zone_VDF::nb_faces_Z() const
{
  return nb_faces_Z_;
}

// Description:
inline int Zone_VDF::Qdm(int num_arete,int i) const
{
  return Qdm_(num_arete,i);
}

// Description:
// inline double Zone_VDF::porosite_face(int i) const
// {
//  return porosite_face_[i];
// }

// Description:
inline IntVect& Zone_VDF::orientation()
{
  return orientation_;
}

// Description:
inline const IntVect& Zone_VDF::orientation() const
{
  return orientation_;
}

// Description:
// inline DoubleVect& Zone_VDF::porosite_face()
// {
//  return porosite_face_;
// }

// Description:
// inline const DoubleVect& Zone_VDF::porosite_face() const
// {
//   return porosite_face_;
// }

// Description:
// inline DoubleVect& Zone_VDF::porosite_elem()
// {
//  return porosite_elem_;
// }

// Description:
// inline const DoubleVect& Zone_VDF::porosite_elem() const
// {
//  return porosite_elem_;
// }

// Description:
// inline double Zone_VDF::porosite_elem(int i) const
// {
//  return porosite_elem_[i];
// }

// Description:
inline int Zone_VDF::orientation(int i) const
{
  return orientation_[i];
}

// Description:
inline double Zone_VDF::face_normales(int num_face,int k) const
{
  int ori = orientation(num_face);
  double surf=0;
  if (ori == k)
    surf=face_surfaces(num_face);
  return surf;
}

// Fonction de calcul utilisable uniquement en coordonnees cartesiennes
// de la distance entre les centres de 2 faces dans la direction k

// Description:
inline double Zone_VDF::dist_face(int fac1, int fac2, int k) const
{
  // Attention cette methode n'est plus appelee par les methodes dist_face de
  // Eval_Diff_VDF_Multi_inco_const.cpp et Eval_Diff_VDF_const.cpp pour optimiser les evaluateurs
  return xv_(fac2,k) - xv_(fac1,k);
}

// Fonction de calcul utilisable uniquement en coordonnees cylindriques
// de la distance entre les centres de 2 faces dans la direction k

// Description:
inline double Zone_VDF::dist_face_axi(int fac1, int fac2, int k) const
{
  if (k != 1)
    return xv_(fac2,k) - xv_(fac1,k);
  else
    {
      double d_teta = xv_(fac2,1) - xv_(fac1,1);
      if (d_teta < 0)
        d_teta += 2.0*M_PI;
      return d_teta*xv_(fac1,0);
    }
}

// Fonction de calcul utilisable uniquement en coordonnees cartesiennes
// de la distance normale pour une face interne
// Pour une face interne distance normale = distance entre les centres des
// deux mailles voisines.

// Description:
inline double Zone_VDF::dist_norm(int num_face) const
{
  int n1 = face_voisins_(num_face,0);
  int n2 = face_voisins_(num_face,1);
  int k = orientation_[num_face];
  return (xp_(n2,k) - xp_(n1,k));
}

// Fonction de calcul utilisable uniquement en coordonnees cylindriques
// de la distance normale pour une face interne

// Description:
inline double Zone_VDF::dist_norm_axi(int num_face) const
{
  int n1 = face_voisins_(num_face,0);
  int n2 = face_voisins_(num_face,1);
  int k = orientation_[num_face];
  double dist;
  if (k != 1)
    dist = xp_(n2,k) - xp_(n1,k);
  else
    {
      double d_teta = xp_(n2,1) - xp_(n1,1);
      if (d_teta < 0)
        d_teta += 2.0*M_PI;
      dist = d_teta*xp_(n1,0);
    }
  return dist;
}

// Fonction de calcul utilisable uniquement en coordonnees cartesiennes
// de la distance normale pour une face de bord
// Pour une face de bord distance normale est egale a la distance entre le
// centre de la maille voisine et le bord.

// Description:
inline double Zone_VDF::dist_norm_bord(int num_face) const
{
  int n1 = face_voisins_(num_face,0);
  int n2 = face_voisins_(num_face,1);
  assert(num_face<nb_faces_bord() || n1==-1 || n2==-1); // Verifie que num_face est bien une face de bord reelle ou virtuelle
  int k = orientation_[num_face];
  if (n1!=-1)
    return (xv_(num_face,k) - xp_(n1,k));
  else
    return (xp_(n2,k) - xv_(num_face,k));
}

// Fonction de calcul utilisable uniquement en coordonnees cylindriques
// de la distance normale pour une face de bord

// Description:
inline double Zone_VDF::dist_norm_bord_axi(int num_face) const
{
  int n1 = face_voisins_(num_face,0);
  int n2 = face_voisins_(num_face,1);
  assert(num_face<nb_faces_bord() || n1==-1 || n2==-1); // Verifie que num_face est bien une face de bord reelle ou virtuelle
  int k = orientation_[num_face];
  double dist;
  if (n1!=-1)
    if (k != 1)
      dist = xv_(num_face,k) - xp_(n1,k);
    else
      {
        double d_teta = xv_(num_face,1) - xp_(n1,1);
        if (d_teta < 0)
          d_teta += 2.0*M_PI;
        dist = d_teta*xp_(n1,0);
      }
  else if (k != 1)
    dist = xp_(n2,k) - xv_(num_face,k);
  else
    {
      double d_teta = xp_(n2,1) - xv_(num_face,1);
      if (d_teta < 0)
        d_teta += 2.0*M_PI;
      dist = d_teta*xp_(n2,0);
    }
  return dist;
}

// Fonction de calcul de la distance entre les centres de 2 faces
// de meme orientation utilisable en coordonnees cylindriques et cartesiennes

// Description:
inline double Zone_VDF::distance_face(int n1, int n2, int k) const
{
  double dist,d_teta;
  assert ( (orientation_[n1]==orientation_[n2]) );
  if ( (axi!=1) || (k!=1) )
    dist = xv_(n2,k) - xv_(n1,k);
  else
    {
      d_teta = xv_(n2,1) - xv_(n1,1);
      if (d_teta < 0)
        d_teta += 2.0*M_PI;
      dist = d_teta*xv_(n1,0);
    }
  return dist;
}

// Fonction de calcul de la distance normale pour une face quelconque
// utilisable en coordonnees cylindriques et cartesiennes

// Description:
inline double Zone_VDF::distance_normale(int num_face) const
{

  double dist,d_teta;
  int n1 = face_voisins_(num_face,0);
  int n2 = face_voisins_(num_face,1);
  int k = orientation_[num_face];
  if ((n1!=-1) && (n2!=-1))
    {
      if ( (k!=1) || (axi!=1) )
        dist = xp_(n2,k) - xp_(n1,k);
      else
        {
          d_teta = xp_(n2,1) - xp_(n1,1);
          if (d_teta < 0)
            d_teta += 2.0*M_PI;
          dist = d_teta*xp_(n1,0);
        }
    }
  else if (n1!=-1)
    {
      if ( (k!=1) || (axi!=1) )
        dist = (xv_(num_face,k) - xp_(n1,k));
      else
        {
          d_teta = xv_(num_face,1) - xp_(n1,1);
          if (d_teta < 0)
            d_teta += 2.0*M_PI;
          dist = d_teta*xp_(n1,0);
        }
    }
  else
    {
      if ( (k!=1) || (axi!=1) )
        dist = (xp_(n2,k) - xv_(num_face,k));
      else
        {
          d_teta = xp_(n2,1) - xv_(num_face,1);
          if (d_teta < 0)
            d_teta += 2.0*M_PI;
          dist = d_teta*xp_(n2,0);
        }
    }
  return dist;
}

// Description:
inline int Zone_VDF::nb_aretes_joint() const
{
  return nb_aretes_joint_;
}

// Description:
inline int Zone_VDF::nb_aretes_coin() const
{
  return nb_aretes_coin_;
}

// Description:
inline int Zone_VDF::premiere_arete_coin() const
{
  return nb_aretes_joint_;
}

// Description:
inline int Zone_VDF::nb_aretes_bord() const
{
  return nb_aretes_bord_;
}

// Description:
inline int Zone_VDF::premiere_arete_bord() const
{
  return nb_aretes_joint_+ nb_aretes_coin_;
}

// Description:
inline int Zone_VDF::nb_aretes_mixtes() const
{
  return nb_aretes_mixtes_;
}

// Description:
inline int Zone_VDF::premiere_arete_mixte() const
{
  return nb_aretes_ - nb_aretes_mixtes_ - nb_aretes_internes_;
}

// Description:
inline int Zone_VDF::nb_aretes_internes() const
{
  return nb_aretes_internes_;
}

// Description:
inline int Zone_VDF::nb_aretes() const
{
  return nb_aretes_;
}

// Description:
inline int Zone_VDF::premiere_arete_interne() const
{
  return nb_aretes_ - nb_aretes_internes_;
}

// Description:
inline double Zone_VDF::h_x() const
{
  return h_x_;
}

// Description:
inline double Zone_VDF::h_y() const
{
  return h_y_;
}

// Description:
inline double Zone_VDF::h_z() const
{
  return h_z_;
}

// Description:
inline double Zone_VDF::dist_elem(int n1, int n2, int k) const
{
  return xp_(n2,k)-xp_(n1,k);
}

// Description:
inline double Zone_VDF::dist_elem_period(int n1, int n2, int k) const
{
  return xp_(n2,k) - xv_(elem_faces(n2,k),k)
         + xv_(elem_faces(n1,k+dimension),k) - xp_(n1,k);
}

// Description:
inline double Zone_VDF::dim_elem(int n1, int k) const
{
  return xv_(elem_faces_(n1,k+dimension),k)-xv_(elem_faces_(n1,k),k);
}

// Description:
inline double Zone_VDF::dim_face(int n1, int k) const
{
  int elem = std::max(face_voisins_(n1,0), face_voisins_(n1,1));
  return dim_elem(elem, k);
}

// Description:
inline double Zone_VDF::delta_C(int elem) const
{
  double dist= 1;
  for (int i=0; i<dimension; i++)
    dist *= dim_elem(elem,i);
  return pow(dist,1./3.);
}

// Description:
inline int Zone_VDF::amont_amont(int num_face, int i) const
{
  int k=orientation_[num_face];
  int num_elem = face_voisins_(num_face,i);
  int face = elem_faces_(num_elem,k+i*dimension);
  return face_voisins_(face,i);
}

// Description:
inline int Zone_VDF::face_amont_princ(int num_face, int i) const
{
  int ori=orientation(num_face);
  int elem=face_voisins_(num_face,i);
  if(elem !=-1)
    elem=elem_faces_(elem,ori+i*dimension);
  return elem;
}

// Description:
inline int Zone_VDF::face_amont_conj(int num_face, int k, int i) const
{
  int ori = orientation(num_face);
  int elem = face_voisins_(num_face,1);
  int face_conj=-2,face,elem_bis=-2;

  if(elem != -1)
    {
      face = elem_faces_(elem, k+i*dimension);
      elem_bis = face_voisins_(face,i);
      if (elem_bis != -1)
        face_conj = elem_faces_(elem_bis, ori);
      else
        face_conj = -1;
    }
  if ((elem==-1) || (elem_bis==-1))
    {
      elem = face_voisins_(num_face,0);
      if(elem != -1)
        {
          face = elem_faces_(elem, k+i*dimension);
          elem_bis = face_voisins_(face,i);
          if (elem_bis != -1)
            face_conj = elem_faces_(elem_bis, ori+dimension);
          else
            face_conj = -1;
        }
    }
  assert(face_conj!=-2);
  return face_conj;
}

// Description:
// Determine la face voisine de notre face
// en prevoyant que cette derniere puisse etre de type bord.
inline int Zone_VDF::face_bord_amont(int num_face, int k, int i) const
{
  int ori = orientation(num_face);
  int elem = face_voisins_(num_face,1);
  if(elem != -1)
    {
      int face = elem_faces_(elem, k+i*dimension);
      int elem_bis = face_voisins_(face,i);
      if (elem_bis != -1)
        elem = elem_faces_(elem_bis, ori);
      else
        elem = -1;
    }
  if (elem == -1)
    {
      elem = face_voisins_(num_face,0);
      if(elem != -1)
        {
          int face = elem_faces_(elem, k+i*dimension);
          int elem_bis = face_voisins_(face,i);
          if (elem_bis != -1)
            elem = elem_faces_(elem_bis, ori+dimension);
          else
            elem = -1;
        }
    }
  return elem;
}

// Description:
inline int Zone_VDF::elem_voisin(int elem, int face , int indic) const
{
  int ori = orientation_(face);
  return face_voisins_(elem_faces_(elem,ori+indic*dimension),indic);
}

// Description:
// Fonction de calcul utilisable uniquement en coordonnees cartesiennes
// de la distance entre le centre d'une face et
// le centre de face_voisins(face,0)
inline double Zone_VDF::dist_face_elem0(int num_face,int n0) const

{
  int ori = orientation_[num_face];
  return xv_(num_face,ori) - xp_(n0,ori);
}

// Description:
// Fonction de calcul utilisable uniquement en coordonnees cartesiennes
// de la distance entre le centre d'une face et
// le centre de face_voisins(face,1)
inline double Zone_VDF::dist_face_elem1(int num_face,int n1) const
{
  int ori = orientation_[num_face];
  return xp_(n1,ori) - xv_(num_face,ori);
}

// Description:
// Fonction de calcul utilisable uniquement en coordonnees cylindriques
// de la distance entre le centre d'une face et
// le centre de face_voisins(face,0)
inline double Zone_VDF::dist_face_elem0_axi(int num_face,int n0) const
{
  int ori = orientation_[num_face];
  double dist;
  if (ori!=1)
    dist = xv_(num_face,ori) - xp_(n0,ori);
  else
    {
      double d_teta = xv_(num_face,1) - xp_(n0,1);
      if (d_teta < 0)
        d_teta += 2.0*M_PI;
      dist = d_teta*xp_(n0,0);
    }
  return dist;
}

// Description:
// Fonction de calcul utilisable uniquement en coordonnees cylindriques
// de la distance entre le centre d'une face et le centre
// de face_voisins(face,1)
inline double Zone_VDF::dist_face_elem1_axi(int num_face,int n1) const
{
  int ori = orientation_[num_face];
  double dist;
  if (ori!=1)
    dist = xp_(n1,ori) - xv_(num_face,ori);
  else
    {
      double d_teta = xp_(n1,1) - xv_(num_face,1);
      if (d_teta < 0)
        d_teta += 2.0*M_PI;
      dist = d_teta*xp_(n1,0);
    }
  return dist;
}

inline double Zone_VDF::dist_norm_period(int num_face,double l) const
{
  int n1 = face_voisins_(num_face,0);
  int n2 = face_voisins_(num_face,1);
  int k = orientation_[num_face];
  return (xp_(n2,k) + l - xp_(n1,k));
}

inline double Zone_VDF::dist_face_elem0_period(int num_face,int n0,double l) const
{
  int ori = orientation_[num_face];
  double dist = xv_(num_face,ori) - xp_(n0,ori);
  if (dist > 0)
    return dist;
  else
    return dist + l;
}

inline double Zone_VDF::dist_face_elem1_period(int num_face,int n1,double l) const
{
  int ori = orientation_[num_face];
  double dist = xp_(n1,ori) - xv_(num_face,ori);
  if (dist > 0)
    return dist;
  else
    return dist + l;
}

inline double Zone_VDF::dist_face_period(int fac1 , int fac2, int k) const
{
  const Domaine& le_domaine = zone().domaine();
  const DoubleTab& coord_sommets = le_domaine.coord_sommets();
  double dist= std::fabs(coord_sommets(face_sommets(fac1,1),k)-xv_(fac1,k));
  dist += std::fabs(xv_(fac2,k) - coord_sommets(face_sommets(fac2,0),k));
  return dist;

}

#endif




