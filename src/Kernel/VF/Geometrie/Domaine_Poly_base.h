/****************************************************************************
* Copyright (c) 2024, CEA
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

#ifndef Domaine_Poly_base_included
#define Domaine_Poly_base_included

#include <Domaine_Poly_tools.h>
#include <Static_Int_Lists.h>
#include <Elem_poly_base.h>
#include <Elem_poly_base.h>
#include <TRUST_Deriv.h>
#include <TRUSTLists.h>
#include <Periodique.h>
#include <Domaine_VF.h>
#include <TRUSTTrav.h>
#include <Conds_lim.h>
#include <Domaine.h>
#include <Lapack.h>
#include <math.h>
#include <vector>
#include <string>
#include <array>
#include <map>

class Geometrie;

/*! @brief class Domaine_Poly_base
 *
 *  	Classe instanciable qui derive de Domaine_VF.
 *  	Cette classe contient les informations geometriques que demande
 *  	la methode des Volumes Elements Finis (element de Crouzeix-Raviart)
 *  	La classe porte un certain nombre d'informations concernant les faces
 *  	Dans cet ensemble de faces on fait figurer aussi les faces du bord et
 *       des joints. Pour manipuler les faces on distingue 2 categories:
 *            - les faces non standard qui sont sur un joint, un bord ou qui sont
 *              internes tout en appartenant a un element du bord
 *            - les faces standard qui sont les faces internes n'appartenant pas
 *              a un element du bord
 *       Cette distinction correspond au traitement des conditions aux limites:les
 *       faces standard ne "voient pas" les conditions aux limites.
 *       L'ensemble des faces est numerote comme suit:
 *            - les faces qui sont sur un Domaine_joint apparaissent en premier
 *     	       (dans l'ordre du vecteur les_joints)
 *    	     - les faces qui sont sur un Domaine_bord apparaissent ensuite
 * 	       (dans l'ordre du vecteur les_bords)
 *   	     - les faces internes non standard apparaissent ensuite
 *            - les faces internes standard en dernier
 *       Finalement on trouve regroupees en premier toutes les faces non standard
 *       qui vont necessiter un traitement particulier
 *       On distingue deux types d'elements
 *            - les elements non standard : ils ont au moins une face de bord
 *            - les elements standard : ils n'ont pas de face de bord
 *       Les elements standard (resp. les elements non standard) ne sont pas ranges
 *       de maniere consecutive dans l'objet Domaine. On utilise le tableau
 *       rang_elem_non_std pour acceder de maniere selective a l'un ou
 *       l'autre des types d'elements
 *
 */
class Domaine_Poly_base : public Domaine_VF
{
  Declare_base(Domaine_Poly_base);
public :
  void typer_elem(Domaine& domaine_geom) override;
  void discretiser() override;
  virtual void calculer_volumes_entrelaces() { }
  void discretiser_aretes();

  void orthocentrer();

  inline const DoubleVect& longueur_aretes() const { return longueur_aretes_; }
  inline const DoubleTab& ta() const { return ta_; }

  void reordonner(Faces&) override;
  void modifier_pour_Cl(const Conds_lim& ) override { }

  inline const Elem_poly_base& type_elem() const { return type_elem_.valeur(); }
  inline int nb_elem_Cl() const { return nb_elem() - nb_elem_std_; }
  inline int nb_faces_joint() const { return 0; /* return nb_faces_joint_;    A FAIRE */ }
  inline int nb_faces_std() const { return nb_faces_std_; }
  inline int nb_elem_std() const { return nb_elem_std_; }
  inline double carre_pas_du_maillage() const { return h_carre; }
  inline double carre_pas_maille(int i) const { return h_carre_(i); }
  inline IntVect& rang_elem_non_std() { return rang_elem_non_std_; }
  inline const IntVect& rang_elem_non_std() const { return rang_elem_non_std_; }
  inline int oriente_normale(int face_opp, int elem2)const;

  void calculer_h_carre();

  inline DoubleTab& volumes_entrelaces_dir() { return volumes_entrelaces_dir_; } // renvoie le tableau des volumes entrelaces par cote.
  inline const DoubleTab& volumes_entrelaces_dir() const { return volumes_entrelaces_dir_; }

  //equivalent de dot(), mais pour le produit (a - ma).nu.(b - mb)
  inline double nu_dot(const DoubleTab* nu, int e, int n, const double *a, const double *b, const double *ma = nullptr, const double *mb = nullptr) const;

  inline double dist_norm(int num_face) const override;
  inline double dist_norm_bord(int num_face) const override;
  DoubleVect& dist_norm_bord(DoubleVect& , const Nom& nom_bord) const;
  inline double dist_face_elem0(int num_face,int n0) const override;
  inline double dist_face_elem1(int num_face,int n1) const override;
  inline double dist_face_elem0_period(int num_face,int n0,double l) const override;
  inline double dist_face_elem1_period(int num_face,int n1,double l) const override;

  void detecter_faces_non_planes() const;

  //faces "equivalentes" : equiv(f, 0/1, i) = face equivalente a e_f(f_e(f, 0/1), i) de l'autre cote, -1 si il n'y en a pas
  const IntTab& equiv() const;

  //connectivite sommet-elements
  const Static_Int_Lists& som_elem() const;

  //indexation dans des tableaux de type (element, sommet) et (element, arete)
  const IntTab& elem_som_d() const; //entree du sommet les_elems(e, i) de l'element e : elem_som_d()(e) + i
  const IntTab& elem_arete_d() const; //entree de l'arete elem_arete(e, i) de l'element e : elem_arete_d()(e) + i

  //pour chaque element, repartition de son volume entre chacun de ses sommets
  const DoubleTab& vol_elem_som() const;
  //pour chaque sommet, produit porosite * volume
  const DoubleTab& pvol_som(const DoubleVect& poro) const;

  //som_arete[som1][som2 > som1] -> arete correspondant a (som1, som2)
  std::vector<std::map<int, int> > som_arete;

  //MD_Vectors pour Champ_Elem_PolyMAC (elems + faces) et pour Champ_Face_PolyMAC (faces + aretes)
  mutable MD_Vector mdv_elems_faces, mdv_faces_aretes;

protected:
  double h_carre = DMAXFLOAT;			 // carre du pas du maillage
  DoubleVect h_carre_;			// carre du pas d'une maille
  OWN_PTR(Elem_poly_base) type_elem_;                  // type de l'element de discretisation

  Sortie& ecrit(Sortie& os) const;

  mutable IntTab equiv_;
  mutable Static_Int_Lists som_elem_;
  mutable IntTab elem_som_d_, elem_arete_d_;
  mutable DoubleTab vol_elem_som_, pvol_som_;

  DoubleVect longueur_aretes_; //longueur des aretes
  mutable DoubleTab ta_;       //vecteurs tangents aux aretes
};

inline int Domaine_Poly_base::oriente_normale(int face_opp, int elem2) const
{
  if(face_voisins(face_opp,0)==elem2)
    return 1;
  else return -1;
}

/* equivalent du dist_norm_bord du VDF */
inline double Domaine_Poly_base::dist_norm_bord(int f) const
{
  assert(face_voisins(f, 1) == -1);
  return std::fabs(dot(&xp_(face_voisins(f, 0), 0), &face_normales_(f, 0), &xv_(f, 0))) / face_surfaces(f);
}

inline double Domaine_Poly_base::dist_norm(int f) const
{
  return std::fabs(dot(&xp_(face_voisins(f, 0), 0), &face_normales_(f, 0), &xp_(face_voisins(f, 1), 0))) / face_surfaces(f);
}

inline double Domaine_Poly_base::dist_face_elem0(int f,int e) const
{
  return std::fabs(dot(&xp_(e, 0), &face_normales_(f, 0), &xv_(f, 0))) / face_surfaces(f);
}

inline double Domaine_Poly_base::dist_face_elem1(int f,int e) const
{
  return std::fabs(dot(&xp_(e, 0), &face_normales_(f, 0), &xv_(f, 0))) / face_surfaces(f);
}

inline double Domaine_Poly_base::dist_face_elem0_period(int num_face,int n0,double l) const
{
  abort();
  return 0;
}

inline double Domaine_Poly_base::dist_face_elem1_period(int num_face,int n1,double l) const
{
  abort();
  return 0;
}

//renvoie le produit scalaire a.nu.b quelle que soient le nombre de composantes et le type de tenseur de nu
inline double Domaine_Poly_base::nu_dot(const DoubleTab* nu, int e, int n, const double *a, const double *b, const double *ma, const double *mb) const
{
  if (!nu) return dot(a, b, ma, mb);
  int d, db, D = dimension;
  double resu = 0;
  if (nu->nb_dim() == 2) resu += (*nu)(e, n) * dot(a, b, ma, mb); //isotrope
  else if (nu->nb_dim() == 3)
    for (d = 0; d < D; d++) //anisotrope diagonal
      resu += (*nu)(e, n, d) * (a[d] - (ma ? ma[d] : 0)) * (b[d] - (mb ? mb[d] : 0));
  else for (d = 0; d < D; d++)
      for (db = 0; db < D; db++)
        resu += (*nu)(e, n, d, db) * (a[d] - (ma ? ma[d] : 0)) * (b[db] - (mb ? mb[db] : 0));
  return resu;
}

#endif /* Domaine_Poly_base_included */
