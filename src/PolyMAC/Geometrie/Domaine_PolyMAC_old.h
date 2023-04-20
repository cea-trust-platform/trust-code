
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
// File:        Domaine_PolyMAC_old.h
// Directory:   $TRUST_ROOT/src/PolyMAC_old/Domaines
// Version:     1
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Domaine_PolyMAC_old_included
#define Domaine_PolyMAC_old_included

#include <Champ_front_var_instationnaire.h>
#include <Echange_global_impose.h>
#include <Neumann_sortie_libre.h>
#include <Matrice_Morse_Sym.h>
#include <Static_Int_Lists.h>
#include <Neumann_homogene.h>
#include <Elem_PolyMAC_old.h>
#include <TRUSTLists.h>
#include <SolveurSys.h>
#include <Periodique.h>
#include <Dirichlet.h>
#include <Conds_lim.h>
#include <TRUSTTrav.h>
#include <Symetrie.h>
#include <Domaine_VF.h>
#include <Domaine.h>
#include <Lapack.h>
#include <vector>
#include <string>
#include <math.h>
#include <array>
#include <map>
#include <Domaine_Poly_tools.h>

#ifndef __clang__
#pragma GCC diagnostic ignored "-Wmaybe-uninitialized"
#endif

class Geometrie;

/*! @brief class Domaine_PolyMAC_old
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
 *
 *
 */


class Domaine_PolyMAC_old : public Domaine_VF
{

  Declare_instanciable(Domaine_PolyMAC_old);

public :
  void typer_elem(Domaine& domaine_geom) override;
  void discretiser() override;
  void swap(int, int, int);
  void reordonner(Faces&) override;
  void modifier_pour_Cl(const Conds_lim& ) override;

  inline const Elem_PolyMAC_old& type_elem() const;
  inline int nb_elem_Cl() const;
  inline int nb_faces_joint() const;
  inline int nb_faces_std() const;
  inline int nb_elem_std() const;
  inline double carre_pas_du_maillage() const;
  inline double carre_pas_maille(int i) const
  {
    return h_carre_(i);
  };
  inline const DoubleVect& longueur_aretes() const
  {
    return longueur_aretes_;
  };
  inline const DoubleTab& ta() const
  {
    return ta_;
  };
  inline const IntTab& arete_faces() const
  {
    return arete_faces_;
  };
  inline IntVect& rang_elem_non_std();
  inline const IntVect& rang_elem_non_std() const;
  inline int oriente_normale(int face_opp, int elem2)const;
  inline const ArrOfInt& ind_faces_virt_non_std() const;
  void calculer_volumes_entrelaces();

  void calculer_h_carre();

  inline DoubleTab& volumes_entrelaces_dir();
  inline const DoubleTab& volumes_entrelaces_dir() const;
  inline double dot (const double *a, const double *b, const double *ma = NULL, const double *mb = NULL) const;
  inline std::array<double, 3> cross(int dima, int dimb, const double *a, const double *b, const double *ma = NULL, const double *mb = NULL) const;

  inline double dist_norm(int num_face) const override;
  inline double dist_norm_bord(int num_face) const override;
  DoubleVect& dist_norm_bord(DoubleVect& , const Nom& nom_bord) const;
  inline double dist_face_elem0(int num_face,int n0) const override;
  inline double dist_face_elem1(int num_face,int n1) const override;
  inline double dist_face_elem0_period(int num_face,int n0,double l) const override;
  inline double dist_face_elem1_period(int num_face,int n1,double l) const override;

  IntVect cyclic; // cyclic(i) = 1 i le poly i est cyclique

  void orthocentrer();
  void detecter_faces_non_planes() const;

  //som_arete[som1][som2 > som1] -> arete correspondant a (som1, som2)
  std::vector<std::map<int, int> > som_arete;

  //quelles structures optionelles on a initialise
  mutable std::map<std::string, int> is_init;
  //interpolations d'ordre 1 du vecteur vitesse aux elements
  void init_ve() const;
  mutable IntTab vedeb, veji; //reconstruction de ve par (veji, veci)[vedeb(e), vedeb(e + 1)[ (faces)
  mutable DoubleTab veci;

  //rotationnel aux faces d'un champ tangent aux aretes
  void init_rf() const;
  mutable IntTab rfdeb, rfji; //reconstruction du rotationnel par (rfji, rfci)[rfdeb(f), rfdeb(f + 1)[ (champ aux aretes)
  mutable DoubleTab rfci;

  //stabilisation d'une matrice de masse mimetique en un element : dans PolyMAC_old -> m1 ou m2
  inline void ajouter_stabilisation(DoubleTab& M, DoubleTab& N) const;
  inline int W_stabiliser(DoubleTab& W, DoubleTab& R, DoubleTab& N, int *ctr, double *spectre) const;

  //matrice mimetique d'un champ aux faces : (valeur normale aux faces) -> (integrale lineaire sur les lignes brisees)
  void init_m2() const;
  mutable IntTab m2d, m2i, m2j, w2i, w2j; //stockage: lignes de M_2^e dans m2i([m2d(e), m2d(e + 1)[), indices/coeffs de ces lignes dans (m2j/m2c)[m2i(i), m2i(i+1)[
  mutable DoubleTab m2c, w2c;             //          avec le coeff diagonal en premier (facilite Echange_contact_PolyMAC_old)
  void init_m2solv() const; //pour resoudre m2.v = s
  mutable Matrice_Morse_Sym m2mat;
  mutable SolveurSys m2solv;

  //interpolation aux elements d'ordre 1 d'un champ defini par ses composantes tangentes aux aretes (ex. : la vorticite)
  inline void init_we() const;
  void init_we_2d() const;
  void init_we_3d() const;
  mutable IntTab wedeb, weji; //reconstruction de we par (weji, weci)[wedeb(e), wedeb(e + 1)[ (sommets en 2D, aretes en 3D)
  mutable DoubleTab weci;

  //matrice mimetique d'un champ aux aretes : (valeur tangente aux aretes) -> (flux a travers l'union des facettes touchant l'arete)
  void init_m1() const;
  void init_m1_2d() const;
  void init_m1_3d() const;
  mutable IntTab m1deb, m1ji; //reconstruction de m1 par (m1ji(.,0), m1ci)[m1deb(a), m1deb(a + 1)[ (sommets en 2D, aretes en 3D); m1ji(.,1) contient le numero d'element
  mutable DoubleTab m1ci;

  //MD_Vectors pour Champ_P0_PolyMAC_old (elems + faces) et pour Champ_Face_PolyMAC_old (faces + aretes)
  mutable MD_Vector mdv_elems_faces, mdv_faces_aretes;

  //std::map permettant de retrouver le couple (proc, item local) associe a un item virtuel pour le mdv_elem_faces
  void init_virt_ef_map() const;
  mutable std::map<std::array<int, 2>, int> virt_ef_map;

private:
  double h_carre;			 // carre du pas du maillage
  DoubleVect h_carre_;			// carre du pas d'une maille
  Elem_PolyMAC_old type_elem_;                  // type de l'element de discretisation
  DoubleVect longueur_aretes_; //longueur des aretes
  int nb_faces_std_;                    // nombre de faces standard
  int nb_elem_std_;                     // nombre d'elements standard
  IntVect rang_elem_non_std_;		 // rang_elem_non_std_= -1 si l'element est standard
  // rang_elem_non_std_= rang de l'element dans les tableaux
  // relatifs aux elements non standards

  ArrOfInt ind_faces_virt_non_std_;      // contient les indices des faces virtuelles non standard
  void remplir_elem_faces() override;
  Sortie& ecrit(Sortie& os) const;
  void creer_faces_virtuelles_non_std();

  mutable IntTab arete_faces_; //connectivite face -> aretes
  mutable DoubleTab ta_;       //vecteurs tangents aux aretes
};

// Fonctions inline

// Decription:
// renvoie le type d'element utilise.
inline const Elem_PolyMAC_old& Domaine_PolyMAC_old::type_elem() const
{
  return type_elem_;
}

// Decription:
// renvoie le tableau des volumes entrelaces par cote.
inline DoubleTab& Domaine_PolyMAC_old::volumes_entrelaces_dir()
{
  return volumes_entrelaces_dir_;
}

// Decription:
// renvoie le tableau des surfaces normales.
inline const DoubleTab& Domaine_PolyMAC_old::volumes_entrelaces_dir() const
{
  return volumes_entrelaces_dir_;
}


// Decription:
inline IntVect& Domaine_PolyMAC_old::rang_elem_non_std()
{
  return rang_elem_non_std_;
}

// Decription:
inline const IntVect& Domaine_PolyMAC_old::rang_elem_non_std() const
{
  return rang_elem_non_std_;
}


// Decription:
inline int Domaine_PolyMAC_old::nb_faces_joint() const
{
  return 0;
  //    return nb_faces_joint_;    A FAIRE
}

// Decription:
inline int Domaine_PolyMAC_old::nb_faces_std() const
{
  return nb_faces_std_;
}

// Decription:
inline int  Domaine_PolyMAC_old::nb_elem_std() const
{
  return nb_elem_std_;
}

// Decription:
inline int Domaine_PolyMAC_old::nb_elem_Cl() const
{
  return nb_elem() - nb_elem_std_;
}


// Decription:
inline double Domaine_PolyMAC_old::carre_pas_du_maillage() const
{
  return h_carre;
}

// Decription:
inline int Domaine_PolyMAC_old::oriente_normale(int face_opp, int elem2) const
{
  if(face_voisins(face_opp,0)==elem2)
    return 1;
  else return -1;
}


// Decription:
// Renvoie le tableau des indices des faces virtuelles non standard
//inline const ArrsOfInt& Domaine_PolyMAC_old::faces_virt_non_std() const
//{
//  return faces_virt_non_std_;
//}

// Decription:
// Renvoie le tableau des indices des faces distantes non standard
inline const ArrOfInt& Domaine_PolyMAC_old::ind_faces_virt_non_std() const
{
  return ind_faces_virt_non_std_;
}

/* produit scalaire de deux vecteurs */
inline double Domaine_PolyMAC_old::dot(const double *a, const double *b, const double *ma, const double *mb) const
{
  double res = 0;
  for (int i = 0; i < dimension; i++) res += (a[i] - (ma ? ma[i] : 0)) * (b[i] - (mb ? mb[i] : 0));
  return res;
}

/* produit vectoriel de deux vecteurs (toujours 3D, meme en 2D) */
inline std::array<double, 3> Domaine_PolyMAC_old::cross(int dima, int dimb, const double *a, const double *b, const double *ma, const double *mb) const
{
  std::array<double, 3> va = {{ 0, 0, 0 }}, vb = {{ 0, 0, 0 }}, res;
  for (int i = 0; i < dima; i++) va[i] = a[i] - (ma ? ma[i] : 0);
  for (int i = 0; i < dimb; i++) vb[i] = b[i] - (mb ? mb[i] : 0);
  for (int i = 0; i < 3; i++) res[i] = va[(i + 1) % 3] * vb[(i + 2) % 3] - va[(i + 2) % 3] * vb[(i + 1) % 3];
  return res;
}

/* equivalent du dist_norm_bord du VDF */
inline double Domaine_PolyMAC_old::dist_norm_bord(int f) const
{
  assert(face_voisins(f, 1) == -1);
  return std::fabs(dot(&xp_(face_voisins(f, 0), 0), &face_normales_(f, 0), &xv_(f, 0))) / face_surfaces(f);
}

inline double Domaine_PolyMAC_old::dist_norm(int f) const
{
  return std::fabs(dot(&xp_(face_voisins(f, 0), 0), &face_normales_(f, 0), &xp_(face_voisins(f, 1), 0))) / face_surfaces(f);
}

inline double Domaine_PolyMAC_old::dist_face_elem0(int f,int e) const
{
  return std::fabs(dot(&xp_(e, 0), &face_normales_(f, 0), &xv_(f, 0))) / face_surfaces(f);
}

inline double Domaine_PolyMAC_old::dist_face_elem1(int f,int e) const
{
  return std::fabs(dot(&xp_(e, 0), &face_normales_(f, 0), &xv_(f, 0))) / face_surfaces(f);
}

inline double Domaine_PolyMAC_old::dist_face_elem0_period(int num_face,int n0,double l) const
{
  abort();
  return 0;
}

inline double Domaine_PolyMAC_old::dist_face_elem1_period(int num_face,int n1,double l) const
{
  abort();
  return 0;
}

#endif
