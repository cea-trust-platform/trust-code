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

#ifndef Domaine_VEF_included
#define Domaine_VEF_included

#include <TRUSTArray_kokkos.tpp>
#include <TRUSTTab_kokkos.tpp>
#include <Elem_VEF_base.h>
#include <TRUST_Deriv.h>
#include <Domaine_VF.h>
#include <kokkos++.h>

class VEF_discretisation;
class Geometrie;

/*! @brief class Domaine_VEF
 *
 *          Classe instanciable qui derive de Domaine_VF.
 *          Cette classe contient les informations geometriques que demande la methode des Volumes Elements Finis (element de Crouzeix-Raviart)
 *          La classe porte un certain nombre d'informations concernant les faces
 *          Dans cet ensemble de faces on fait figurer aussi les faces du bord et des joints. Pour manipuler les faces on distingue 2 categories:
 *            - les faces non standard qui sont sur un joint, un bord ou qui sont
 *              internes tout en appartenant a un element du bord
 *            - les faces standard qui sont les faces internes n'appartenant pas
 *              a un element du bord
 *       Cette distinction correspond au traitement des conditions aux limites:les faces standard ne "voient pas" les conditions aux limites.
 *       L'ensemble des faces est numerote comme suit:
 *            - les faces qui sont sur un Domaine_joint apparaissent en premier
 *                    (dans l'ordre du vecteur les_joints)
 *                 - les faces qui sont sur un Domaine_bord apparaissent ensuite
 *                (dans l'ordre du vecteur les_bords)
 *                - les faces internes non standard apparaissent ensuite
 *            - les faces internes standard en dernier
 *       Finalement on trouve regroupees en premier toutes les faces non standard qui vont necessiter un traitement particulier
 *       On distingue deux types d'elements
 *            - les elements non standard : ils ont au moins une face de bord
 *            - les elements standard : ils n'ont pas de face de bord
 *       Les elements standard (resp. les elements non standard) ne sont pas ranges de maniere consecutive dans l'objet Domaine. On utilise le tableau
 *       rang_elem_non_std pour acceder de maniere selective a l'un ou l'autre des types d'elements
 */
class Domaine_VEF: public Domaine_VF
{
  Declare_instanciable(Domaine_VEF);
public:
  void discretiser() override;
  virtual void discretiser_suite(const VEF_discretisation&);
  void discretiser_arete();

  void construire_ok_arete();
  int lecture_ok_arete();
  void verifie_ok_arete(int) const;
  void construire_renum_arete_perio(const Conds_lim&);

  virtual void creer_tableau_p1bulle(Array_base&, RESIZE_OPTIONS opt = RESIZE_OPTIONS::COPY_INIT) const;

  void swap(int, int, int);
  void reordonner(Faces&) override;
  void modifier_pour_Cl(const Conds_lim&) override;
  void typer_elem(Domaine&) override;
  void calculer_volumes_entrelaces();
  void calculer_h_carre();
  DoubleTab& vecteur_face_facette();

  inline const Elem_VEF_base& type_elem() const { return type_elem_.valeur(); }
  inline int nb_elem_Cl() const { return nb_elem() - nb_elem_std_; }
  inline int nb_faces_joint() const { return 0; }
  inline int nb_faces_std() const { return nb_faces_std_; }
  inline int nb_elem_std() const { return nb_elem_std_; }
  inline int premiere_face_std() const { return nb_faces() - nb_faces_std_; }
  inline int nb_faces_non_std() const { return nb_faces() - nb_faces_std_; }
  inline double carre_pas_du_maillage() const { return h_carre; }
  inline const DoubleVect& carre_pas_maille() const { return h_carre_; }
  inline DoubleTab& facette_normales() { return facette_normales_; }
  inline const DoubleTab& facette_normales() const { return facette_normales_; }
  inline IntVect& rang_elem_non_std() { return rang_elem_non_std_; }
  inline const IntVect& rang_elem_non_std() const { return rang_elem_non_std_; }
  inline int oriente_normale(int face_opp, int elem2) const { return (face_voisins(face_opp, 0) == elem2) ? 1 : -1; }

  inline double volume_au_sommet(int som) const { return volumes_som[som]; }
  inline const DoubleVect& volume_aux_sommets() const { return volumes_som; }
  inline int get_P1Bulle() const { assert(P1Bulle != -1); return P1Bulle; }
  inline int get_alphaE() const { assert(alphaE != -1); return alphaE; }
  inline int get_alphaS() const { assert(alphaS != -1); return alphaS; }
  inline int get_alphaA() const { assert(alphaA != -1); return alphaA; }
  inline int get_alphaRT() const { assert(alphaRT != -1); return alphaRT; }
  inline int get_modif_div_face_dirichlet() const { assert(modif_div_face_dirichlet != -1); return modif_div_face_dirichlet; }
  inline int get_cl_pression_sommet_faible() const { assert(cl_pression_sommet_faible != -1); return cl_pression_sommet_faible; }
  inline const ArrOfInt& get_renum_arete_perio() const { return renum_arete_perio; }
  inline const IntVect& get_ok_arete() const { return ok_arete; }
  inline const DoubleVect& get_volumes_aretes() const { return volumes_aretes; }

  inline virtual const MD_Vector& md_vector_p1b() const { assert(md_vector_p1b_.non_nul()); return md_vector_p1b_; }

  inline int numero_premier_element() const;
  inline int numero_premier_sommet() const;
  inline int numero_premiere_arete() const;

  inline double dist_face_elem0(int num_face,int n0) const override;
  inline double dist_face_elem1(int num_face,int n1) const override;

private:
  double h_carre = 1.e30;                         // carre du pas du maillage
  DoubleVect h_carre_;                        // carre du pas d'une maille
  OWN_PTR(Elem_VEF_base) type_elem_;                  // type de l'element de discretisation
  DoubleTab facette_normales_;          // normales aux faces des volumes entrelaces
  DoubleTab vecteur_face_facette_;                // vecteur centre face->centre facette
  IntVect orientation_;


  DoubleVect volumes_som, volumes_aretes;
  ArrOfInt renum_arete_perio;
  IntVect ok_arete;

  int P1Bulle = -1, alphaE = -1, alphaS = -1, alphaA = -1;
  int alphaRT = -1; // pour trio statio
  int modif_div_face_dirichlet = -1;
  int cl_pression_sommet_faible = -1; // determine si les cl de pression sont imposees de facon faible ou forte -> voir divergence et assembleur
  // Descripteur pour les tableaux p1b (selon alphaE, alphaS et alphaA) (construit dans Domaine_VEF::discretiser())
  MD_Vector md_vector_p1b_;

  Sortie& ecrit(Sortie& os) const;
};

// Fonction Kokkos hors classe: En effet, sinon avec dom_VEF.oriente_normale(...), une instance de Domaine_VEF est copiee du host au device !
KOKKOS_INLINE_FUNCTION int oriente_normale(int face_opp, int elem2, CIntTabView face_voisins)
{
  return (face_voisins(face_opp, 0) == elem2) ? 1 : -1;
}

// Methode pour tester:
void exemple_champ_non_homogene(const Domaine_VEF&, DoubleTab&);

inline int Domaine_VEF::numero_premier_element() const
{
  if (!alphaE)
    return -1;
  else
    return 0;
}
inline int Domaine_VEF::numero_premier_sommet() const
{
  if (!alphaS)
    return -1;
  else if (!alphaE)
    return 0;
  else
    return nb_elem_tot();
}
inline int Domaine_VEF::numero_premiere_arete() const
{
  if (!alphaA)
    return -1;
  else if (!alphaE && !alphaS)
    return 0;
  else if (!alphaE && alphaS)
    return nb_som_tot();
  else if (!alphaS && alphaE)
    return nb_elem_tot();
  else
    return nb_elem_tot() + nb_som_tot();
}

inline double Domaine_VEF::dist_face_elem0(int f,int e) const
{
  return std::fabs(dot(&xp_(e, 0), &face_normales_(f, 0), &xv_(f, 0))) / face_surfaces(f);
}

inline double Domaine_VEF::dist_face_elem1(int f,int e) const
{
  return std::fabs(dot(&xp_(e, 0), &face_normales_(f, 0), &xv_(f, 0))) / face_surfaces(f);
}
#endif
