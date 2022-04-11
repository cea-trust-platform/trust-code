
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
// File:        Zone_Poly_base.h
// Directory:   $TRUST_ROOT/src/PolyMAC_V2/Zones
// Version:     1
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Zone_Poly_base_included
#define Zone_Poly_base_included

#include <Champ_front_var_instationnaire.h>
#include <Static_Int_Lists.h>
#include <Elem_poly_base.h>
#include <Elem_poly.h>
#include <TRUSTLists.h>
#include <Periodique.h>
#include <TRUSTTrav.h>
#include <Conds_lim.h>
#include <Zone_VF.h>
#include <Domaine.h>
#include <Lapack.h>
#include <math.h>
#include <vector>
#include <string>
#include <array>
#include <map>

#ifndef __clang__
#pragma GCC diagnostic ignored "-Wmaybe-uninitialized"
#endif

class Geometrie;

//
// .DESCRIPTION class Zone_Poly_base
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


class Zone_Poly_base : public Zone_VF
{

  Declare_base(Zone_Poly_base);

public :
  void typer_elem(Zone& zone_geom) override;
  void discretiser() override;
  void reordonner(Faces&) override;
  void modifier_pour_Cl(const Conds_lim& ) override { };

  inline const Elem_poly& type_elem() const;
  inline int nb_elem_Cl() const;
  inline int nb_faces_joint() const;
  inline int nb_faces_std() const;
  inline int nb_elem_std() const;
  inline double carre_pas_du_maillage() const;
  inline double carre_pas_maille(int i) const
  {
    return h_carre_(i);
  };
  inline double face_normales(int,int ) const override;

  inline DoubleTab& face_normales();
  inline const DoubleTab& face_normales() const;
  inline IntVect& rang_elem_non_std();
  inline const IntVect& rang_elem_non_std() const;
  inline int oriente_normale(int face_opp, int elem2)const;
  inline const ArrOfInt& ind_faces_virt_non_std() const;

  void calculer_h_carre();

  inline DoubleTab& volumes_entrelaces_dir();
  inline const DoubleTab& volumes_entrelaces_dir() const;
  //produit scalaire (a - ma).(b - mb)
  inline double dot (const double *a, const double *b, const double *ma = NULL, const double *mb = NULL) const;
  //equivalent de dot(), mais pour le produit (a - ma).nu.(b - mb)
  inline double nu_dot(const DoubleTab* nu, int e, int n, const double *a, const double *b, const double *ma = NULL, const double *mb = NULL) const;
  //produit vectoriel
  inline std::array<double, 3> cross(int dima, int dimb, const double *a, const double *b, const double *ma = NULL, const double *mb = NULL) const;

  inline double dist_norm(int num_face) const;
  inline double dist_norm_bord(int num_face) const;
  DoubleVect& dist_norm_bord(DoubleVect& , const Nom& nom_bord) const;
  inline double dist_face_elem0(int num_face,int n0) const;
  inline double dist_face_elem1(int num_face,int n1) const;
  inline double dist_face_elem0_period(int num_face,int n0,double l) const;
  inline double dist_face_elem1_period(int num_face,int n1,double l) const;

  void detecter_faces_non_planes() const;

  //quelles structures optionelles on a initialise
  mutable std::map<std::string, int> is_init;
  //faces "equivalentes" : equiv(f, 0/1, i) = face equivalente a e_f(f_e(f, 0/1), i) de l'autre cote, -1 si il n'y en a pas
  void init_equiv() const;
  mutable IntTab equiv;

  void init_som_elem() const;
  mutable Static_Int_Lists som_elem;

protected:
  double h_carre;			 // carre du pas du maillage
  DoubleVect h_carre_;			// carre du pas d'une maille
  Elem_poly type_elem_;                  // type de l'element de discretisation
  DoubleTab face_normales_;             // normales aux faces
  int nb_faces_std_;                    // nombre de faces standard
  int nb_elem_std_;                     // nombre d'elements standard
  IntVect rang_elem_non_std_;		 // rang_elem_non_std_= -1 si l'element est standard
  // rang_elem_non_std_= rang de l'element dans les tableaux
  // relatifs aux elements non standards

  ArrOfInt ind_faces_virt_non_std_;      // contient les indices des faces virtuelles non standard
  void remplir_elem_faces() override {};
  Sortie& ecrit(Sortie& os) const;
  void creer_faces_virtuelles_non_std();
};

// Fonctions inline

// Decription:
// renvoie le type d'element utilise.
inline const Elem_poly& Zone_Poly_base::type_elem() const
{
  return type_elem_;
}

// Decription:
// renvoie la composante comp de la surface normale a la face.
inline double Zone_Poly_base::face_normales(int face,int comp) const
{
  return face_normales_(face,comp);
}

// Decription:
// renvoie le tableau des surfaces normales.
inline DoubleTab& Zone_Poly_base::face_normales()
{
  return face_normales_;
}

// Decription:
// renvoie le tableau des surfaces normales.
inline const DoubleTab& Zone_Poly_base::face_normales() const
{
  return face_normales_;
}

// Decription:
// renvoie le tableau des volumes entrelaces par cote.
inline DoubleTab& Zone_Poly_base::volumes_entrelaces_dir()
{
  return volumes_entrelaces_dir_;
}

// Decription:
// renvoie le tableau des surfaces normales.
inline const DoubleTab& Zone_Poly_base::volumes_entrelaces_dir() const
{
  return volumes_entrelaces_dir_;
}


// Decription:
inline IntVect& Zone_Poly_base::rang_elem_non_std()
{
  return rang_elem_non_std_;
}

// Decription:
inline const IntVect& Zone_Poly_base::rang_elem_non_std() const
{
  return rang_elem_non_std_;
}


// Decription:
inline int Zone_Poly_base::nb_faces_joint() const
{
  return 0;
  //    return nb_faces_joint_;    A FAIRE
}

// Decription:
inline int Zone_Poly_base::nb_faces_std() const
{
  return nb_faces_std_;
}

// Decription:
inline int  Zone_Poly_base::nb_elem_std() const
{
  return nb_elem_std_;
}

// Decription:
inline int Zone_Poly_base::nb_elem_Cl() const
{
  return nb_elem() - nb_elem_std_;
}


// Decription:
inline double Zone_Poly_base::carre_pas_du_maillage() const
{
  return h_carre;
}

// Decription:
inline int Zone_Poly_base::oriente_normale(int face_opp, int elem2) const
{
  if(face_voisins(face_opp,0)==elem2)
    return 1;
  else return -1;
}


// Decription:
// Renvoie le tableau des indices des faces virtuelles non standard
//inline const VECT(ArrOfInt)& Zone_Poly_base::faces_virt_non_std() const
//{
//  return faces_virt_non_std_;
//}

// Decription:
// Renvoie le tableau des indices des faces distantes non standard
inline const ArrOfInt& Zone_Poly_base::ind_faces_virt_non_std() const
{
  return ind_faces_virt_non_std_;
}

/* produit scalaire de deux vecteurs */
inline double Zone_Poly_base::dot(const double *a, const double *b, const double *ma, const double *mb) const
{
  double res = 0;
  for (int i = 0; i < dimension; i++) res += (a[i] - (ma ? ma[i] : 0)) * (b[i] - (mb ? mb[i] : 0));
  return res;
}

/* produit vectoriel de deux vecteurs (toujours 3D, meme en 2D) */
inline std::array<double, 3> Zone_Poly_base::cross(int dima, int dimb, const double *a, const double *b, const double *ma, const double *mb) const
{
  std::array<double, 3> va = {{ 0, 0, 0 }}, vb = {{ 0, 0, 0 }}, res;
  for (int i = 0; i < dima; i++) va[i] = a[i] - (ma ? ma[i] : 0);
  for (int i = 0; i < dimb; i++) vb[i] = b[i] - (mb ? mb[i] : 0);
  for (int i = 0; i < 3; i++) res[i] = va[(i + 1) % 3] * vb[(i + 2) % 3] - va[(i + 2) % 3] * vb[(i + 1) % 3];
  return res;
}

/* produit matricel et transposee de DoubleTab */
static inline DoubleTab prod(DoubleTab a, DoubleTab b)
{
  int i, j, k, m = a.dimension(0), n = a.dimension(1), p = b.dimension(1);
  assert(n == b.dimension(0));
  DoubleTab r(m, p);
  for (i = 0; i < m; i++) for (j = 0; j < n; j++) for (k = 0; k < p; k++) r(i, k) += a(i, j) * b(j, k);
  return r;
}
static inline DoubleTab transp(DoubleTab a)
{
  int i, j, m = a.dimension(0), n = a.dimension(1);
  DoubleTab r(n, m);
  for (i = 0; i < m; i++) for (j = 0; j < n; j++) r(j, i) = a(i, j);
  return r;
}

/* minimise ||M.x - b||_2, met le noyau de M dans P et retourne le residu */
static inline double kersol(const DoubleTab& M, DoubleTab& b, double eps, DoubleTab *P, DoubleTab& x, DoubleTab& S)
{
  int i, nk, m = M.dimension(0), n = M.dimension(1), k = std::min(m, n), l = std::max(m, n), w = 5 * l, info, iP, jP;
  double res2 = 0;
  char a = 'A';
  //lapack en mode Fortran -> on decompose en fait Mt!!
  DoubleTab A = M, U(m, m), Vt(n, n), W(w), iS(n, m);
  S.resize(k);
  F77NAME(dgesvd)(&a, &a, &n, &m, A.addr(), &n, S.addr(), Vt.addr(), &n, U.addr(), &m, W.addr(), &w, &info);
  for (i = 0, nk = n; i < k && S(i) > eps * S(0); i++) nk--;
  if (P) P->resize(n, nk);
  for (i = 0, jP = -1; i < n; i++) if (i < k && S(i) > eps * S(0)) iS(i, i) = 1 / S(i); //terme diagonal de iS
    else if (P) for (iP = 0, jP++; iP < n; iP++) (*P)(iP, jP) = Vt(i, iP); //colonne de V -> colonne de P
  x = prod(transp(Vt), prod(iS, prod(transp(U), b)));
  DoubleTab res = prod(M, x);
  for (i = 0; i < m; i++) res2 += std::pow(res(i, 0) - b(i, 0), 2);
  return sqrt(res2);
}

/* equivalent du dist_norm_bord du VDF */
inline double Zone_Poly_base::dist_norm_bord(int f) const
{
  assert(face_voisins(f, 1) == -1);
  return std::fabs(dot(&xp_(face_voisins(f, 0), 0), &face_normales_(f, 0), &xv_(f, 0))) / face_surfaces(f);
}

inline double Zone_Poly_base::dist_norm(int f) const
{
  return std::fabs(dot(&xp_(face_voisins(f, 0), 0), &face_normales_(f, 0), &xp_(face_voisins(f, 1), 0))) / face_surfaces(f);
}

inline double Zone_Poly_base::dist_face_elem0(int f,int e) const
{
  return std::fabs(dot(&xp_(e, 0), &face_normales_(f, 0), &xv_(f, 0))) / face_surfaces(f);
}

inline double Zone_Poly_base::dist_face_elem1(int f,int e) const
{
  return std::fabs(dot(&xp_(e, 0), &face_normales_(f, 0), &xv_(f, 0))) / face_surfaces(f);
}

inline double Zone_Poly_base::dist_face_elem0_period(int num_face,int n0,double l) const
{
  abort();
  return 0;
}

inline double Zone_Poly_base::dist_face_elem1_period(int num_face,int n1,double l) const
{
  abort();
  return 0;
}

//renvoie le produit scalaire a.nu.b quelle que soient le nombre de composantes et le type de tenseur de nu
inline double Zone_Poly_base::nu_dot(const DoubleTab* nu, int e, int n, const double *a, const double *b, const double *ma, const double *mb) const
{
  if (!nu) return dot(a, b, ma, mb);
  int d, db, D = dimension;
  double resu = 0;
  if (nu->nb_dim() == 2) resu += (*nu)(e, n) * dot(a, b, ma, mb); //isotrope
  else if (nu->nb_dim() == 3) for (d = 0; d < D; d++) //anisotrope diagonal
      resu += (*nu)(e, n, d) * (a[d] - (ma ? ma[d] : 0)) * (b[d] - (mb ? mb[d] : 0));
  else for (d = 0; d < D; d++) for (db = 0; db < D; db++)
        resu += (*nu)(e, n, d, db) * (a[d] - (ma ? ma[d] : 0)) * (b[db] - (mb ? mb[db] : 0));
  return resu;
}

/* compaction d'un tableau qui avait set_smart_resize = 1 */
#define CRIMP(a) a.nb_dim() > 2 ? a.resize(a.dimension(0) + 1, a.dimension(1), a.dimension(2)) : a.nb_dim() > 1 ? a.resize(a.dimension(0) + 1, a.dimension(1)) : a.resize(a.dimension(0) + 1), \
        a.set_smart_resize(0), \
        a.nb_dim() > 2 ? a.resize(a.dimension(0) - 1, a.dimension(1), a.dimension(2)) : a.nb_dim() > 1 ? a.resize(a.dimension(0) - 1, a.dimension(1)) : a.resize(a.dimension(0) - 1)
#endif
