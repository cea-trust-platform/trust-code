
/****************************************************************************
* Copyright (c) 2023, CEA
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

#ifndef Domaine_PolyMAC_included
#define Domaine_PolyMAC_included

#include <Echange_global_impose.h>
#include <Neumann_sortie_libre.h>
#include <Domaine_Poly_base.h>
#include <Matrice_Morse_Sym.h>
#include <Neumann_homogene.h>
#include <Elem_PolyMAC.h>
#include <SolveurSys.h>
#include <Periodique.h>
#include <Dirichlet.h>
#include <Symetrie.h>

#ifndef __clang__
#pragma GCC diagnostic ignored "-Wmaybe-uninitialized"
#endif

class Domaine_PolyMAC : public Domaine_Poly_base
{
  Declare_instanciable(Domaine_PolyMAC);
public :
  void typer_elem(Domaine& domaine_geom) override;
  void discretiser() override;
  void swap(int, int, int) { }
  void modifier_pour_Cl(const Conds_lim& ) override;

  inline double carre_pas_maille(int i) const { return h_carre_(i); }
  inline const DoubleVect& longueur_aretes() const { return longueur_aretes_; }
  inline const DoubleTab& ta() const { return ta_; }
  inline const IntTab& arete_faces() const { return arete_faces_; }

  void calculer_volumes_entrelaces();

  void calculer_h_carre();

  inline double dot (const double *a, const double *b, const double *ma = NULL, const double *mb = NULL) const;
  inline std::array<double, 3> cross(int dima, int dimb, const double *a, const double *b, const double *ma = NULL, const double *mb = NULL) const;

  IntVect cyclic; // cyclic(i) = 1 i le poly i est cyclique

  void orthocentrer();

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

  //stabilisation d'une matrice de masse mimetique en un element : dans PolyMAC -> m1 ou m2
  inline void ajouter_stabilisation(DoubleTab& M, DoubleTab& N) const;
  inline int W_stabiliser(DoubleTab& W, DoubleTab& R, DoubleTab& N, int *ctr, double *spectre) const;

  //matrice mimetique d'un champ aux faces : (valeur normale aux faces) -> (integrale lineaire sur les lignes brisees)
  void init_m2() const;
  mutable IntTab m2d, m2i, m2j, w2i, w2j; //stockage: lignes de M_2^e dans m2i([m2d(e), m2d(e + 1)[), indices/coeffs de ces lignes dans (m2j/m2c)[m2i(i), m2i(i+1)[
  mutable DoubleTab m2c, w2c;             //          avec le coeff diagonal en premier (facilite Echange_contact_PolyMAC)
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

  //MD_Vectors pour Champ_P0_PolyMAC (elems + faces) et pour Champ_Face_PolyMAC (faces + aretes)
  mutable MD_Vector mdv_elems_faces, mdv_faces_aretes;

  //std::map permettant de retrouver le couple (proc, item local) associe a un item virtuel pour le mdv_elem_faces
  void init_virt_ef_map() const;
  mutable std::map<std::array<int, 2>, int> virt_ef_map;

private:
  DoubleVect longueur_aretes_; //longueur des aretes

  void remplir_elem_faces() override;
  void creer_faces_virtuelles_non_std();
  void init_m2_new() const;
  void init_m2_osqp() const;

  mutable IntTab arete_faces_; //connectivite face -> aretes
  mutable DoubleTab ta_;       //vecteurs tangents aux aretes
};

/* produit scalaire de deux vecteurs */
inline double Domaine_PolyMAC::dot(const double *a, const double *b, const double *ma, const double *mb) const
{
  double res = 0;
  for (int i = 0; i < dimension; i++) res += (a[i] - (ma ? ma[i] : 0)) * (b[i] - (mb ? mb[i] : 0));
  return res;
}

/* produit vectoriel de deux vecteurs (toujours 3D, meme en 2D) */
inline std::array<double, 3> Domaine_PolyMAC::cross(int dima, int dimb, const double *a, const double *b, const double *ma, const double *mb) const
{
  std::array<double, 3> va = {{ 0, 0, 0 }}, vb = {{ 0, 0, 0 }}, res;
  for (int i = 0; i < dima; i++) va[i] = a[i] - (ma ? ma[i] : 0);
  for (int i = 0; i < dimb; i++) vb[i] = b[i] - (mb ? mb[i] : 0);
  for (int i = 0; i < 3; i++) res[i] = va[(i + 1) % 3] * vb[(i + 2) % 3] - va[(i + 2) % 3] * vb[(i + 1) % 3];
  return res;
}

#endif /* Domaine_PolyMAC_included */
